#pragma once

#include <string>
#include <sstream>
#include <vector>

#include <lvgl.h>

#include "NetworkTools.h"
#include "Logger.h"
#include "Settings.h"

class NetworkUI
{
private:
	static lv_obj_t *networkWindow;	
	static void lv_win_close_event_cb(lv_obj_t *sw, lv_event_t e);
	static void lv_network_cb(lv_obj_t *sw, lv_event_t e);
	static void connect_cb(lv_obj_t *sw, lv_event_t e);
	static void cancel_cb(lv_obj_t *sw, lv_event_t e);
	
	static lv_obj_t *keyboard;
	static lv_obj_t *passwordTA;
	
public:
	static void CreateNetworkWindow();
	
};