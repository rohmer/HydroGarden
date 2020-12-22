#include "UI.h"
FILE *UserInterface::fp;

int UserInterface::currentBriteness = 0;
lv_style_t UserInterface::styleRed, UserInterface::styleGreen;
bool UserInterface::isPriming = false;

int UserInterface::networkCheckCtr = 100*60;
lv_obj_t *UserInterface::networkStatus = nullptr;
lv_obj_t *UserInterface::waterLED = nullptr;
lv_obj_t *UserInterface::feedLED = nullptr;

lv_obj_t *UserInterface::timeLbl;
std::vector<lv_obj_t*> UserInterface::logLabels;
uint UserInterface::briteness = 2;
lv_obj_t *UserInterface::settingsWindow = nullptr;
lv_obj_t *UserInterface::lightOnValueLbl = nullptr;
lv_obj_t *UserInterface::lightOnSlider = nullptr;
lv_obj_t *UserInterface::lightDurLbl = nullptr;
lv_obj_t *UserInterface::lightDurSlider = nullptr;
lv_obj_t *UserInterface::pumpDurLbl = nullptr;
lv_obj_t *UserInterface::pumpDurSlider = nullptr;
lv_obj_t *UserInterface::dailyMLLbl = nullptr;
lv_obj_t *UserInterface::dailyMLSlider = nullptr;
lv_obj_t *UserInterface::logWindow = nullptr;
std::vector<lv_obj_t *> UserInterface::logMsgLbls;
lv_obj_t *UserInterface::logContainer = nullptr;
lv_obj_t *UserInterface::lightLED = nullptr;
lv_obj_t *UserInterface::pumpLED = nullptr;
lv_obj_t *UserInterface::feedingLED = nullptr;

lv_obj_t *UserInterface::lightContBtn = nullptr;
lv_obj_t *UserInterface::pumpContBtn = nullptr;
lv_obj_t *UserInterface::feedingContBtn = nullptr;
lv_obj_t *UserInterface::feedingWindow = nullptr;
uint UserInterface::pumpCounter = 1;

UserInterface::UserInterface()
{
	LOGI("->UI()");
	settings = Settings::GetInstance();
	
	init();
	LOGI("<-UI()");
}

void UserInterface::init()
{
	static lv_disp_buf_t disp_buf;
	static lv_color_t buf1[DISP_BUF_SIZE];
	static lv_color_t buf2[DISP_BUF_SIZE];
	lv_disp_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);
	lv_init();
	fbdev_init();
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.buffer = &disp_buf;
	disp_drv.flush_cb = fbdev_flush;

	lv_disp_t *disp;
	disp = lv_disp_drv_register(&disp_drv);
	evdev_init();
	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = evdev_read;
	lv_indev_drv_register(&indev_drv);
	updateTask = lv_task_create(updateUITask, 100, LV_TASK_PRIO_LOW, nullptr);
}

