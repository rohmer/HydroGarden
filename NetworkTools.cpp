#include "NetworkTools.h"

bool NetworkTools::InitiateNetworkScan()
{
	init_wpa_cli();

	char buf[128];
	if (wpa_cli_execute("scan", buf, sizeof(buf)) < 0)
	{
		LOGE("Failed to initiate wireless scan");
		return false;
	}
	return true;
}

std::vector<sWirelessNetwork> NetworkTools::GetScanResults()
{
	std::vector<sWirelessNetwork> networks;	
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	if (wpa_cli_execute("scan_results", buf, buf_len) < 0)
	{
		return networks;
	}

	std::stringstream ss(buf);
	std::string line;
	while (std::getline(ss, line, '\n'))
	{
		std::string fields[5];
		int i = 0;
		std::string token;
		size_t current, previous = 0;

		std::istringstream l(line);
		while (std::getline(l, token, '\t'))
		{
			fields[i] = token;
			i++;
		}
		if (i == 5) 
		{
			sWirelessNetwork wn;
			wn.bssid = fields[0];
			wn.frequency = std::atoi(fields[1].c_str());
			wn.signalStrength = std::atoi(fields[2].c_str());
			wn.flags = fields[3];
			wn.ssid = fields[4];
			networks.push_back(wn);
		}
	}
	return networks;
}

sWifiStatus NetworkTools::GetWifiStatus()
{
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	sWifiStatus status;
	status.statusRetrieved = false;
	
	if (wpa_cli_execute("status", buf, buf_len) < 0)
	{
		return status;
	}
	
	std::stringstream ss(buf);
	std::string line;
	while (std::getline(ss, line, '\n'))
	{
		std::string fields[2];
		int i = 0;
		std::string token;
		size_t current, previous = 0;

		std::istringstream l(line);
		while (std::getline(l, token, '='))
		{
			fields[i] = token;
			i++;
		}
		
		if (i == 2)
		{
			if (fields[0] == "bssid")
				status.bssid = fields[1];
			if (fields[0] == "freq")
				status.freq = std::atoi(fields[1].c_str());
			if (fields[0] == "ssid")
				status.ssid = fields[1];
			if (fields[0] == "id")
				status.network_id = std::atoi(fields[1].c_str());
			if (fields[0] == "mode")
				status.mode = fields[1];
			if (fields[0] == "pairwise_cipher")
				status.pairwise_cipher = fields[1];
			if (fields[0] == "group_cipher")
				status.group_cipher = fields[1];
			if (fields[0] == "key_mgmt")
				status.key_mgmt= fields[1];
			if (fields[0] == "ip_address")
				status.ipaddress = fields[1];
			if (fields[0] == "uuid")
				status.uuid = fields[1];
			if (fields[0] == "wpa_state")
			{
				status.wpa_state = eWPAState::UNKNOWN;
				if (fields[1] == "ASSOCIATED")
					status.wpa_state = eWPAState::ASSOCIATED;

				if (fields[1] == "ASSOCIATING")
					status.wpa_state = eWPAState::ASSOCIATING;
				if (fields[1] == "AUTHENTICATING")
					status.wpa_state = eWPAState::AUTHENTICATING;
				if (fields[1] == "COMPLETED")
					status.wpa_state = eWPAState::COMPLETED;
				if (fields[1] == "DISCONNECTED")
					status.wpa_state = eWPAState::DISCONNECTED;
				if (fields[1] == "FOUR_WAY_HANDSHAKE")
					status.wpa_state = eWPAState::FOUR_WAY_HANDSHAKE;
				if (fields[1] == "GROUP_HANDSHAKE")
					status.wpa_state = eWPAState::GROUP_HANDSHAKE;
				if (fields[1] == "INACTIVE")
					status.wpa_state = eWPAState::INACTIVE;
				if (fields[1] == "INTERFACE_DISABLED")
					status.wpa_state = eWPAState::INTERFACE_DISABLED;
				if (fields[1] == "INVALID")
					status.wpa_state = eWPAState::INVALID;
				if (fields[1] == "SCANNING")
					status.wpa_state = eWPAState::SCANNING;
				if (fields[1] == "UNINITIALIZED")
					status.wpa_state = eWPAState::UNINITIALIZED;
			}
		}
	}
	return status;
}
std::string NetworkTools::GetConnectedNetwork()
{
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	
	sWifiStatus status = GetWifiStatus();
	if (status.wpa_state == eWPAState::COMPLETED)
		return status.ssid;
	return "";
}

bool NetworkTools::RemoveNetwork(uint networkID)
{
	std::stringstream ss;
	ss << "remove_network " << networkID;
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	if (wpa_cli_execute(ss.str().c_str(), buf, buf_len) < 0)
	{
		return false;
	}
	
	if (strcmp(buf, "OK\n") == 0)
		return true;
	return false;
}

bool NetworkTools::RemoveNetwork(std::string ssid)
{
	std::vector<sWPANetwork> networks = ListNetworks();
	for (std::vector<sWPANetwork>::iterator it = networks.begin(); it != networks.end(); it++)
	{
		if (it->ssid == ssid)
		{
			return RemoveNetwork(it->networkID);
		}
	}
	
	return true;
}

std::vector<sWPANetwork> NetworkTools::ListNetworks()

