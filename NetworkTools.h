#pragma once

#include <string.h> 
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include "Logger.h"
#include "wpa_cli_oper.h"
#include "libwpa_cli_wrapper.h"
#include "smartlist.h"

struct sWPANetwork
{
	uint networkID;
	std::string ssid;
	std::string bssid;
	std::string flags;
};
	
struct sWirelessNetwork
{
	std::string ssid, bssid,flags;
	int signalStrength, frequency;
	bool isCurrent;
};
struct sWifiStatus
{
	std::string bssid;
	std::string ssid;
	bool statusRetrieved;
	std::string	address;  	// = 8a:02 : 32 : db : b3 : b8
	std::string	uuid;
	eWPAState	wpa_state;
	int		freq;
	int		network_id;
	std::string	mode;
	std::string	pairwise_cipher;
	std::string	group_cipher;
	std::string	key_mgmt;
	std::string	ipaddress;		
};

class NetworkTools
{
public:
	static bool InitiateNetworkScan();
	static std::vector<sWirelessNetwork> GetScanResults();
	static bool ConnectWifi(std::string ssid, std::string password);
	
	static std::vector<sWPANetwork> ListNetworks();
	
	static bool SetNetworkSSID(uint networkID, std::string ssid);
	
	static bool RemoveNetwork(std::string ssid);
	static bool RemoveNetwork(uint networkID);
	
	static int AddNetwork(std::string ssid);
	static int AddNetwork();

	static bool SetPasswordWEP(std::string ssid, std::string password);
	static bool SetPasswordWEP(uint networkID, std::string password);
	
	static bool EnableNetwork(uint networkID);
	static bool EnableNetwork(std::string ssid);
	
	static bool DisableNetwork(uint networkID);
	static bool DisableNetwork(std::string ssid);
	
	static int GetNetworkID(std::string ssid);
	
	static bool SaveConfig();
	
	static bool Reassociate();
	
	static std::string GetConnectedNetwork();
	static sWifiStatus GetWifiStatus();
	
private:
	
};