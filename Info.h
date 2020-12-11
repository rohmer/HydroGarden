#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <sstream>

#include <json.hpp>

#include "Version.h"
#include "Scheduler.h"

class Info
{
public:
	static std::string GetInfo();
};