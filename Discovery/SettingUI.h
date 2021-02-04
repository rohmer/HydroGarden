#pragma once

#include <lvgl.h>
#include <json.hpp>

#include "Setting.h"
#include "SettingUIString.h"

// Static class for generating UI objects for display, as well as JSON objects for discovery

class SettingUI
{
public:
	static nlohmann::json ToJSON(Setting setting);
	static lv_obj_t ToObject(Setting setting);
};