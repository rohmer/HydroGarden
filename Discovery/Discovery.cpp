#include "Discovery.h"

Discovery::Discovery(
		int discoveryPort,
		int applicationID,
		bool canBeDiscovered,
		bool canDiscover,
		std::string appName,
		nlohmann::json appVersion,
		Pistache::Rest::Router *eprouter,
		std::function<std::map<std::string, Setting>(std::map<std::string,Setting>)> settingGetFunction,
		std::function<std::map<std::string, bool>(std::map<std::string,Setting>)> settingSetFunction
	) :
	appName(appName),
	appVersion(appVersion),
	router(eprouter),
	settingGetter(settingGetFunction),
	settingSetter(settingSetFunction)
{
	AddEndpoint(Endpoint("/discovery", Endpoint::GET), 
		std::bind(&Discovery::discovery, this,placeholders::_1, placeholders::_2)
	);		
	if(settingGetter!=nullptr)
		AddEndpoint(Endpoint("/settings", Endpoint::GET),
		std::bind(&Discovery::getSettings, this, placeholders::_1, placeholders::_2)
	);
	if (settingSetter != nullptr)
		AddEndpoint(Endpoint("/settings", Endpoint::POST), 
		std::bind(&Discovery::setSettings, this, placeholders::_1, placeholders::_2)
	);
	if (canBeDiscovered || canDiscover)
	{
		initDiscovery(discoveryPort, applicationID, canBeDiscovered, canDiscover);
	}
}

void Discovery::initDiscovery(int port, int appId, bool canBeDiscovered, bool canDiscover)
{
	udpdiscovery::PeerParameters parameters;
	parameters.set_port(port);
	parameters.set_application_id(appId);
	parameters.set_can_be_discovered(canBeDiscovered);
	parameters.set_can_discover(canDiscover);
	peer.Start(parameters, toJSON().dump());
}

nlohmann::json Discovery::DiscoverPeers()
{
	std::list<udpdiscovery::DiscoveredPeer> new_discovered_peers = peer.ListDiscovered();
	nlohmann::json json;
	for (std::list<udpdiscovery::DiscoveredPeer>::iterator it = new_discovered_peers.begin(); it != new_discovered_peers.end(); it++)
	{
		try
		{
			json["peers"].push_back(nlohmann::json::parse(it->user_data()));
		}
		catch (const std::exception&)
		{
		}
	}
	
	return json;
}
	
Pistache::Rest::Route::Result Discovery::getSettings(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response)
{
	std::map<std::string, Setting> settingValues = settingGetter(settings);
	nlohmann::json json;
	for (std::map<std::string, Setting>::iterator it = settingValues.begin();
		it != settingValues.end();
		it++)
	{
		json["settings"].push_back(it->second.ToJSON());
	}
	response.send(Pistache::Http::Code::Ok, json.dump());
	return Pistache::Rest::Route::Result::Ok;
}

Pistache::Rest::Route::Result Discovery::setSettings(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response)
{
	std::string body = request.body();
	nlohmann::json json;
	
	try 
	{
		json = json.parse(body);
	}
	catch (std::exception)
	{
		return Pistache::Rest::Route::Result::Failure;
	}
	if (!json.contains("settings"))
		return Pistache::Rest::Route::Result::Failure;
	nlohmann::json set = json["settings"];
	std::map<std::string, Setting> lSettings;
	for (auto &setting : set)
	{	
		try
		{
			Setting pset = Setting::FromJSON(setting);
			if(pset.SettingName()!="")
				lSettings.emplace(pset.SettingName(), pset);
		}
		catch (const std::exception&)
		{
			//TODO: Figure out how to deal with errors in lib
			// eat it for now
		}
	}
	
	struct sSetResult
	{
		sSetResult(std::string name, std::string result, bool valSet)
			: settingName(name)
			, disposition(result)
			, valueSet(valSet)
		{
		}
		std::string settingName;
		std::string disposition;
		bool valueSet;
	};
	std::vector<sSetResult> results;
	std::map<std::string, Setting> settingsToSet;
	
	for (std::map<std::string, Setting>::iterator it = lSettings.begin(); it != lSettings.end(); it++)
	{
		Setting s = it->second;
		std::string name = it->first;
		if (s.IsReadOnly())
		{
			results.push_back(sSetResult(s.SettingName(), "Read Only", false));
		}
		else
		{
			if (s.IsValidValue())
			{
				bool validType = false;
				for (std::map<std::string,Setting>::iterator iter=settings.begin(); iter!=settings.end(); iter++)
				{
					if (iter->first == name)
					{
						if (s.SettingType() == iter->second.SettingType())
							validType = true;
						break;
					}
				}
				if (validType)
				{
					settingsToSet.emplace(s.SettingName(), s);
					results.push_back(sSetResult(s.SettingName(), "Value set", true));
				}
				else
				{
					results.push_back(sSetResult(s.SettingName(), "Type Mismatch", false));
				}
			}
			else
			{
				std::stringstream ss;
				ss << "Value: " << s.GetStrVal() << " outside minimum/maximun values";
				results.push_back(sSetResult(s.SettingName(), ss.str(), false));
			}			
		}
	}
	
	settingSetter(settingsToSet);
	std::stringstream rJson;
	rJson << "{\"results\": [";
	for(std::vector<sSetResult>::iterator it = results.begin() ; it != results.end() ; it++)
	{
		rJson << "\"" << it->settingName << "\": {\"result\":\"" << it->disposition << "\"valueSet\":" << it->valueSet << "},";
	}
	rJson << "]}";
	response.send(Pistache::Http::Code::Ok, rJson.str());
	return Pistache::Rest::Route::Result::Ok;
}

void Discovery::AddEndpoint(Endpoint endpoint,
	std::function<Pistache::Rest::Route::Result(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter response)> endpointFunction)
{
	endpoints.push_back(endpoint);
	if (endpointFunction == nullptr)
		return;
	if (endpoint.endpointMethod == Endpoint::BOTH || endpoint.endpointMethod == Endpoint::GET)
	{
		Pistache::Rest::Routes::Get(*router, endpoint.endpointPath, endpointFunction);
	}
	if (endpoint.endpointMethod == Endpoint::BOTH || endpoint.endpointMethod == Endpoint::POST)
	{
		Pistache::Rest::Routes::Post(*router, endpoint.endpointPath, endpointFunction);
	}
}

Pistache::Rest::Route::Result Discovery::discovery(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response)
{
	response.send(Pistache::Http::Code::Ok, toJSON().dump());
	
	return Pistache::Rest::Route::Result::Ok;
}

nlohmann::json Discovery::toJSON()
{
	nlohmann::json json;
	json["app"]["name"] = appName;
	std::string foo = appVersion.dump();
	json["app"] = appVersion;
	json["endpoints"] = nlohmann::json::array();
	json["settings"] = nlohmann::json::array();
	int epSize = endpoints.size();
	
	for (int i = 0; i < endpoints.size(); i++)
		json["endpoints"].push_back(endpoints[i].ToJSON());
	for (std::map<std::string, Setting>::iterator it = settings.begin();
		it != settings.end();
		it++)
	{
		json["settings"].push_back(it->second.ToJSON());
	}
	return json;
}