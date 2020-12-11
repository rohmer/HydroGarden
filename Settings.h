#pragma once

#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <External/JSON/json.hpp>

#include "Logger.h"

#define MLPSEC 1.083

class Settings
{
private:
	Settings();
	static Settings *instance;
	
	uint lightStartHour, lightStartMin, lightDuration;
	uint pumpRunTime;
	float dailyMLFood;										// 15ml per tablespoon
	
	std::string network;
	
public:
	static Settings *GetInstance();
	
	uint LightStartHour() { return lightStartHour;}
	uint LightStartMin() { return lightStartMin;}
	uint LightDuration() { return lightDuration;}
	uint PumpRunTime() { return pumpRunTime; }
	float DailyMLFood() { return dailyMLFood; } 
	std::string ConnectedNetwork()
	{ 
		return network;
	}
	
	void SetLightTime(uint startHour, uint startMin, uint duration);
	void SetPumpRunTime(uint runTimeSec);
	void SetDailyMLFood(float MLOfFood);
	
	void SetNetwork(std::string network);
		
	bool LoadSettings();
	
	bool SaveSettings();
	bool SettingsFileExists();
	
	std::string ToJSON();
	void FromJSON(std::string val);
};