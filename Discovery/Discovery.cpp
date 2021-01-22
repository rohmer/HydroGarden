#include "Discovery.h"

Discovery::Discovery(
		Pistache::Rest::Router *eprouter,
		std::function<std::map<std::string, Setting>(std::map<std::string,Setting>)> settingGetFunction,
		std::function<std::map<std::string, bool>(std::map<std::string,Setting>)> settingSetFunction
	) :
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
		AddEndpoint(Endpoint("/settings", Endpoint::PUT), 
		std::bind(&Discovery::setSettings, this, placeholders::_1, placeholders::_2)
	);
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
	for (nlohmann::json::iterator it = settings.begin(); it != settings.end(); it++)
	{
		
	}
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
	if (endpoint.endpointMethod == Endpoint::BOTH || endpoint.endpointMethod == Endpoint::PUT)
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