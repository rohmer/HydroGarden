#include "NetworkUI.h"

lv_obj_t *NetworkUI::hnSetBtn = nullptr;
lv_obj_t *NetworkUI::hnCanBtn = nullptr;
lv_obj_t *NetworkUI::hnLbl = nullptr;

lv_obj_t *NetworkUI::hostnameWin = nullptr;
lv_obj_t *NetworkUI::hostnameTA = nullptr;

lv_obj_t *NetworkUI::pwWindow = nullptr;
lv_obj_t *NetworkUI::networkWindow = nullptr;
lv_obj_t *NetworkUI::keyboard = nullptr;
lv_obj_t *NetworkUI::passwordTA = nullptr;
lv_task_t *NetworkUI::updateNetworkTask = nullptr;
std::vector<std::string> NetworkUI::networkButtons;
bool NetworkUI::failedScan = false;
lv_obj_t *NetworkUI::wPage = nullptr;
bool NetworkUI::updateActive = true;

void NetworkUI::updateNetworkList(lv_task_t *task)
{
	if (!updateActive)
		return;
	
	std::vector<sWirelessNetwork> netList = NetworkTools::GetScanResults();
	for (int i = 0; i < netList.size(); i++)
	{
		bool drawn = false;
		for (int j = 0; j < networkButtons.size(); j++)
		{
			if (networkButtons[j] == netList[i].ssid)
				drawn = true;
		}
			
		if (!drawn)
		{
			networkButtons.push_back(netList[i].ssid);
	
			lv_obj_t *btn = lv_btn_create(wPage, nullptr);
			lv_obj_set_size(btn, 310, 30);
			std::string ssidStr = networkButtons[networkButtons.size() - 1];
			lv_obj_set_user_data(btn, (void *)networkButtons[networkButtons.size()-1].c_str());
			lv_obj_t *btnLbl = lv_label_create(btn, nullptr);			
			lv_label_set_text(btnLbl, ssidStr.c_str());
			lv_obj_set_pos(btn, 0, (networkButtons.size() * 40));
		
			lv_obj_set_event_cb(btn, lv_network_cb);
		
			lv_obj_t *str = lv_label_create(wPage, nullptr);
			int isstr = 100 + netList[i].signalStrength;
			std::stringstream sstr;
			sstr << isstr;
			lv_label_set_text(str, sstr.str().c_str());
			lv_obj_set_pos(str, 375, (networkButtons.size() * 40));
			
			lv_obj_t *wFlags = lv_label_create(wPage, nullptr);
			lv_label_set_text(wFlags, netList[i].flags.c_str());
			lv_obj_set_pos(wFlags, 460, (networkButtons.size() * 40));
		}
	}
}

void NetworkUI::CreateNetworkWindow()
{
	failedScan = false;
	updateActive = true;
	networkButtons.clear();
	networkWindow = lv_win_create(lv_scr_act(), nullptr);
	
	char cHost[1024];
	gethostname(cHost, 1024);
	
	hnLbl = lv_label_create(networkWindow, nullptr);
	std::stringstream ss;
	ss << "Device Name: " << cHost;
	lv_label_set_text(hnLbl, ss.str().c_str());
	lv_obj_set_pos(hnLbl, 30, 30);
	
	lv_obj_t *hnBtn = lv_btn_create(networkWindow, nullptr);
	lv_obj_t *hnBLbl = lv_label_create(hnBtn, nullptr);
	std::stringstream ss2;
	ss2 << LV_SYMBOL_KEYBOARD << " Change Device Name";
	lv_label_set_text(hnBLbl, ss2.str().c_str());
	lv_obj_set_size(hnBtn, 250, 35);
	lv_obj_set_pos(hnBtn, 148+strlen(cHost)*9+10, 23);
	lv_obj_set_event_cb(hnBtn, hostname_cb);
	
	lv_win_set_title(networkWindow, "Wireless Settings");

	lv_obj_t *closeBtn = lv_win_add_btn_right(networkWindow, LV_SYMBOL_CLOSE);
	lv_obj_set_event_cb(closeBtn, lv_win_close_event_cb);
	
	if (NetworkTools::InitiateNetworkScan())
		lv_task_create(updateNetworkList, 500, LV_TASK_PRIO_LOW, nullptr);
	
	lv_obj_t *wirelessListCont = lv_cont_create(networkWindow, nullptr);
	lv_obj_set_size(wirelessListCont, 750, 335);
	lv_obj_set_pos(wirelessListCont, 35, 75);
	
	lv_obj_t *l1 = lv_label_create(wirelessListCont, nullptr);
	lv_obj_set_pos(l1, 10, 10);
	lv_label_set_text(l1, "Wireless Networks");
	lv_obj_set_style_local_text_font(l1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_28);
	
	wPage = lv_page_create(wirelessListCont, nullptr);
	lv_obj_set_size(wPage, 740, 285);
	lv_obj_set_pos(wPage, 5, 45);
	
	lv_obj_t *wLbl = lv_label_create(wPage, nullptr);
	lv_label_set_text(wLbl, "Wireless SSID");
	lv_obj_set_pos(wLbl, 10, 5);

	lv_obj_t *sLbl = lv_label_create(wPage, nullptr);
	lv_label_set_text(sLbl, "Signal Strength");
	lv_obj_set_pos(sLbl, 320, 5);
	
	lv_obj_t *tLbl = lv_label_create(wPage, nullptr);
	lv_label_set_text(tLbl, "Wireless Flags");
	lv_obj_set_pos(tLbl, 525, 5);
}

