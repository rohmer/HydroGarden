#pragma once

#include <ctime>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <External/JSON/json.hpp>

#include "Logger.h"

#define MLPSEC 1.083

struct sNetwork
{
	std::string ssid, password;
	bool isDefault;
};
		
class Settings
{
private:
	Settings();
	static Settings *instance;
	
	uint lightStartHour, lightStartMin, lightDuration;
	uint pumpRunTime;
	float dailyMLFood;										// 15ml per tablespoon
	
	std::map<std::string, sNetwork> networks;
	
public:
	static Settings *GetInstance();
	
	uint LightStartHour() { return lightStartHour;}
	uint LightStartMin() { return lightStartMin;}
	uint LightDuration() { return lightDuration;}
	uint PumpRunTime() { return pumpRunTime; }
	float DailyMLFood() { return dailyMLFood; } 
	
	void SetLightTime(uint startHour, uint startMin, uint duration);
	void SetPumpRunTime(uint runTimeSec);
	void SetDailyMLFood(float MLOfFood);
		
	bool LoadSettings();
	
	bool SaveSettings();
	bool SettingsFileExists();
	
	std::string ToJSON();
	void FromJSON(std::string val);
	
	void AddNetwork(std::string ssid, std::string password, bool isDefault);
	sNetwork GetNetwork(std::string ssid);
	std::vector<sNetwork> GetNetworks();
};