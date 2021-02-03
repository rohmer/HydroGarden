#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include <pistache/description.h>
#include <pistache/endpoint.h>
#include <udp_discovery_peer.hpp>

#include "Setting.h"
#include "SettingGroup.h"
#include "Endpoint.h"
#include "ImgJSON.h"

using namespace std;

// libDiscovery
// This provides a way for external devices to discover settings, capabilities, etc of a device on the network
class Discovery
{
public:
	Discovery(
		int discoveryPort,
		int applicationID,
		bool canBeDiscovered,
		bool canDiscover,
		std::string appName,
		nlohmann::json appVersion,
		Pistache::Rest::Router *router,
		std::function<std::map<std::string,SettingGroup>(std::map<std::string, SettingGroup>)> settingGetFunction=nullptr,
		std::function<std::map<std::string, bool>(std::map<std::string,SettingGroup>)> settingSetFunction=nullptr,
		std::filesystem::path applicationIcon="",
		std::string defaultGroup="settings"
	);
	
	void AddSetting(Setting setting)
	{
		AddSetting(setting, defaultGroup);
	}
	
	void AddSetting(Setting setting, std::string settingGroup)
	{
		if (settingGroups.find(settingGroup) == settingGroups.end())
			return;
		settingGroups[settingGroup].AddSetting(setting);		
	}
	
	void AddSettingGroup(std::string GroupName)
	{
		settingGroups.emplace(GroupName, SettingGroup(GroupName));
	}
	
	void AddSettingGroup(std::string GroupName, Image GroupImage)
	{
		settingGroups.emplace(GroupName, SettingGroup(GroupName, GroupImage));		
	}
		
	void AddEndpoint(Endpoint endpoint,
		std::function<Pistache::Rest::Route::Result(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter response)> endpointFunction=nullptr);
	
	nlohmann::json DiscoverPeers();
	
	SettingGroup GetSettingGroup(std::string GroupName)
	{
		if (settingGroups.find(GroupName) != settingGroups.end())
			return settingGroups[GroupName];
		return SettingGroup();
	}
	
	void InitNetworkDiscovery();
	
private:
	Pistache::Rest::Route::Result discovery(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
	udpdiscovery::PeerParameters parameters;
	
	std::string appName;
	nlohmann::json appVersion;
	nlohmann::json appIcon;
	
	std::map<std::string, SettingGroup> settings;
	std::vector<Endpoint> endpoints;
	
	Pistache::Rest::Router *router;
	std::function<std::map<std::string, SettingGroup>(std::map<std::string,SettingGroup>)> settingGetter;
	std::function<std::map<std::string, bool>(std::map<std::string, SettingGroup>)> settingSetter;
	nlohmann::json toJSON();
	
	Pistache::Rest::Route::Result getSettings(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
	Pistache::Rest::Route::Result setSettings(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
	
	udpdiscovery::Peer peer;
	std::string defaultGroup;
	
	void initDiscovery(int port, int appId, bool canBeDiscovered, bool canDiscover);
	
	std::map<std::string, SettingGroup> settingGroups;
};