void UserInterface::updateUITask(lv_task_t *task)
{
	// Update Time
	std::time_t t = std::time(nullptr);
	std::stringstream ss;
	ss << std::asctime(std::localtime(&t));
	lv_label_set_text(timeLbl, ss.str().c_str());
	int tm = lv_disp_get_inactive_time(nullptr);
	if (tm > 120000)
		briteness = 0;
	else if (tm > 60000)
		briteness = 10;
	else
		if (tm > 30000)
		briteness = 100;
	else
		briteness = 255;
	SetBriteness(briteness);
	
	bool lOn = Scheduler::GetInstance()->GetLight();
	if (lOn)
		lv_led_on(lightLED);
	else
		lv_led_off(lightLED);
	bool pOn = Scheduler::GetInstance()->GetPump();
	if (pOn)
		lv_led_on(pumpLED);
	else
		lv_led_off(pumpLED);
	bool fOn = Scheduler::GetInstance()->IsFeeding(t);
	if (fOn)
		lv_led_on(feedingLED);
	else
		lv_led_off(feedingLED);
	networkCheckCtr++;
	if (networkCheckCtr > 50)
	{
		networkNotif();
		networkCheckCtr = 0;
	
	
		if (Hardware::WaterLevel())
			lv_obj_set_style_local_bg_color(waterLED, LV_LED_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
		else
			lv_obj_set_style_local_bg_color(waterLED, LV_LED_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
	
		lv_led_on(waterLED);

		if (Hardware::FeedLevel())
			lv_obj_set_style_local_bg_color(feedLED, LV_LED_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
		else
			lv_obj_set_style_local_bg_color(feedLED, LV_LED_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
	
		lv_led_on(feedLED);
	}
}

void UserInterface::networkNotif()
{
	
	if (UserInterface::networkStatus == nullptr) 
	{
		UserInterface::networkStatus = lv_label_create(lv_scr_act(), nullptr);
		lv_obj_set_pos(networkStatus, 125, LV_DPX(15));
	}
	
	std::string connectedNetwork = NetworkTools::GetConnectedNetwork();
	if (connectedNetwork.empty())
	{
		std::stringstream ss;
		ss << "No Network Connection " << LV_SYMBOL_WARNING;
		lv_label_set_text(UserInterface::networkStatus, ss.str().c_str());
	}
	else
	{			
		std::stringstream ss;
		ss << connectedNetwork << " " << LV_SYMBOL_WIFI;
		lv_label_set_text(UserInterface::networkStatus, ss.str().c_str());
	}
	
}


void UserInterface::CreateMainWindow()
{
	tabView = lv_tabview_create(lv_scr_act(), nullptr);
	if (LV_THEME_DEFAULT_INIT == lv_theme_material_init) {
		lv_disp_size_t disp_size = lv_disp_get_size_category(NULL);
		if (disp_size >= LV_DISP_SIZE_MEDIUM) {
			lv_obj_set_style_local_pad_left(tabView, LV_TABVIEW_PART_TAB_BG, LV_STATE_DEFAULT, LV_HOR_RES / 2);
			lv_obj_t * sw = lv_switch_create(lv_scr_act(), NULL);
			if (lv_theme_get_flags() & LV_THEME_MATERIAL_FLAG_DARK)
				lv_switch_on(sw, LV_ANIM_OFF);
			lv_obj_set_event_cb(sw, color_chg_event_cb);
			lv_obj_set_pos(sw, LV_DPX(10), LV_DPX(15));
			lv_obj_set_style_local_value_str(sw, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, "Dark");
			lv_obj_set_style_local_value_align(sw, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_ALIGN_OUT_RIGHT_MID);
			lv_obj_set_style_local_value_ofs_x(sw, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_DPI / 35);
		}
	}
	timeLbl = lv_label_create(lv_scr_act(), nullptr);
	lv_obj_set_pos(timeLbl, 575, LV_DPX(15));

	lv_obj_t *setButton = lv_btn_create(lv_scr_act(), nullptr);
	lv_obj_set_size(setButton, 200, 50);
	lv_obj_set_pos(setButton, 25, 75);
	lv_obj_t *setLbl = lv_label_create(setButton, nullptr);
	std::stringstream ss;
	ss << LV_SYMBOL_SETTINGS << " Settings";
	lv_obj_set_style_local_text_font(setLbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_28);
	lv_label_set_text(setLbl, ss.str().c_str());
	lv_obj_set_event_cb(setButton, settings_click_event_cb);
	
	lv_obj_t *logButton = lv_btn_create(lv_scr_act(), nullptr);
	lv_obj_set_size(logButton, 200, 50);
	lv_obj_set_pos(logButton, 25, 135);
	lv_obj_t *logLbl = lv_label_create(logButton, nullptr);
	std::stringstream lss;
	lss << LV_SYMBOL_COPY << " Logs";
	lv_obj_set_style_local_text_font(logLbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_28);
	lv_label_set_text(logLbl, lss.str().c_str());
	lv_obj_set_event_cb(logButton, log_click_event_cb);
	
	lv_obj_t *netButton = lv_btn_create(lv_scr_act(), nullptr);
	lv_obj_set_size(netButton, 200, 50);
	lv_obj_set_pos(netButton, 25, 195);
	lv_obj_t *netLbl = lv_label_create(netButton, nullptr);
	std::stringstream nss;
	nss << LV_SYMBOL_WIFI << " Network";
	lv_obj_set_style_local_text_font(netLbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_28);
	lv_label_set_text(netLbl, nss.str().c_str());
	lv_obj_set_event_cb(netButton, network_click_cb);

	statusWidget();
	
	lv_style_init(&styleRed);
	lv_style_set_bg_color(&styleRed, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_style_init(&styleGreen);
	lv_style_set_bg_color(&styleGreen, LV_STATE_DEFAULT, LV_COLOR_GREEN);
}

void UserInterface::network_click_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e == LV_EVENT_CLICKED)
	{
		NetworkUI::CreateNetworkWindow();
	}
}

void UserInterface::feedingWidget()
{
	feedingWindow = lv_win_create(lv_scr_act(), nullptr);
	lv_win_set_title(feedingWindow, "Feeding Controls");
	lv_win_set_layout(feedingWindow, LV_LAYOUT_PRETTY_MID);
	lv_obj_t *closeBtn = lv_win_add_btn_right(feedingWindow, LV_SYMBOL_CLOSE);
	lv_obj_set_event_cb(closeBtn, lv_win_close_event_cb);
	
	lv_obj_t *primeBtn = lv_btn_create(feedingWindow, nullptr);
	lv_obj_set_size(primeBtn, 250, 250);
	lv_obj_t *primeLbl = lv_label_create(primeBtn, nullptr);
	lv_label_set_text(primeLbl, "Prime Pump");
	lv_obj_set_event_cb(primeBtn, prime_cb);
	
	lv_obj_t *feedBtn = lv_btn_create(feedingWindow, nullptr);
	lv_obj_set_size(feedBtn, 250, 250);
	lv_obj_t *feedLbl = lv_label_create(feedBtn, nullptr);
	lv_label_set_text(feedLbl, "Start New Garden");
	lv_obj_set_event_cb(feedBtn, feed_start_cb);
}	

void UserInterface::feed_start_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e == LV_EVENT_CLICKED && !isPriming)
	{
		isPriming = true;
		float time = Settings::GetInstance()->DailyMLFood() * 4 / MLPSEC * 1000000;
		Hardware::PumpsOn();
		Hardware::FeedOn();
		usleep(time);
		Hardware::FeedOff();
		Hardware::PumpsOff();
		isPriming = false;
	}
 }

void UserInterface::prime_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e == LV_EVENT_CLICKED && !isPriming)
	{	
		isPriming = true;
		Hardware::FeedOn();
		usleep(1000000);
		Hardware::FeedOff();
		isPriming = false;
	}
}

