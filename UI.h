#pragma once

#include <ctime>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <lvgl.h>
#include <display/fbdev.h>
#include <indev/evdev.h>
#include "Logger.h"
#include "Settings.h"
#include "Scheduler.h"
#include "NetworkUI.h"

#define DISP_BUF_SIZE 384000  // 480x800

class UserInterface
{
public:
	UserInterface();
	void CreateMainWindow();
	static void SetBriteness(uint brightness);
	
private:
	void init();
	static void network_click_cb(lv_obj_t *sw, lv_event_t e);
	static void log_click_event_cb(lv_obj_t *sw, lv_event_t e);
	static void close_log_cb(lv_obj_t *sw, lv_event_t e);
	static void color_chg_event_cb(lv_obj_t * sw, lv_event_t e);
	static void settings_click_event_cb(lv_obj_t *sw, lv_event_t e);
	static void close_settings_cb(lv_obj_t *sw, lv_event_t e);
	static void lightOn_cb(lv_obj_t *sw, lv_event_t e);
	static void lightDur_cb(lv_obj_t *sw, lv_event_t e);
	static void pumpDur_cb(lv_obj_t *sw, lv_event_t e);
	static void dailyML_cb(lv_obj_t *sw, lv_event_t e);
	static void save_cb(lv_obj_t *sw, lv_event_t e);
	static void lightToggle_cb(lv_obj_t *sw, lv_event_t e);
	static void pumpToggle_cb(lv_obj_t *sw, lv_event_t e);
	static void feeding_cb(lv_obj_t *sw, lv_event_t e);
	static void prime_cb(lv_obj_t *sw, lv_event_t e);
	static void feed_start_cb(lv_obj_t *sw, lv_event_t e);
	
	static bool isPriming;
	static void updateUITask(lv_task_t *task);
	
	static void settingsWidget();
	static void setLightOnLabel(int value);
	static void setLightDurLabel(int value);
	static void setPumpDurLabel(int value);
	static void setDailyML(int value);
	
	static void statusWidget();
	static void feedingWidget();
	
	static void logWidget();
	
	static void networkNotif();
	
	static std::vector<lv_obj_t*> logLabels;
	
	lv_obj_t *tabView;
	lv_task_t *updateTask, *networkTask;
	static lv_obj_t *timeLbl;
	static uint briteness;
	
	Settings* settings;
	static lv_obj_t *settingsWindow;
	static lv_obj_t *lightOnValueLbl, *lightOnSlider;
	static lv_obj_t *lightDurLbl, *lightDurSlider;
	static lv_obj_t *pumpDurLbl, *pumpDurSlider;
	static lv_obj_t *dailyMLLbl, *dailyMLSlider;
	
	static lv_obj_t *waterLED, *feedLED;
	static lv_style_t styleRed, styleGreen;
	static lv_obj_t *lightLED, *pumpLED, *feedingLED;
	static lv_obj_t *lightContBtn, *pumpContBtn, *feedingContBtn;
	
	static lv_obj_t *logWindow;
	static std::vector<lv_obj_t*> logMsgLbls;
	static lv_obj_t *logContainer;
	static uint pumpCounter;
	
	static lv_obj_t *feedingWindow;
	
	static lv_obj_t *networkStatus;
	
	static int networkCheckCtr;
};