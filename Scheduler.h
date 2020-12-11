#pragma once
#include <functional>
#include <unistd.h>
#include <string>
#include <thread>
#include <map>
#include <chrono>
#include <vector>

#include "Settings.h"
#include "Logger.h"
#include "Hardware.h"

class Scheduler
{
private:
	
	std::thread schedulerThread;
	
	void schedulerLoop();
	bool feedingTime();
	void doFeeding();
	bool shutdown = false;
	
	bool lightOn = false, pumpOn = false, feeding = false;
	bool lightOverride = false;
	Scheduler();
	
	static Scheduler *instance;
	void init();
	
	time_t pumpToggleRunOff = -1;
	
public:
	static Scheduler *GetInstance();
	~Scheduler();	
	
	time_t GetNextLightOn();
	time_t GetNextLightOff();
	time_t GetNextFeeding();
	time_t GetNextPumpOn();
	time_t GetNextPumpOff();
	time_t GetFeedDuration();
	bool IsLightOn(time_t now);
	void SetLight(bool isOn);

	bool IsPumpOn(time_t now);
	
	bool IsFeeding(time_t now)
	{
		return feeding;
	}
	
	void DoPumpToggleRun();
	
	bool GetLight()
	{ 
		return lightOn;
	}
	bool GetPump()
	{
		return pumpOn;
	}
};