void UserInterface::statusWidget()

{
	lv_obj_t *statusCont = lv_cont_create(lv_scr_act(), nullptr);
	lv_obj_set_size(statusCont, 525, 350);
	lv_obj_set_pos(statusCont,250, 75);
	
	lv_obj_t *lightStatCont = lv_cont_create(statusCont, nullptr);
	lv_cont_set_layout(lightStatCont, LV_LAYOUT_PRETTY_TOP);

	lv_obj_set_size(lightStatCont, 150, 170);
	lv_obj_set_pos(lightStatCont, 15, 15);
	lv_obj_t *llbl = lv_label_create(lightStatCont, nullptr);
	lv_label_set_text(llbl, "Lights");
	if(lightLED==nullptr)
		lightLED = lv_led_create(lightStatCont, nullptr);
	lv_obj_set_size(lightLED, 50, 50);
	
	lightContBtn = lv_btn_create(lightStatCont, nullptr);
	lv_obj_t *lcblbl = lv_label_create(lightContBtn, nullptr);
	lv_label_set_text(lcblbl, "Toggle");
	lv_obj_set_event_cb(lightContBtn, lightToggle_cb);
	
	lv_obj_t *pumpStatCont= lv_cont_create(statusCont, nullptr);
	lv_cont_set_layout(pumpStatCont, LV_LAYOUT_PRETTY_TOP);

	lv_obj_set_size(pumpStatCont, 150, 170);
	lv_obj_set_pos(pumpStatCont, 185, 15);
	lv_obj_t *pumplbl=lv_label_create(pumpStatCont,nullptr);
	lv_label_set_text(pumplbl, "Pumps");
	
	if(pumpLED==nullptr)
		pumpLED = lv_led_create(pumpStatCont, nullptr);
	lv_obj_set_size(pumpLED, 50, 50);
	pumpContBtn = lv_btn_create(pumpStatCont, nullptr);
	lv_obj_t *plbl = lv_label_create(pumpContBtn, nullptr);
	lv_label_set_text(plbl, "Run Pump");
	lv_obj_set_event_cb(pumpContBtn, pumpToggle_cb);
	
	lv_obj_t *feedStatCont = lv_cont_create(statusCont, nullptr);
	lv_cont_set_layout(feedStatCont, LV_LAYOUT_PRETTY_TOP);

	lv_obj_set_size(feedStatCont, 150, 170);
	lv_obj_set_pos(feedStatCont, 355, 15);
	lv_obj_t *feedLbl = lv_label_create(feedStatCont, nullptr);
	lv_label_set_text(feedLbl, "Feeding");
	
	if(feedingLED==nullptr)
		feedingLED = lv_led_create(feedStatCont, nullptr);
	lv_obj_set_size(feedingLED, 50, 50);
	feedingContBtn = lv_btn_create(feedStatCont, nullptr);
	lv_obj_t *flbl = lv_label_create(feedingContBtn, nullptr);
	lv_label_set_text(flbl, "Feeding");
	lv_obj_set_event_cb(feedingContBtn, feeding_cb);
	
	lv_obj_t *waterLevelCont = lv_cont_create(statusCont, nullptr);
	lv_cont_set_layout(waterLevelCont, LV_LAYOUT_PRETTY_TOP);

	lv_obj_set_size(waterLevelCont, 150, 140);
	lv_obj_set_pos(waterLevelCont, 15, 200);
	lv_obj_t *wlbl = lv_label_create(waterLevelCont, nullptr);
	lv_label_set_text(wlbl, "Water Level");
	if(waterLED==nullptr)		
		waterLED = lv_led_create(waterLevelCont, nullptr);
	lv_obj_set_size(waterLED, 50, 50);
	
	lv_obj_t *feedLevelCont = lv_cont_create(statusCont, nullptr);
	lv_cont_set_layout(feedLevelCont, LV_LAYOUT_PRETTY_TOP);

	lv_obj_set_size(feedLevelCont, 150, 140);
	lv_obj_set_pos(feedLevelCont, 185, 200);
	lv_obj_t *folbl = lv_label_create(feedLevelCont, nullptr);
	lv_label_set_text(folbl, "Food Level");
	if (feedLED == nullptr)		
		feedLED = lv_led_create(feedLevelCont, nullptr);
	lv_obj_set_size(feedLED, 50, 50);
	lv_led_set_bright(feedLED, LV_LED_BRIGHT_MAX);
	lv_led_set_bright(waterLED, LV_LED_BRIGHT_MAX);
}

