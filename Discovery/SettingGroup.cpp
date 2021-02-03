#include "SettingGroup.h"

#include <iostream>

SettingGroup::SettingGroup(std::string GroupName)
	: name(GroupName)
{
}

SettingGroup::SettingGroup(std::string GroupName, Image GroupImage)
	: name(GroupName)
	, groupImage(GroupImage)
{
}

void SettingGroup::AddSetting(Setting setting)
{
	settings.emplace(setting.SettingName(), setting);
}

std::map<std::string,Setting> SettingGroup::GetSettings()
{
	return settings;
}

nlohmann::json SettingGroup::ToJSON()
{
	nlohmann::json json;
	json["settingGroup"]["name"] = name;
	if (groupImage.Height() != 0 && groupImage.Width() != 0)
	{
		json["settingGroup"]["img"] = groupImage.ToJSON();
	}
	for (std::map<std::string,Setting>::iterator it = settings.begin(); it != settings.end(); it++)
	{	
		json["settingGroup"]["settings"].push_back(it->second.ToJSON());
	}
	if (HasStatus())
	{
		json["settingGroup"]["status"] = GetGroupStatus();
	}
	return json;
	
}

void SettingGroup::ClearSettings()
{
	settings.clear();
}

bool SettingGroup::HasSetting(std::string setting)
{
	return (settings.find(setting) != settings.end());
}
	
Setting SettingGroup::GetSetting(std::string settingName)
{
	if (HasSetting(settingName))
		return settings[settingName];
	return Setting();
}
	
bool SettingGroup::GetSetting(Setting &setting)
{
	if (settings.find(setting.SettingName()) == settings.end())
		return false;
	setting = settings[setting.SettingName()];
	return true;
}
	
void SettingGroup::SetName(std::string name)
{
	this->name = name;
}
	
void SettingGroup::SetImage(Image image)
{
	groupImage = image;
}
	
SettingGroup SettingGroup::FromJSON(nlohmann::json json)
{
	SettingGroup group;
	if (json.contains("settingGroup"))
	{
		if (json["settingGroup"].contains("name"))
			group.SetName(json["settingGroup"]["name"]);
		if (json["settingGroup"].contains("img"))
			group.SetImage(Image::FromJSON(json["settingGroup"]["img"]));
		if (json["settingGroup"].contains("settings"))
		{
			for (
				nlohmann::json::iterator it = json["settingGroup"]["settings"].begin();
				it != json["settingGroup"]["settings"].end();
				it++)
			{
				group.AddSetting(Setting::FromJSON(it.value()));
			}
		}			
	}
	
	return group;
}

bool SettingGroup::SetSettingValue(std::string name, std::string value)
{
	if (HasSetting(name))
	{
		settings[name].SetValue(value);
		return true;
	}
	return false;
}

bool SettingGroup::SetSettingValue(std::string name, int value)
{
	if (HasSetting(name))
	{
		settings[name].SetValue(value);
		return true;
	}
	return false;
}
bool SettingGroup::SetSettingValue(std::string name, bool value)
{
	if (HasSetting(name))
	{
		settings[name].SetValue(value);
		return true;
	}
	return false;
}
bool SettingGroup::SetSettingValue(std::string name, float value)
{
	if (HasSetting(name))
	{
		settings[name].SetValue(value);
		return true;
	}
	return false;
}
bool SettingGroup::SetSettingValue(std::string name, std::vector<std::string> value)
{
	if (HasSetting(name))
	{
		settings[name].SetValue(value);
		return true;
	}
	return false;
}

bool SettingGroup::HasStatus()
{
	for (std::map<std::string, Setting>::iterator it = settings.begin(); it != settings.end(); it++)
		if (it->second.SettingType() == Setting::eSTATUS)
			return true;
	return false;
}

Setting::eStatusType SettingGroup::GetGroupStatus()
{
	// 0 unknown, 1 green, 2 yellow 3 red;
	int val = 0;
	if (HasStatus())
	{
		for (std::map<std::string, Setting>::iterator it = settings.begin(); it != settings.end(); it++)
			if (it->second.SettingType() == Setting::eSTATUS)
			{
				switch (it->second.GetStatusVal())
				{
				case Setting::RED:
					val = 3;
					break;
				case Setting::YELLOW:
					if (val < 2)
						val = 2;
					break;
				case Setting::GREEN:
					if (val < 1)
						val = 1;
					break;
				default:
					break;
				}
			}
		if (val == 3)
			return Setting::RED;
		if (val == 2)
			return Setting::YELLOW;
		if (val == 1)
			return Setting::GREEN;
	}
	
	return Setting::UNKNOWN;
}