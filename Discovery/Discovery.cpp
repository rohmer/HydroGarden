#include "Discovery.h"

Discovery::Discovery(
		int discoveryPort,
	int applicationID,
	bool canBeDiscovered,
	bool canDiscover,
	std::string appName,
	nlohmann::json appVersion,
	Pistache::Rest::Router *eprouter,
	std::function<std::map<std::string,
	SettingGroup>(std::map<std::string, SettingGroup>)> settingGetFunction,
	std::function<std::map<std::string,
	bool>(std::map<std::string, SettingGroup>)> settingSetFunction,
	std::filesystem::path applicationIcon,
	std::string defaultSettingGroupName)
	: appName(appName)
	, appVersion(appVersion)
	, router(eprouter)
	, settingGetter(settingGetFunction)
	, settingSetter(settingSetFunction)
	, defaultGroup(defaultSettingGroupName)
{
	settingGroups.emplace(defaultGroup, SettingGroup(defaultGroup));
	AddEndpoint(Endpoint("/discovery", Endpoint::GET), 
		std::bind(&Discovery::discovery, this, placeholders::_1, placeholders::_2));		
	if (settingGetter != nullptr)
		AddEndpoint(Endpoint("/settings", Endpoint::GET),
			std::bind(&Discovery::getSettings, this, placeholders::_1, placeholders::_2));
	if (settingSetter != nullptr)
		AddEndpoint(Endpoint("/settings", Endpoint::POST), 
			std::bind(&Discovery::setSettings, this, placeholders::_1, placeholders::_2));
	if (applicationIcon != "")
	{
		appIcon = ImgJSON::JsonFromFile(applicationIcon);
	}
	if (canBeDiscovered || canDiscover) 
	{
		initDiscovery(discoveryPort, applicationID, canBeDiscovered, canDiscover);
	}
}

void Discovery::InitNetworkDiscovery()
{
	peer.Start(parameters, toJSON().dump());
}

void Discovery::initDiscovery(int port, int appId, bool canBeDiscovered, bool canDiscover)
{
	parameters.set_port(port);
	parameters.set_application_id(appId);
	parameters.set_can_be_discovered(canBeDiscovered);
	parameters.set_can_discover(canDiscover);
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
	std::map<std::string, SettingGroup> settingValues = settingGetter(settingGroups);
	nlohmann::json json;
	for (std::map<std::string, SettingGroup>::iterator it = settingValues.begin();
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
	std::map<std::string, SettingGroup> jsetGroup;
	for (auto &setting : set)
	{	
		try
		{
			SettingGroup pset = SettingGroup::FromJSON(setting);
			if(pset.GetName()!="")
				jsetGroup.emplace(pset.GetName(), pset);
		}
		catch (const std::exception&)
		{
			//TODO: Figure out how to deal with errors in lib
			// eat it for now
		}
	}
	
	struct sSetResult
	{
		sSetResult(std::string groupName, std::string name, std::string result, bool valSet)
			: settingGroup(groupName)
			, settingName(name)
			, disposition(result)
			, valueSet(valSet)
		{
		}
		std::string settingGroup, settingName;
		std::string disposition;
		bool valueSet;
	};
	std::vector<sSetResult> results;
	std::map<std::string, SettingGroup> toSet;
	
	for (std::map<std::string, SettingGroup>::iterator it = jsetGroup.begin();
		it != jsetGroup.end(); it++)
	{
		if (settingGroups.find(it->first) != settingGroups.end())
		{
			// This is a valid and defined setting group
			SettingGroup localSettingGroup = settingGroups[it->first];
			SettingGroup  jsonSettingGroup = it->second;
			
			// Now check all the settings in the json against our local settings
			std::map<std::string, Setting> jsonSettings = jsonSettingGroup.GetSettings();
			
			for (std::map<std::string, Setting>::iterator jit=jsonSettings.begin(); jit!=jsonSettings.end(); jit++)
			{
				Setting s;
				if (localSettingGroup.GetSetting(jit->second))
				{
					// It is a valid setting, check for min/max
					bool valid = true;
					if (s.SettingType() == Setting::eINT)
					{
						if (jit->second.GetIntVal() > s.IntMax() || jit->second.GetIntVal() < s.IntMin())
							valid = false;
					}
					if (s.SettingType() == Setting::eFLOAT)
					{
						if (jit->second.GetFloatVal() > s.FloatMin() || jit->second.GetFloatVal() < s.FloatMin())
							valid = false;
					}
					if (s.SettingType() == Setting::ePICKLIST)
					{
						if (jit->second.GetIntVal() < 0 || jit->second.GetIntVal() >= s.SettingListItems().size())
							valid = false;
					}
					if (valid == false)
					{
						results.push_back(
							sSetResult(it->first, s.SettingName(), "Outside Min/Max Value", false)
						);
					}
					else
					{
						if (toSet.find(it->first) == toSet.end())
						{
							toSet.emplace(
								it->first,
								SettingGroup(it->first)
							);	
						}
						toSet[it->first].AddSetting(s);
					}
					
				}
			}
		}
	}
	std::map<std::string, bool> r = settingSetter(toSet);
	for (std::map<std::string, bool>::iterator it = r.begin(); it != r.end(); it++)
	{
		results.push_back(
			sSetResult("", it->first, "", it->second));
	}
		
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
	json["app"] = appVersion;
	json["app"]["icon"] = appIcon;
	json["endpoints"] = nlohmann::json::array();
	json["settings"] = nlohmann::json::array();
	int epSize = endpoints.size();
	
	for (int i = 0; i < endpoints.size(); i++)
		json["endpoints"].push_back(endpoints[i].ToJSON());
	for (std::map<std::string, SettingGroup>::iterator it = settingGroups.begin(); it != settingGroups.end(); it++)
	{
		json["settings"].push_back(it->second.ToJSON());
	}
		
	return json;
}