void UserInterface::pumpToggle_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e == LV_EVENT_CLICKED)
	{
		Scheduler::GetInstance()->DoPumpToggleRun();
	}
}	

void UserInterface::feeding_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e == LV_EVENT_CLICKED)
	{
		feedingWidget();
	}
}	

void UserInterface::lightToggle_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e == LV_EVENT_CLICKED)
	{
		
		if (Scheduler::GetInstance()->GetLight())
			Scheduler::GetInstance()->SetLight(false);
		else
			Scheduler::GetInstance()->SetLight(true);
	}
}

void UserInterface::log_click_event_cb(lv_obj_t *sw, lv_event_t e)
{
	logWindow = lv_win_create(lv_scr_act(), nullptr);
	lv_obj_set_size(logWindow, 790, 400);
	lv_obj_set_pos(logWindow, 5, 50);
	lv_win_set_title(logWindow, "Logs");
	lv_win_set_layout(logWindow, LV_LAYOUT_COLUMN_LEFT);
	lv_obj_t *logWinButton = lv_win_add_btn_right(logWindow, LV_SYMBOL_CLOSE);
	lv_obj_set_event_cb(logWinButton, lv_win_close_event_cb);

	/*logContainer = lv_cont_create(logWindow, NULL);
	lv_cont_set_layout(logContainer, LV_LAYOUT_COLUMN_LEFT);
	lv_cont_set_fit2(logContainer, LV_FIT_NONE, LV_FIT_TIGHT);
	lv_obj_set_size(logContainer, 750, 390);*/
	//lv_cont_set_layout(logWindow, LV_LAYOUT_COLUMN_LEFT);
	std::deque<sLogMsg> logMessages = Logger::GetInstance()->GetLogMessages();

	
	for (int i = logMessages.size()-1; i >=0 ; i--)
	{
		tm *lTime = localtime(&logMessages[i].logTime);
		std::stringstream ss;
		ss << lTime->tm_hour << ":" << lTime->tm_min << ":" << lTime->tm_sec;
		std::string color;
		switch (logMessages[i].level)
		{
		case WARN:
			color = "#FFFF00";
			break;
		case ERROR:
			color = "#FF0000";
			break;
		default:
			color = "#000000";
			break;
		}
		
		ss << color << " " << logMessages[i].msg;
		
		std::string s = ss.str();
		lv_obj_t *label = lv_label_create(logWindow, nullptr);
		lv_label_set_text(label, s.c_str());
		logMsgLbls.push_back(label);
	}
}

