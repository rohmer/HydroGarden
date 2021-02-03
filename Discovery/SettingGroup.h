#pragma once

#include <string>
#include <vector>

#include <json.hpp>
#include "Setting.h"
#include "Image.h"

// A setting group contains settings
// This will be displayed on a seperate tab on the client

class SettingGroup
{
public:
	SettingGroup() {}
	SettingGroup(std::string GroupName);
	SettingGroup(std::string GroupName, Image GroupImage);

	static SettingGroup FromJSON(nlohmann::json json);
	nlohmann::json ToJSON();
	
	void AddSetting(Setting setting);
	std::map<std::string, Setting> GetSettings();
	bool GetSetting(Setting &setting);
	Setting GetSetting(std::string SettingName);
	
	bool HasSetting(std::string setting);
	
	void ClearSettings();
	
	void SetName(std::string name);
	void SetImage(Image GroupImage);
	std::string GetName()
	{
		return name;
	}
	
	bool SetSettingValue(std::string settingName, std::string value);
	bool SetSettingValue(std::string settingName, int value);
	bool SetSettingValue(std::string settingName, float value);
	bool SetSettingValue(std::string settingName, bool value);
	bool SetSettingValue(std::string settingName, std::vector<std::string> value);
	
	bool HasStatus();
	Setting::eStatusType GetGroupStatus();
	
private:
	std::string name;
	std::map<std::string,Setting> settings;
	Image groupImage;
};