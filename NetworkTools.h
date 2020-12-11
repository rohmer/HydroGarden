#pragma once

#include <string.h> 
#include <sstream>
#include <string>
#include <vector>

#include "wifi_scan.h"
#include "Logger.h"

struct sWirelessNetwork
{
	std::string ssid;
	int signalStrength;
};

class NetworkTools
{
public:
	static std::vector<sWirelessNetwork> ScanNetworks();
	static bool ConnectNetwork(std::string ssid, std::string password);
	static std::string GetConnectedNetwork();
	
private:

};