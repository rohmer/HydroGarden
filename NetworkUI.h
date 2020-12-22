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
	static lv_obj_t *hnSetBtn, *hnCanBtn, *hnLbl;
	static lv_obj_t *networkWindow;	
	static void lv_win_close_event_cb(lv_obj_t *sw, lv_event_t e);
	static void lv_network_cb(lv_obj_t *sw, lv_event_t e);
	static void connect_cb(lv_obj_t *sw, lv_event_t e);
	static void cancel_cb(lv_obj_t *sw, lv_event_t e);
	static void hostname_cb(lv_obj_t *sw, lv_event_t e);
	static void hostnameEdit_cb(lv_obj_t *sw, lv_event_t e);
	
	static void setHostname_cb(lv_obj_t *sw, lv_event_t e);
	static void cancelHostname_cb(lv_obj_t *sw, lv_event_t e);
	
	static lv_obj_t *keyboard;
	static lv_obj_t *passwordTA;
	static lv_obj_t *hostnameWin;
	static lv_obj_t *hostnameTA;
	
	static void updateNetworkList(lv_task_t *task);

	static lv_task_t *updateNetworkTask;
	static std::vector<std::string> networkButtons;
	static bool failedScan;
	static lv_obj_t *wPage;
	static bool updateActive;
	static lv_obj_t *pwWindow;
public:
	static void CreateNetworkWindow();
	
};