{
	std::vector<sWPANetwork> networks;
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	
	if (wpa_cli_execute("list_networks", buf, buf_len) < 0)
	{
		return networks;
	}
	
	std::stringstream ss(buf);
	std::string line;
	while (std::getline(ss, line, '\n'))
	{
		std::string fields[4];
		int i = 0;
		std::string token;
		size_t current, previous = 0;

		std::istringstream l(line);
		while (std::getline(l, token, '\t'))
		{
			fields[i] = token;
			i++;
		}
	
		if (i == 4)
		{
			sWPANetwork net;
			net.networkID = std::atoi(fields[0].c_str());
			net.ssid = fields[1];
			net.bssid = fields[2];
			net.flags = fields[3];
			networks.push_back(net);
		}
	}
	return networks;
}

bool NetworkTools::SetNetworkSSID(uint networkID, std::string ssid)
{
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	std::stringstream ss;
	ss << "set_network " << networkID << "  ssid \"" << ssid << "\"";
	std::string cmd = ss.str();
	if (wpa_cli_execute(ss.str().c_str(), buf, buf_len) < 0)
		return false;
	if (strcmp(buf, "OK\n") != 0)
		return false;
	
	return true;
}

int NetworkTools::AddNetwork()
{
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	if (wpa_cli_execute("add_network", buf, buf_len) < 0)
	{
		return -1;
	}
	
	return std::atoi(buf);
}

int NetworkTools::AddNetwork(std::string ssid)
{
	std::vector<sWPANetwork> networks = ListNetworks();
	for (std::vector<sWPANetwork>::iterator it = networks.begin(); it != networks.end(); it++)
		if (it->ssid == ssid)
			return it->networkID;

	int id = AddNetwork();
	if (id < 0)
		return id;
	
	if (!SetNetworkSSID(id, ssid))
	{
		RemoveNetwork(id);
		return -1;
	}
	
	return id;
}	

int NetworkTools::GetNetworkID(std::string ssid)
{
	std::vector<sWPANetwork> networks = ListNetworks();
	for (std::vector<sWPANetwork>::iterator it = networks.begin(); it != networks.end(); it++)
		if (it->ssid == ssid)
			return it->networkID;
	return -1;
}

bool NetworkTools::SetPasswordWEP(uint networkID, std::string password)
{
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	std::stringstream ss;
	ss << "set_network " << networkID << "  wep_key0 \"" << password << "\"";
	std::string cmd = ss.str();
	if (wpa_cli_execute(ss.str().c_str(), buf, buf_len) < 0)
		return false;
	if (strcmp(buf, "OK\n") != 0)
		return false;
	
	return true;
}
	
bool NetworkTools::SetPasswordWEP(std::string ssid, std::string password)
{
	int nID = GetNetworkID(ssid);
	if (nID != -1)
		return SetPasswordWEP(nID, password);
	return false;
}

bool NetworkTools::EnableNetwork(uint networkID)
{
	std::vector<sWPANetwork> networks = ListNetworks();
	for (std::vector<sWPANetwork>::iterator it = networks.begin(); it != networks.end(); it++)
		DisableNetwork(it->networkID);
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	std::stringstream ss;
	ss << "enable_network " << networkID;
	std::string cmd = ss.str();
	if (wpa_cli_execute(ss.str().c_str(), buf, buf_len) < 0)
		return false;
	if (strcmp(buf, "OK\n") != 0)
		return false;
	
	return true;
}
	
bool NetworkTools::EnableNetwork(std::string ssid)
{
	int nID = GetNetworkID(ssid);
	if (nID != -1)
		return EnableNetwork(nID);
	return false;
}

bool NetworkTools::SaveConfig()
{
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	if (wpa_cli_execute("save config", buf, buf_len) < 0)
		return false;
	return true;
}

bool NetworkTools::DisableNetwork(uint networkID)
{
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	std::stringstream ss;
	ss << "disable_network " << networkID;
	std::string cmd = ss.str();
	if (wpa_cli_execute(ss.str().c_str(), buf, buf_len) < 0)
		return false;
	if (strcmp(buf, "OK\n") != 0)
		return false;
	
	return true;
}
	
bool NetworkTools::DisableNetwork(std::string ssid)
{
	int nID = GetNetworkID(ssid);
	if (nID != -1)
		return DisableNetwork(nID);
	return false;
}

bool NetworkTools::ConnectWifi(std::string ssid, std::string password)
{
	int current = -1;
	std::vector<sWPANetwork> networks = ListNetworks();
	for (std::vector<sWPANetwork>::iterator it = networks.begin(); it != networks.end(); it++)
		if (it->flags.find("[CURRENT]") != std::string::npos)
			current = it->networkID;
	int id = AddNetwork(ssid);
	
	if (!SetPasswordWEP(id, password))
	{
		RemoveNetwork(id);
		return false;
	}
	
	if (!EnableNetwork(id))
	{
		RemoveNetwork(id);
		EnableNetwork(current);
		return false;
	}
	
	bool connected = false;
	int tries = 0;
	while (!connected && tries < 8)
	{
		usleep(5000);
		sWifiStatus status = GetWifiStatus();
		if (status.wpa_state == COMPLETED && status.ssid == ssid)
		{
			connected = true;
		}
		tries++;
	}
	
	if (connected)
	{
		SaveConfig();
		return true;
	}
	RemoveNetwork(id);
	EnableNetwork(current);

	return false;
}

bool NetworkTools::Reassociate()
{
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	init_wpa_cli();
	std::stringstream ss;
	
	if (wpa_cli_execute("reassociate", buf, buf_len) < 0)
		return false;
	if (strcmp(buf, "OK\n") != 0)
		return false;
	
	return true;
}