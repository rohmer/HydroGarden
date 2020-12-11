#pragma once

#include <sstream>
#include <string>
#include <deque>
#include "json.hpp"

struct sStateChange
{
	std::string time;
	std::string change;
};

class StateChange
{
private:
	StateChange();
	static StateChange *instance;
	std::deque<sStateChange> states;
	
public:
	static StateChange *GetInstance();
	
	void Change(std::string state);
	
	std::string ToJSON();
	
	
};