void UserInterface::close_log_cb(lv_obj_t * sw, lv_event_t e)
{
	LOGI("CLOSELOG");
	if (e == LV_EVENT_CLICKED)
	{
		for (std::vector<lv_obj_t*>::iterator it = logMsgLbls.begin(); it != logMsgLbls.end(); it++)
			lv_obj_del(*it);
		logMsgLbls.clear();
		lv_obj_del(logWindow);		
	}
}


void UserInterface::color_chg_event_cb(lv_obj_t * sw, lv_event_t e)
{
	if (LV_THEME_DEFAULT_INIT != lv_theme_material_init) return;
	if (e == LV_EVENT_VALUE_CHANGED) {
		uint32_t flag = LV_THEME_MATERIAL_FLAG_LIGHT;
		if (lv_switch_get_state(sw)) flag = LV_THEME_MATERIAL_FLAG_DARK;

		LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(),
			lv_theme_get_color_secondary(),
			flag,
			lv_theme_get_font_small(),
			lv_theme_get_font_normal(),
			lv_theme_get_font_subtitle(),
			lv_theme_get_font_title());
	}
}

void UserInterface::settings_click_event_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e == LV_EVENT_CLICKED)
	{
		LOGI("SETTINGSUICLICK");
		settingsWidget();
	}
}


