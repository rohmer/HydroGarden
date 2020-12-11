#include "Settings.h"

Settings *Settings::instance = nullptr;

Settings::Settings()
{
	LOGI("->Settings()");
	if (!SettingsFileExists())
	{
		lightStartHour = 7;
		lightStartMin = 30;
		lightDuration = 14 * 60;
		pumpRunTime = 120;
		dailyMLFood = 4.5;
		network = "";
		SaveSettings();
	} 
	LoadSettings();
	LOGI("<-Settings()");
}

Settings* Settings::GetInstance()
{
	if (instance == nullptr)
		instance = new Settings();
	return instance;
}

void Settings::SetLightTime(uint startHour, uint startMin, uint duration)
{
	lightStartHour = startHour;
	lightStartMin = startMin;
	lightDuration = duration;
}	

void Settings::SetPumpRunTime(uint runTimeMin)
{
	pumpRunTime = runTimeMin;
}

void Settings::SetDailyMLFood(float MLOfFood)
{
	dailyMLFood = MLOfFood;
}

bool Settings::SaveSettings()
{
	std::stringstream jsonStr;
	jsonStr << ToJSON();
	
	
	try
	{
		std::ofstream settings("/usr/local/etc/settings.json");
		if (!settings.is_open())
		{
			LOGE("Failed to open settings");
			return false;
		}
		
		settings << jsonStr.rdbuf();
		
		settings.close();
		LOGI("Saved Settings");		
	}
	catch (const std::exception&)
	{
		LOGE("Error Saving Settings");
	}
	
	return true;
}

std::string Settings::ToJSON()
{
	nlohmann::json json;
	json["settings"]["LightStartHour"] = lightStartHour;
	json["settings"]["LightStartMin"] = lightStartMin;
	json["settings"]["LightDuration"] = lightDuration;
	json["settings"]["PumpRunTime"] = pumpRunTime;
	json["settings"]["DailyMLFood"] = dailyMLFood;
	json["settings"]["Network"] = network;
	std::stringstream ss;
	ss << json;
	return ss.str();
}

bool Settings::SettingsFileExists()

{
	std::ifstream sf("/usr/local/etc/settings.json");
	bool val = sf.is_open();
	if (sf.is_open())
		sf.close();
	return val;
}

bool Settings::LoadSettings()
{
	if (!SettingsFileExists())
		return false;
	std::ifstream sf("/usr/local/etc/settings.json");
	
	if (!sf.is_open())
	{
		LOGW("Settings File does NOT exist");
		return false;
	}
	
	nlohmann::json json;
	sf >> json;
	try
	{
		lightStartHour = json["settings"]["LightStartHour"];		
	}
	catch (const std::exception&)
	{
		lightStartHour = 7;
	}
	
	try
	{
		lightStartMin = json["settings"]["LightStartMin"];		
	}
	catch (const std::exception&)
	{
		lightStartMin = 0;
	}
	try
	{
		lightDuration = json["settings"]["LightDuration"];	
	}
	catch (const std::exception&)
	{
		lightDuration = 14 * 60;
	}
	try
	{	
		pumpRunTime = json["settings"]["PumpRunTime"];
	}
	catch (const std::exception&)
	{
		pumpRunTime = 120;	
	}
	try
	{
		dailyMLFood = json["settings"]["DailyMLFood"];	
	}
	catch (const std::exception&)
	{
		dailyMLFood = 4.5;
	}
	try
	{
		network = json["settings"]["Network"];	
	}
	catch (const std::exception&)
	{
		network = "";
	}
	
	LOGI("Settings loaded");
	
	return true;
}
