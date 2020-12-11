#pragma once
#include <sstream>
#include <string>
#include "Logger.h"

enum eTaskType
{
	LIGHT_ON,
	LIGHT_OFF,
	PUMP_ON,
	PUMP_OFF,
	FEEDING
};

enum eTaskSchedule
{
	HOURLY,
	DAILY
};

struct sTask
{
private:
	eTaskType taskType;
	eTaskSchedule taskSchedule;
	
public:
	eTaskType GetType()
	{
		return taskType;
	}
	
	eTaskSchedule GetSchedule()
	{
		return taskSchedule;
	}
	
	sTask(eTaskType TaskType, eTaskSchedule TaskSchedule)
		: taskType(TaskType)
		, taskSchedule(TaskSchedule)
	{
		std::string taskStr;
		switch (taskType)
		{
		case eTaskType::FEEDING:
			taskStr = "Feeding";
			break;
		case eTaskType::LIGHT_OFF:
			taskStr = "Light Off";
			break;
		case eTaskType::LIGHT_ON:
			taskStr = "Light On";
			break;
		case eTaskType::PUMP_OFF:
			taskStr = "Pump Off";
			break;
		case eTaskType::PUMP_ON:
			taskStr = "Pump On";
			break;			
		}
		
		std::string schedule = "Hourly";
		if (taskSchedule == eTaskSchedule::DAILY)
			schedule = "Daily";
		std::stringstream ss;
		ss << schedule << " Task: " << taskStr << " created";
		LOGI(ss.str());
	}
};