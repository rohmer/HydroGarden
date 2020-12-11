#include "NetworkTools.h"

std::vector<sWirelessNetwork> NetworkTools::ScanNetworks()
{
	std::vector<sWirelessNetwork> networks;
	int status, i;
	struct bss_info bss[10]; 
	struct wifi_scan *wifi = wifi_scan_init("wlan0");

	status = wifi_scan_all(wifi, bss, 10);
	
	for (i = 0; i < status && i<10; ++i)
	{
		sWirelessNetwork ap;
		ap.ssid = bss[i].ssid;
		ap.signalStrength = bss[i].signal_mbm / 100;
		bool inlist = false;
		for (int j = 0; j < networks.size(); j++)
			if (networks[j].ssid == ap.ssid)
				inlist = true;
		if(!inlist)
			networks.push_back(ap);
	}
	
	wifi_scan_close(wifi);
	return networks;
}

std::string NetworkTools::GetConnectedNetwork()
{
	int status, i;
	struct bss_info bss[10]; 
	struct wifi_scan *wifi = wifi_scan_init("wlan0");

	status = wifi_scan_all(wifi, bss, 10);
	
	for (i = 0; i < status && i < 10; ++i)
	{
		if (bss[i].status == bss_status::BSS_ASSOCIATED)
		{
			std::stringstream ss;
			ss << bss[i].ssid;
			return ss.str();
		}
	}
	return "";
}

bool NetworkTools::ConnectNetwork(std::string ssid, std::string password)
{

	return false;
}
