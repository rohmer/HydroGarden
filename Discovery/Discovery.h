#pragma once

#include <functional>
#include <string>
#include <vector>

#include <pistache/description.h>
#include <pistache/endpoint.h>

#include "Setting.h"
#include "Endpoint.h"

using namespace std;

// libDiscovery
// This provides a way for external devices to discover settings, capabilities, etc of a device on the network
class Discovery
{
public:
	Discovery(
		Pistache::Rest::Router *router,
		std::function<std::map<std::string,Setting>(std::map<std::string, Setting>)> settingGetFunction=nullptr,
		std::function<std::map<std::string, bool>(std::map<std::string,Setting>)> settingSetFunction=nullptr
	);
	
	void AddSetting(Setting setting)
	{
		settings.emplace(setting.SettingName(),setting);
	}
	
	void AddEndpoint(Endpoint endpoint,
		std::function<Pistache::Rest::Route::Result (const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter response)> endpointFunction=nullptr);
	
private:
	Pistache::Rest::Route::Result discovery(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
	
	std::map<std::string, Setting> settings;
	std::vector<Endpoint> endpoints;
	
	Pistache::Rest::Router *router;
	std::function<std::map<std::string, Setting>(std::map<std::string,Setting>)> settingGetter;
	std::function<std::map<std::string, bool>(std::map<std::string, Setting>)> settingSetter;
	nlohmann::json toJSON();
	
	Pistache::Rest::Route::Result getSettings(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
	Pistache::Rest::Route::Result setSettings(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
};