void UserInterface::settingsWidget()
{

	settingsWindow = lv_win_create(lv_scr_act(), nullptr);
	lv_obj_set_size(settingsWindow, 790, 400);
	lv_obj_set_pos(settingsWindow, 5, 50);
	lv_win_set_title(settingsWindow, "Settings");
	lv_obj_t *closeWinButton=lv_win_add_btn_right(settingsWindow, LV_SYMBOL_CLOSE);
	lv_obj_set_event_cb(closeWinButton, close_settings_cb);
	lv_obj_t *lightOnSliderCont = lv_cont_create(settingsWindow, nullptr);
	lv_obj_set_size(lightOnSliderCont, 750, 90);
	lv_obj_t *lightOnLbl = lv_label_create(lightOnSliderCont, nullptr);
	lv_label_set_text(lightOnLbl, "Time to turn on grow lights");
	lv_obj_set_pos(lightOnLbl, 5, 5);
	
	lightOnSlider = lv_slider_create(lightOnSliderCont, nullptr);
	lv_obj_set_size(lightOnSlider, 700, 20);
	lv_obj_set_pos(lightOnSlider, 25, 35);
	lv_slider_set_range(lightOnSlider, 0, 47);
	lv_obj_set_event_cb(lightOnSlider, lightOn_cb);
	int val = Settings::GetInstance()->LightStartHour()*2;
	if (Settings::GetInstance()->LightStartMin() == 30)
		val++;
	lightOnValueLbl = lv_label_create(lightOnSliderCont, NULL);
	lv_obj_set_auto_realign(lightOnValueLbl, true);
	lv_obj_align(lightOnValueLbl, lightOnSlider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	lv_slider_set_value(lightOnSlider, val,LV_ANIM_OFF);
	setLightOnLabel(val);
	
	lv_obj_t *lightDurationCont = lv_cont_create(settingsWindow, nullptr);
	lv_obj_set_size(lightDurationCont, 750, 90);
	lv_obj_set_pos(lightDurationCont, 20, 115);
	lv_obj_t *lightDurationLbl = lv_label_create(lightDurationCont, nullptr);
	lv_label_set_text(lightDurationLbl, "Grow light on duration");
	lv_obj_set_pos(lightDurationLbl, 5, 5);
	
	lightDurSlider = lv_slider_create(lightDurationCont, nullptr);
	lv_obj_set_size(lightDurSlider, 700, 20);
	lv_obj_set_pos(lightDurSlider, 25, 35);
	lv_slider_set_range(lightDurSlider, 1, 47);
	lv_obj_set_event_cb(lightDurSlider, lightDur_cb);
	int v = Settings::GetInstance()->LightDuration() / 30;
	lightDurLbl = lv_label_create(lightDurationCont, NULL);
	lv_obj_set_auto_realign(lightDurLbl, true);
	lv_obj_align(lightDurLbl, lightDurSlider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	lv_slider_set_value(lightDurSlider, v, LV_ANIM_OFF);
	setLightDurLabel(v);
	
	lv_obj_t *pumpDurCont=lv_cont_create(settingsWindow, nullptr);
	lv_obj_set_size(pumpDurCont, 370, 90);
	lv_obj_set_pos(pumpDurCont, 20, 210);
	lv_obj_t *pumpDurLabel = lv_label_create(pumpDurCont, nullptr);
	lv_label_set_text(pumpDurLabel, "Pump run time per hour");
	lv_obj_set_pos(pumpDurLabel, 5, 5);
	pumpDurSlider = lv_slider_create(pumpDurCont, nullptr);
	lv_obj_set_size(pumpDurSlider, 300, 20);
	lv_obj_set_pos(pumpDurSlider, 25, 35);
	lv_slider_set_range(pumpDurSlider, 1, 30);
	lv_obj_set_event_cb(pumpDurSlider, pumpDur_cb);
	int d = Settings::GetInstance()->PumpRunTime()/30;
	if (Settings::GetInstance()->PumpRunTime() % 30 != 0)
		d++;
	pumpDurLbl = lv_label_create(pumpDurCont, NULL);
	lv_obj_set_auto_realign(pumpDurLbl, true);
	lv_obj_align(pumpDurLbl, pumpDurSlider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	setPumpDurLabel(d);
	lv_slider_set_value(pumpDurSlider, d,LV_ANIM_OFF);
	
	lv_obj_t *dailyMLCont = lv_cont_create(settingsWindow, nullptr);
	lv_obj_set_size(dailyMLCont, 370, 90);
	lv_obj_set_pos(dailyMLCont, 400, 210);
	dailyMLSlider = lv_slider_create(dailyMLCont, nullptr);
	lv_obj_set_size(dailyMLSlider, 300, 20);
	lv_obj_set_pos(dailyMLSlider, 25, 35);
	lv_obj_t *dailyLbl= lv_label_create(dailyMLCont, nullptr);
	lv_label_set_text(dailyLbl, "ML food per day");
	lv_obj_set_pos(dailyLbl, 5, 5);
	lv_slider_set_range(dailyMLSlider, 1, 30);
	lv_obj_set_event_cb(dailyMLSlider, dailyML_cb);
	float df = Settings::GetInstance()->DailyMLFood()*2;

	dailyMLLbl = lv_label_create(dailyMLCont, NULL);
	lv_obj_set_auto_realign(dailyMLLbl, true);
	lv_obj_align(dailyMLLbl, dailyMLSlider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	setDailyML(df);
	lv_slider_set_value(dailyMLSlider, df, LV_ANIM_OFF);
	
	lv_obj_t *saveBtn = lv_btn_create(settingsWindow, nullptr);
	lv_obj_set_size(saveBtn, 150, 30);
	lv_obj_set_pos(saveBtn, 600, 310);
	lv_obj_t *saveLbl = lv_label_create(saveBtn, nullptr);
	lv_label_set_text(saveLbl, "Save Settings");
	lv_obj_set_event_cb(saveBtn, save_cb);
	lv_obj_t *cancelBtn = lv_btn_create(settingsWindow, nullptr);
	lv_obj_set_size(cancelBtn, 150, 30);
	lv_obj_set_pos(cancelBtn, 50, 310);
	lv_obj_t *cancelLbl = lv_label_create(cancelBtn, nullptr);
	lv_label_set_text(cancelLbl, "Close Window");
	lv_obj_set_event_cb(cancelBtn, close_settings_cb);
}

void UserInterface::save_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e == LV_EVENT_CLICKED)
	{
		int val = lv_slider_get_value(lightOnSlider);
		int hour = val / 2;
	
		int min = 0;
		if (val % 2 == 1)
			min = 30;
		val = lv_slider_get_value(lightDurSlider);
		int duration = val * 30;
		if (val % 2 == 1)
			duration += 30;
		Settings::GetInstance()->SetLightTime(hour, min, duration);
		val = lv_slider_get_value(pumpDurSlider);
		float durMin = val / 2;
		if (val % 2 == 1)
			durMin += 0.5;
		Settings::GetInstance()->SetPumpRunTime(durMin * 60);
	
		val = lv_slider_get_value(dailyMLSlider);
		float mlFood = val / 2;
		if (val % 2 == 1)
			mlFood += 0.5;
		Settings::GetInstance()->SetDailyMLFood(mlFood);
		Settings::GetInstance()->SaveSettings();
		lv_obj_del_async(settingsWindow);
	}
}

void UserInterface::dailyML_cb(lv_obj_t *sw, lv_event_t e)

{
	if (e == LV_EVENT_VALUE_CHANGED)
		setDailyML(lv_slider_get_value(dailyMLSlider));	
}

void UserInterface::setDailyML(int value)
{
	int v = value / 2;
	std::string dec = ".0";
	if (value % 2 == 1 || value==1)
		dec = ".5";
	std::stringstream ss;
	ss << v << dec << " ML/Day";
	lv_label_set_text(dailyMLLbl, ss.str().c_str());
	
}

void UserInterface::pumpDur_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e == LV_EVENT_VALUE_CHANGED)
		setPumpDurLabel(lv_slider_get_value(pumpDurSlider));	
}

