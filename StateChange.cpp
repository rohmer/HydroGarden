#include "StateChange.h"

StateChange *StateChange::instance = nullptr;

StateChange::StateChange()
{
}

StateChange *StateChange::GetInstance()
{
	if (instance == nullptr)
		instance = new StateChange();
	return instance;
}

void StateChange::Change(std::string state)
{
	time_t t = time(nullptr);
	struct tm *ts = localtime(&t);
	std::stringstream tstr;
	tstr << ts->tm_hour << ":" << ts->tm_min << ":" << ts->tm_sec;
	
	sStateChange sc;
	sc.time = tstr.str();
	sc.change = state;
	states.push_front(sc);
	while (states.size() > 100)
		states.pop_back();
}

std::string StateChange::ToJSON()
{
	nlohmann::json json;
	for (int i = 0; i < states.size(); i++)
	{
		json[i]["time"] = states[i].time;
		json[i]["state"] = states[i].change;
	}
	std::stringstream ss;
	ss << json;
	return ss.str();
}