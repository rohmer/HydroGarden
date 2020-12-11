#include "Scheduler.h"

Scheduler *Scheduler::instance = nullptr;

Scheduler::Scheduler()
{
	LOGI("->Scheulder()");
	init();	
	schedulerThread = std::thread(&Scheduler::schedulerLoop, this);
	LOGI("<-Scheduler()");
}

Scheduler::~Scheduler()
{
	LOGI("->~Scheduler()");
	shutdown = true;
	schedulerThread.join();
}

Scheduler *Scheduler::GetInstance()
{
	if (instance == nullptr)
		instance = new Scheduler();
	return instance;
		
}

void Scheduler::schedulerLoop()
{
	LOGI("->schedulerLoop()");
	bool prevLight = false;
	while (!shutdown)
	{
		time_t currentTime = time(nullptr);
		bool shouldLightBeOn = IsLightOn(currentTime);
		if (shouldLightBeOn != lightOn && !lightOverride)
		{
			lightOn = shouldLightBeOn;
			if (lightOn)
			{
				LOGI("Turning Lights On");
				Hardware::LightsOn();
			}
			else
			{
				LOGI("Turning Lights Off");
				Hardware::LightsOff();
			}
		}
			
		bool pOn = IsPumpOn(currentTime);

		if (pumpToggleRunOff != -1 && pumpToggleRunOff < currentTime)
		{
			LOGI("Turning Pumps Off");
			pumpOn = false;
			pumpToggleRunOff = -1;
			Hardware::PumpsOff();
		}
		
		if (pOn != pumpOn && pumpToggleRunOff==-1) 
		{
			pumpOn = pOn;
			if (pumpOn)
			{
				LOGI("Turning Pumps On");
				Hardware::PumpsOn();
			}
			else
			{
				LOGI("Turning Pumps Off");
				Hardware::PumpsOff();
			}
			// Change Pump Status
			
		}
		
		// Handle feeding
		if(feedingTime())
		{
			doFeeding();
		}
		usleep(1000000);

	}
	LOGI("<-schedulerLoop()");
}

void Scheduler::doFeeding()
{
	LOGI("Scheduled Feeding");
	float feedTime = Settings::GetInstance()->DailyMLFood() / MLPSEC * 1000000;
	feeding = true;
	Hardware::FeedOn();
	usleep(feedTime);
	Hardware::FeedOff();
}


void Scheduler::init()
{
	LOGI("->Scheduler::init()");
	pumpToggleRunOff = -1;
	LOGI("<-Scheduler::init()");
}

bool Scheduler::feedingTime()
{
	time_t now = time(nullptr);
	tm *fStart = localtime(&now);
	fStart->tm_hour = 0;
	fStart->tm_min = 0;
	fStart->tm_sec = 0;
	time_t fs = mktime(fStart);
	int feedingTime = round(Settings::GetInstance()->DailyMLFood() / MLPSEC);
	if (now >= fs && now < fs + feedingTime)
		return true;
	return false;
}

time_t Scheduler::GetNextLightOn()
{
	time_t now = time(nullptr);
	time_t lOn = now;
	tm *tmLOn = localtime(&now);
	
	tmLOn->tm_hour = Settings::GetInstance()->LightStartHour();
	tmLOn->tm_min = Settings::GetInstance()->LightStartMin();
	
	lOn = mktime(tmLOn);
	if (lOn < now)
		lOn += 60 * 60 * 24;
	return lOn;
}

time_t Scheduler::GetNextLightOff()
{
	time_t now = time(nullptr);
	time_t lOff = now;
	tm *tmLOff = localtime(&now);
	
	tmLOff->tm_hour = Settings::GetInstance()->LightStartHour();
	tmLOff->tm_min = Settings::GetInstance()->LightStartMin();
	
	lOff = mktime(tmLOff);
	lOff += Settings::GetInstance()->LightDuration() * 60;
	
	if (lOff < now)
		lOff += 60 * 60 * 24;
	return lOff;
}

time_t Scheduler::GetNextFeeding()
{
	time_t now = time(nullptr);
	time_t nFeed = now;
	tm *tmNfeed = localtime(&now);
	tmNfeed->tm_hour = 0;
	tmNfeed->tm_min = 0;
	tmNfeed->tm_sec = 1;
	return mktime(tmNfeed);
}

time_t Scheduler::GetNextPumpOn()
{
	time_t now = time(nullptr);
	time_t pumpOn = now;
	tm *tmnOn= localtime(&now);
	tmnOn->tm_min = 0;
	tmnOn->tm_sec = 0;
	pumpOn = mktime(tmnOn) + 60 * 60;
	return pumpOn;
}

time_t Scheduler::GetNextPumpOff()
{
	time_t now = time(nullptr);
	time_t pumpOn = now;
	tm *tmnOn = localtime(&now);
	tmnOn->tm_min = 0;
	tmnOn->tm_sec = 0;
	pumpOn = mktime(tmnOn) + 60 * 60;
	return pumpOn+Settings::GetInstance()->PumpRunTime();
}

time_t Scheduler::GetFeedDuration()
{
	float feedAmt = Settings::GetInstance()->DailyMLFood();
	return round(feedAmt / MLPSEC);
}

bool Scheduler::IsLightOn(time_t now)
{
	time_t n= time(nullptr);
	tm *lightOnTM = localtime(&n);
	lightOnTM->tm_hour = Settings::GetInstance()->LightStartHour();
	lightOnTM->tm_min = Settings::GetInstance()->LightStartMin();
	lightOnTM->tm_sec = 0;
	time_t start = mktime(lightOnTM);
	if (now >= start && now < (start + Settings::GetInstance()->LightDuration() * 60))
	{
		return true;
	}
	lightOn = false;
	return false;
}

void Scheduler::SetLight(bool isOn)
{
	if (lightOn)
	{
		LOGI("Turning Light Off");
		lightOn = false;
		Hardware::LightsOff();		
		time_t now = time(nullptr);
		if (IsLightOn(now))
			lightOverride = true;
		else
			lightOverride = false;
		return;
	}
	LOGI("Turning Light On");
	lightOn = true;
	time_t now = time(nullptr);
	if (!IsLightOn(now))
		lightOverride = true;
	else
		lightOverride = false;
	Hardware::LightsOn();
}

bool Scheduler::IsPumpOn(time_t now)	
{
	time_t n = time(NULL);
	
	if (pumpOn && pumpToggleRunOff != -1)
	{
		if (n > pumpToggleRunOff)
		{
			pumpOn = false;
			pumpToggleRunOff = -1;
			return false;
		}
		return true;
	}
	tm *pumpOn = localtime(&n);
	pumpOn->tm_min = 0;
	pumpOn->tm_sec = 0;
	time_t start = mktime(pumpOn);
	if (now >= start && now < (start + Settings::GetInstance()->PumpRunTime()))
	{
		return true;
	}
	
	return false;
}

void Scheduler::DoPumpToggleRun()
{
	if (pumpOn)
		return;
	Hardware::PumpsOn();
	time_t now = time(nullptr);
	pumpToggleRunOff = now + 61;
	pumpOn = true;
}