#include "Setting.h"

Setting::Setting(
		std::string name,
	std::string description,
	eSettingType settingType,
	std::string settingValue,
	bool readOnly)
	: settingName(name)
	, settingDescription(description)
	, settingType(settingType)
	, settingValue(settingValue)
	, readOnly(readOnly)
{
}

Setting::Setting(
		std::string name,
	std::string description,
	eSettingType settingType,
	int min,
	int max,
	int settingValue,
	bool readOnly)
	: settingName(name)
	, settingDescription(description)
	, settingType(settingType)
	, iMin(min)
	, iMax(max)
	, readOnly(readOnly)
{
	std::stringstream ss;
	ss << settingValue;
	this->settingValue = ss.str();
}

Setting::Setting(
		std::string name,
	std::string description,
	eSettingType settingType,
	float min,
	float max,
	float settingValue,
	bool readOnly)
	: settingName(name)
	, settingDescription(description)
	, settingType(settingType)
	, fMin(min)
	, fMax(max)
	, readOnly(readOnly)
{
	std::stringstream ss;
	ss << settingValue;
	this->settingValue = ss.str();
}

Setting::Setting(
		std::string name,
	std::string description,
	eSettingType settingType,
	bool settingValue,
	bool readOnly)
	: settingName(name)
	, settingDescription(description)
	, settingType(settingType)	
	, readOnly(readOnly)
{
	if (settingValue == true)
		this->settingValue = "1";
	else
		this->settingValue = "0";
}

Setting::Setting(
		std::string name,
	std::string description,
	eSettingType settingType,
	int settingValue,
	std::vector<std::string> listItems,
	bool readOnly)
	: settingName(name)
	, settingDescription(description)
	, settingType(settingType)	
	, settingListItems(listItems)
	, readOnly(readOnly)
{
	std::stringstream ss;
	ss << settingValue;
	this->settingValue = ss.str();
}

nlohmann::json Setting::ToJSON()
{
	nlohmann::json json;
	json["setting"]["name"] = settingName.c_str();
	json["setting"]["desc"] = settingDescription.c_str();
	json["setting"]["value"] = settingValue.c_str();
	json["setting"]["type"] = settingType;
	json["setting"]["readOnly"] = readOnly;
	if (settingType == Setting::eLIST)
	{
		for (int i = 0; i < settingListItems.size(); i++)
		{
			json["setting"]["listItems"][i] = settingListItems[i].c_str();
		}
	}
	if (settingType == Setting::eINT)
	{
		json["setting"]["min"] = iMin;
		json["setting"]["max"] = iMax;
	}
	if (settingType == Setting::eFLOAT)
	{
		json["setting"]["min"] = fMin;
		json["setting"]["max"] = fMax;
	}

	return json;

}
	
Setting Setting::FromJSON(std::string jsonStr)
{
	Setting setting;
	nlohmann::json json=nlohmann::json::parse(jsonStr);
	if (!json.is_object() || json.empty())
		return setting;

	if (json.contains("setting"))
	{
		if (json["setting"]["name"].is_string())
			setting.settingName = json["setting"]["name"];
		if (json["setting"]["desc"].is_string())
			setting.settingDescription = json["setting"]["desc"];
		if (json["setting"]["value"].is_string())
			setting.settingValue = json["setting"]["value"];
		if (json["setting"]["readOnly"].is_boolean())
			setting.readOnly = json["setting"]["readOnly"];
		else
			setting.readOnly = false;
		if (json["setting"].contains("type"))
		{
			if (json["setting"]["type"] == "STRING")
				setting.settingType = eSettingType::eSTR;
			if (json["setting"]["type"] == "INT")
				setting.settingType = eSettingType::eINT;
			if (json["setting"]["type"] == "BOOL")
				setting.settingType = eSettingType::eBOOL;
			if (json["setting"]["type"] == "FLOAT")
				setting.settingType = eSettingType::eFLOAT;
			if (json["setting"]["type"] == "LIST")
				setting.settingType = eSettingType::eLIST;
		}
		if (setting.settingType == eLIST)
		{
			if (json["setting"]["listItems"].is_array())
			{
				for (
					nlohmann::json::iterator it = json["setting"]["listItems"].begin();
					it != json["setting"]["listItems"].end();
					it++)
				{
					setting.settingListItems.push_back(it.value());
				}
			}
		}
		if (setting.settingType == eINT)
		{
			if (json["setting"]["min"].is_number())
				setting.iMin = json["setting"]["min"];
			if (json["setting"]["max"].is_number())
				setting.iMax = json["setting"]["max"];
		}
		if (setting.settingType == eFLOAT)
		{
			if (json["setting"]["min"].is_number())
				setting.fMin = json["setting"]["min"];
			if (json["setting"]["max"].is_number())
				setting.fMax = json["setting"]["max"];
		}
	}
	
	return setting;
}