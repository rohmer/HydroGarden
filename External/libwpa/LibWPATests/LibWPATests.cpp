#include <iostream>
#include <string>
#include "libwpa_cli_wrapper.h"
#include "wpa_cli_oper.h"
#include <string.h>
#include <sstream>
#include "smartlist.h"

using namespace std;

int main(int argc, char *argv[])
{
	std::stringstream ss;
	ss << "remove_network 1";
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	if (wpa_cli_execute(ss.str().c_str(), buf, buf_len) < 0)
	{
		cout << "Error executing remove network\n";
	}
	
	if (strcmp(buf, "OK\n") != 0)
	{
		cout << "Buffer: " << buf << "\n";
	}
	std::string interface = "wlan0";
	
	// Test Init
	int res = init_wpa_cli();
	if (res < 0)
	{
		cout << "Failed to init wpa_cli, exiting\n\n";
		exit(-1);
	}
	
	cout << "Init WPA Test - Success\n";
	
	WifiStatus status = get_wifi_status();
	
	smartlist_t *wifiList = get_network_list();
	if (wifiList == NULL)
		cout << "Failed to get connected wifi list";
	
	for (int i = 0; i < wifiList->num_used; i++)
	{
		WifiListItem *li = (WifiListItem *)smartlist_pop_last(wifiList);
		cout << li->networkID << "\t" << li->ssid << "\t" << li->isCurrent << "\n";
	}
	
	WifiConfig *config = new WifiConfig();
	
	config->flag = 1;
	strcpy(config->password, "cocoabeans");
	strcpy(config->ssid, "PoohAlt");
	if (connect_wifi(config) < 0)
	{
		
	}
	
}