void NetworkUI::hostname_cb(lv_obj_t* sw, lv_event_t e)
{
	if (e != LV_EVENT_CLICKED)
	{
		return;
	}
	char cHost[1024];
	gethostname(cHost, 1024);
	hostnameWin = lv_win_create(lv_scr_act(), nullptr);
	
	std::stringstream ss;
	lv_win_set_title(hostnameWin, "Set Device Name");
	
	lv_obj_t *lbl1 = lv_label_create(hostnameWin, nullptr);
	lv_label_set_text(lbl1, "Device Name:");
	lv_obj_set_pos(lbl1, 30, 30);
	
	hostnameTA = lv_textarea_create(hostnameWin, nullptr);
	lv_obj_set_pos(hostnameTA, 160, 20);
	lv_textarea_set_one_line(hostnameTA, true);
	keyboard = lv_keyboard_create(hostnameWin, nullptr);
	lv_keyboard_set_textarea(keyboard, hostnameTA);
	lv_obj_set_width(hostnameTA, 300);
	lv_textarea_set_text(hostnameTA, cHost);
	
	lv_obj_set_event_cb(hostnameTA, hostnameEdit_cb);
	
	hnSetBtn = lv_btn_create(hostnameWin, nullptr);
	lv_obj_set_size(hnSetBtn, 300, 40);
	lv_obj_t *hsbLbl = lv_label_create(hnSetBtn, nullptr);
	lv_obj_set_pos(hnSetBtn, 400, 100);
	lv_label_set_text(hsbLbl, "Set Device Name");
	lv_btn_set_state(hnSetBtn, LV_STATE_DISABLED);
	lv_obj_set_event_cb(hnSetBtn, setHostname_cb);
	
	hnCanBtn = lv_btn_create(hostnameWin, nullptr);
	lv_obj_set_size(hnCanBtn, 300, 40);
	lv_obj_set_pos(hnCanBtn, 20, 100);
	lv_obj_t *hncLbl = lv_label_create(hnCanBtn, nullptr);
	lv_label_set_text(hncLbl, "Cancel");
	lv_obj_set_state(hnCanBtn, LV_STATE_DEFAULT);
	lv_obj_set_event_cb(hnCanBtn, cancelHostname_cb);
	
}

void NetworkUI::setHostname_cb(lv_obj_t *sw, lv_event_t e)
{	
	if (e != LV_EVENT_CLICKED)
		return;
	if (lv_btn_get_state(hnSetBtn) == LV_STATE_DISABLED)
	{
		lv_obj_del(hostnameWin);
	}
	
	const char *hn = lv_textarea_get_text(hostnameTA);
	if (strcmp(hn, Settings::GetInstance()->GetHostname().c_str()) != 0)
	{
		sethostname(hn, strlen(hn));
		NetworkTools::Reassociate();
	}
	
	lv_obj_del(hostnameWin);
}	

void NetworkUI::cancelHostname_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e != LV_EVENT_CLICKED)
		return;
	lv_obj_del(hostnameWin);
}

