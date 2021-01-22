#pragma once

#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>

#include <External/JSON/json.hpp>

#include "Logger.h"

#define MLPSEC 1.083

// If the below is defined, we will send the network password as clear text on a get for each wireless network
// Clearly not super secure
#define NETWORKPASS_CLEARTEXT 1

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
	std::string hostname;
	
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
	
	std::string GetHostname();
	void SetHostname(std::string hostname);
};