#include "NetworkUI.h"

lv_obj_t *NetworkUI::networkWindow = nullptr;
lv_obj_t *NetworkUI::keyboard = nullptr;
lv_obj_t *NetworkUI::passwordTA = nullptr;

void NetworkUI::CreateNetworkWindow()
{
	networkWindow = lv_win_create(lv_scr_act(), nullptr);
	lv_win_set_title(networkWindow, "Wireless Settings");

	lv_obj_t *closeBtn = lv_win_add_btn_right(networkWindow, LV_SYMBOL_CLOSE);
	lv_obj_set_event_cb(closeBtn, lv_win_close_event_cb);
	
	std::vector<sWirelessNetwork> networks = NetworkTools::ScanNetworks();
	
	lv_obj_t *wirelessListCont = lv_cont_create(networkWindow, nullptr);
	lv_obj_set_size(wirelessListCont, 500, 400);
	lv_obj_set_pos(wirelessListCont, 275, 20);
	
	lv_obj_t *l1 = lv_label_create(wirelessListCont, nullptr);
	lv_obj_set_pos(l1, 10, 10);
	lv_label_set_text(l1, "Wireless Networks");
	lv_obj_set_style_local_text_font(l1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_28);
	
	lv_obj_t *wPage = lv_page_create(wirelessListCont, nullptr);
	lv_obj_set_size(wPage, 490, 340);
	lv_obj_set_pos(wPage, 5, 45);
	
	lv_obj_t *wLbl = lv_label_create(wPage, nullptr);
	lv_label_set_text(wLbl, "Wireless SSID");
	lv_obj_set_pos(wLbl, 10, 5);

	lv_obj_t *sLbl = lv_label_create(wPage, nullptr);
	lv_label_set_text(sLbl, "Signal Strength");
	lv_obj_set_pos(sLbl, 320, 5);

	for (int i = 0; i < networks.size(); i++)
	{
		lv_obj_t *btn = lv_btn_create(wPage, nullptr);
		lv_obj_set_size(btn, 310, 30);
		
		lv_obj_set_user_data(btn, (void *)networks[i].ssid.c_str());
		lv_obj_t *btnLbl = lv_label_create(btn, nullptr);
		lv_label_set_text(btnLbl, networks[i].ssid.c_str());
		lv_obj_set_pos(btn, 0, 30 + (i * 40));
		
		lv_obj_set_event_cb(btn, lv_network_cb);
		
		lv_obj_t *str = lv_label_create(wPage, nullptr);
		int isstr = 100 + networks[i].signalStrength;
		std::stringstream sstr;
		sstr << isstr;
		lv_label_set_text(str, sstr.str().c_str());
		lv_obj_set_pos(str, 375, 30 + (i * 40));
		
	}
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
	lv_obj_t *pwWindow = lv_win_create(lv_scr_act(), nullptr);
	
	std::stringstream ss;
	ss << "Set password for " << ssid;
	lv_win_set_title(pwWindow, ss.str().c_str());
	
	lv_obj_t *pwl = lv_label_create(pwWindow, nullptr);
	lv_label_set_text(pwl, "Network Password");
	lv_obj_set_pos(pwl, 10, 10);
	passwordTA = lv_textarea_create(pwWindow, nullptr);
	lv_textarea_set_one_line(passwordTA, true);
	lv_obj_set_pos(passwordTA, 200, 13);
	lv_textarea_set_text(passwordTA, "");
	
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
	
		if (NetworkTools::ConnectNetwork(ssid, spwd.str()))
		{
			lv_obj_del_async(networkWindow);
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
		lv_obj_del_async(networkWindow);
	}
}