void UserInterface::setPumpDurLabel(int value)
{
	std::stringstream ss;
	int v = value/2;
	std::string dec = ".0";
	if (value % 2 != 0)
		dec = ".5";
	ss << v << dec << " minutes/hour";
	lv_label_set_text(pumpDurLbl, ss.str().c_str());
}

void UserInterface::lightOn_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e == LV_EVENT_VALUE_CHANGED)
		setLightOnLabel(lv_slider_get_value(lightOnSlider));	
}

void UserInterface::lightDur_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e == LV_EVENT_VALUE_CHANGED)
		setLightDurLabel(lv_slider_get_value(lightDurSlider));
}

void UserInterface::setLightDurLabel(int val)
{
	int hours = val / 2;

	std::stringstream hss;
	if (hours < 10)
		hss << "0";
	hss << hours;
	
	std::string minutes = ".0";
	if (val % 2 != 0)
		minutes = ".5";
	
	std::stringstream ss;
	ss << hss.str() << minutes << " hours";
	lv_label_set_text(lightDurLbl, ss.str().c_str());
	
}

void UserInterface::setLightOnLabel(int val)

{
	std::string ampm = "AM";
	if (val > 23)
		ampm = "PM";
	std::string min = "00";
	if (val % 2 == 1)
		min = "30";
	if (val > 24)
		val -= 24;
	int hour = val / 2;

	if (hour == 0)
		hour = 12;
	std::stringstream ss;
	ss << hour << ":" << min << " " << ampm;
	lv_label_set_text(lightOnValueLbl, ss.str().c_str());
}

void UserInterface::close_settings_cb(lv_obj_t *sw, lv_event_t e)
{
	LOGI("CLOSESETTINGS");
	if (e == LV_EVENT_CLICKED)
	{
		lv_obj_del_async(settingsWindow);		
	}
}

void UserInterface::SetBriteness(uint briteness)
{
	if (currentBriteness == briteness)
		return;
	currentBriteness = briteness;
	try
	{
		std::ofstream f;
		f.open("/sys/class/backlight/rpi_backlight/brightness");
		f << briteness << std::endl;
		f.close();	
	}
	catch (const std::exception&)
	{
		
	}
	
}
