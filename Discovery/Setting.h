#pragma once

#include <sstream>
#include <string>
#include <vector>

#include <json.hpp>

class Setting
{
public:
	enum eSettingType
	{
		eSTR,
		eINT,
		eBOOL,
		eFLOAT, 
		ePICKLIST,
		eSTRINGLIST,
		eSTATUS
	};
	
	enum eStatusType
	{
		RED,
		YELLOW, 
		GREEN,
		UNKNOWN
	};
		
	Setting() {}
	
	Setting(std::string name, std::string description, eSettingType settingType);
	
	Setting(std::string name,
		std::string description,
		eStatusType status);
	
	Setting(
		std::string name,
		std::string description,
		eSettingType settingType,
		std::string settingValue,
		bool readOnly);
	Setting(
		std::string name,
		std::string description,
		eSettingType settingType,
		int min,
		int max,
		int settingValue,
		bool readOnly);
	Setting(
		std::string name,
		std::string description,
		eSettingType settingType,
		bool settingValue,
		bool readOnly);
	Setting(
		std::string name,
		std::string description,
		eSettingType settingType,
		float min,
		float max,
		float settingValue,
		bool readOnly);
	Setting(
		std::string name,
		std::string description,
		eSettingType settingType,
		int settingValue,
		std::vector<std::string> listItems,
		bool readOnly);

	nlohmann::json ToJSON();
	static Setting FromJSON(std::string jsonStr);
	static Setting FromJSON(nlohmann::json json);
	
	bool IsReadOnly() { return readOnly; }
	eSettingType SettingType() { return settingType; }

	std::vector<std::string> SettingListItems()
	{
		return settingListItems;
	}
	std::string SettingDescription()
	{
		return settingDescription;
	}
	float FloatMax()
	{ 
		return fMax;
	}
	float FloatMin()
	{
		return fMin;
	}
	int IntMax()
	{
		return iMax;
	}
	int IntMin()
	{
		return iMin;
	}
	std::string SettingName() 
	{
		return settingName;
	}
		
	void SetValue(eStatusType status)
	{
		switch (status)
		{
		case Setting::RED:
			settingValue = "RED";
			break;
		case Setting::YELLOW:
			settingValue = "YELLOW";
			break;
		case Setting::GREEN:
			settingValue = "GREEN";
			break;
		case Setting::UNKNOWN:
			settingValue = "UNKNOWN";
			break;
		}
	}
	
	void SetValue(std::string val) 
	{
		settingValue = val;
	}
	void SetValue(float val)
	{
		std::stringstream ss;
		ss << val;
		settingValue = ss.str();
	}
	void SetValue(bool val)
	{
		if (val)
			settingValue = "1";
		else
			settingValue = "0";
	}
	void SetValue(int val)
	{
		std::stringstream ss;
		ss << val;
		settingValue = ss.str();
	}
	void SetValue(uint val)
	{
		std::stringstream ss;
		ss << val;
		settingValue = ss.str();
	}
	
	void SetValue(std::vector<std::string> val)
	{
		settingListItems = val;
	}
		
	std::vector<std::string> GetListItems()
	{
		return settingListItems;
	}
	void AddListItem(std::string listItem)
	{
		settingListItems.push_back(listItem);
	}
	
	bool GetBoolVal();
	int GetIntVal();
	float GetFloatVal();
	std::string GetStrVal();
	std::string GetListVal();
	std::vector<std::string> GetStringList();
	eStatusType GetStatusVal();
	
	bool IsValidValue();
	
private:		
	bool readOnly;
	eSettingType settingType;
	std::string settingValue, settingName;
	std::vector<std::string> settingListItems;
	std::string settingDescription;
	float fMin, fMax;
	int iMin, iMax;
};