#pragma once

#include <lvgl.h>
#include <json.hpp>

#include "Setting.h"

class SettingUIString
{
public:
	static nlohmann::json ToJSON(Setting setting);
};