void NetworkUI::hostnameEdit_cb(lv_obj_t *sw, lv_event_t e)
{
	if (e != LV_EVENT_VALUE_CHANGED)
		return;

	char cHost[1024];
	gethostname(cHost, 1024);
	if (strcmp(cHost, lv_textarea_get_text(hostnameTA)) == 0)
		lv_obj_set_state(hnSetBtn, LV_STATE_DISABLED);
	else
		lv_obj_set_state(hnSetBtn, LV_STATE_DEFAULT);
}

void NetworkUI::lv_network_cb(lv_obj_t* sw, lv_event_t e)
{
	if (e != LV_EVENT_CLICKED)
	{
		return;
	}
	std::stringstream ssids;
	ssids << (char *)lv_obj_get_user_data(sw);
	
	static std::string ssid = ssids.str();
	
	// Now pop up a window to allow them to type in the password
	pwWindow = lv_win_create(lv_scr_act(), nullptr);
	
	std::stringstream ss;
	ss << "Set password for " << ssid;
	lv_win_set_title(pwWindow, ss.str().c_str());
	
	lv_obj_t *pwl = lv_label_create(pwWindow, nullptr);
	lv_label_set_text(pwl, "Network Password");
	lv_obj_set_pos(pwl, 10, 10);
	passwordTA = lv_textarea_create(pwWindow, nullptr);
	lv_textarea_set_one_line(passwordTA, true);
	lv_obj_set_pos(passwordTA, 200, 13);
	
	std::vector<sNetwork> networks = Settings::GetInstance()->GetNetworks();
	std::string pw="";
	for (int i = 0; i < networks.size(); i++)
	{
		if (strcmp(networks[i].ssid.c_str(), ssid.c_str()) == 0)
		{
			pw = networks[i].password;
		}
	}
	lv_textarea_set_text(passwordTA, pw.c_str());
	
	keyboard = lv_keyboard_create(pwWindow, nullptr);
	lv_keyboard_set_textarea(keyboard, passwordTA);
	
	lv_obj_t *connectButton = lv_btn_create(pwWindow, nullptr);
	lv_obj_set_size(connectButton, 300, 60);
	lv_obj_set_pos(connectButton, 50, 90);
	lv_obj_t *cLbl = lv_label_create(connectButton, nullptr);
	lv_label_set_text(cLbl, "Connect");
	lv_obj_set_style_local_text_font(cLbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_28);
	lv_obj_set_user_data(connectButton, (void *)ssid.c_str());
	lv_obj_set_event_cb(connectButton, connect_cb);
	lv_obj_t *cancelButton = lv_btn_create(pwWindow, nullptr);
	lv_obj_set_size(cancelButton, 300, 60);
	lv_obj_set_pos(cancelButton, 400, 90);
	lv_obj_t *canLbl = lv_label_create(cancelButton, nullptr);
	lv_label_set_text(canLbl, "Cancel");
	lv_obj_set_style_local_text_font(canLbl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_28);
	lv_obj_set_event_cb(cancelButton, cancel_cb);
	
}

void NetworkUI::connect_cb(lv_obj_t* sw, lv_event_t e)
{
	if (e == LV_EVENT_CLICKED)
	{
		char *ssids = (char *)lv_obj_get_user_data(sw);
	
		std::string ssid = ssids;

	
		std::stringstream spwd;
		spwd << lv_textarea_get_text(passwordTA);
	
		if (NetworkTools::ConnectWifi(ssid, spwd.str()))
		{
			int ctr = 0;
			while (ctr < 20)
			{
				sWifiStatus status = NetworkTools::GetWifiStatus();
				if (status.wpa_state == COMPLETED && status.ssid==ssid)
				{
					lv_obj_del_async(pwWindow);
					return;
				}
				usleep(10000);
				ctr++;
			}
		}
	}
}

void NetworkUI::cancel_cb(lv_obj_t* sw, lv_event_t e) 
{
	
}

void NetworkUI::lv_win_close_event_cb(lv_obj_t* sw, lv_event_t e)
{
	if (e == LV_EVENT_CLICKED)
	{
		updateActive = false;
		if (updateNetworkTask != nullptr)
			lv_task_del(updateNetworkTask);
		networkButtons.clear();
		lv_obj_del(networkWindow);
	}
}