#pragma once
#include <string>
#include <sstream>
#include <bcm2835/src/bcm2835.h>

#include "Logger.h"
#include "StateChange.h"

#define LIGHTPIN RPI_GPIO_P1_03
#define PUMPPIN RPI_GPIO_P1_05
#define FEEDPIN RPI_GPIO_P1_07
#define WATERLEVELPIN RPI_GPIO_P1_08
#define FEEDLEVELPIN RPI_GPIO_P1_10

class Hardware
{
public:	
	static void LightsOn();
	static void LightsOff();
	static void PumpsOn();
	static void PumpsOff();
	static void FeedOn();
	static void FeedOff();
	
	static bool FeedLevel();
	static bool WaterLevel();
	
	static bool LightStatus();
	static bool PumpStatus();
	static bool FeedStatus();
	
private:
	static bool init;
	static void initialize();

	static bool lightStatus, pumpStatus, feedStatus;
public:

};