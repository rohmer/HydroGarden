#include "wpa_cli_oper.h"
#include "libwpa_cli_wrapper.h"

#include <stdlib.h>
#include <strings.h>
#include <string.h>

int get_wifi_config(WifiConfig* config)
{
	return -1;
}

static WifiListItem* get_wifi_network_list_from_line(const char* line, int line_len)
{
	char* buf = (char*)malloc(line_len + 1);
	memset(buf, 0x00, line_len + 1);
	memcpy(buf, line, line_len);

	const char *delim = "\t";

	char* p = NULL;
	char *saveptr = NULL;

	char fields[4][256] = { 0 };
	int field_count = 0;

	p = strtok_r(buf, delim, &saveptr);
	while (p != NULL)
	{
		strcpy(fields[field_count], p);
		++field_count;
		p = strtok_r(NULL, delim, &saveptr);
	}

	free(buf);
	buf = NULL;

	WifiListItem* item = NULL;
	if (field_count == 4)
	{
		item = (WifiListItem*)malloc(sizeof(WifiListItem));
		if (item == NULL)
		{
			return NULL;
		}
		memset(item, 0x00, sizeof(WifiListItem));
		field_count = 0;
		item->networkID = atoi(fields[0]);
		strncpy(item->ssid, fields[1], sizeof(item->ssid));
		if (strcmp(fields[3],"[CURRENT]")==0)
			item->isCurrent = 1;
		else
			item->isCurrent = 0;
	}

	return item;
}

smartlist_t* get_network_list()
{
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	if (wpa_cli_execute("list_networks", buf, buf_len) < 0)
	{
		return NULL;
	}
	smartlist_t* scan_results = smartlist_new();
	if (scan_results == NULL)
	{
		return NULL;
	}
	
	char* p = NULL;
	char *saveptr = NULL;
	const char *delim = "\n";
	
	p = strtok_r(buf, delim, &saveptr);
	while (p != NULL)
	{
		WifiListItem* item = get_wifi_network_list_from_line(p, strlen(p));
		if (item != NULL)
		{
			smartlist_add(scan_results, item);
		}
		p = strtok_r(NULL, delim, &saveptr);
	}

	return scan_results;
}

static WifiScanItem* get_wifi_scan_item_from_line(const char* line, int line_len)
{
	char* buf = (char*)malloc(line_len + 1);
	memset(buf, 0x00, line_len + 1);
	memcpy(buf, line, line_len);

	const char *delim = "\t";

	char* p = NULL;
	char *saveptr = NULL;

	char fields[5][256] = { 0 };
	int field_count = 0;

	p = strtok_r(buf, delim, &saveptr);
	while (p != NULL)
	{
		strcpy(fields[field_count], p);
		++field_count;
		p = strtok_r(NULL, delim, &saveptr);
	}

	free(buf);
	buf = NULL;

	WifiScanItem* item = NULL;
	if (field_count == 5)
	{
		item = (WifiScanItem*)malloc(sizeof(WifiScanItem));
		if (item == NULL)
		{
			return NULL;
		}
		memset(item, 0x00, sizeof(WifiScanItem));
		field_count = 0;
		strcpy(item->bssid, fields[field_count++]);
		item->frequency = atoi(fields[field_count++]);
		item->signal_level = atoi(fields[field_count++]);
		strcpy(item->flags, fields[field_count++]);
		strcpy(item->ssid, fields[field_count]); // TODO: 16进制转字符串
	}

	return item;
}

smartlist_t* get_wifi_scan_results()
{
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);

	if (wpa_cli_execute("scan_results", buf, buf_len) < 0)
	{
		return NULL;
	}

	smartlist_t* scan_results = smartlist_new();
	if (scan_results == NULL)
	{
		return NULL;
	}

	const char *delim = "\n";

	char* p = NULL;
	char *saveptr = NULL;

	p = strtok_r(buf, delim, &saveptr);
	while (p != NULL)
	{
		WifiScanItem* item = get_wifi_scan_item_from_line(p, strlen(p));
		if (item != NULL)
		{
			smartlist_add(scan_results, item);
		}
		p = strtok_r(NULL, delim, &saveptr);
	}
	return scan_results;
}

static int wpa_cli_comm_exec(const char* cmd)
{
	char buf[256] = { 0 };
	int buf_len = sizeof(buf);

	if (wpa_cli_execute(cmd, buf, buf_len) < 0)
	{
		return -1;
	}
	if (strncasecmp(buf, "OK", 2) != 0)
	{
		return -1;
	}

	return 0;
}

static int add_network(char* network_id, int len)
{
	char buf[256] = { 0 };
	int buf_len = sizeof(buf);

	// add_network
	if (wpa_cli_execute("add_network", buf, buf_len) < 0)
	{
		return -1;
	}

	strncpy(network_id, buf, strlen(buf)-1);
	return 0;
}

static int set_ssid(const char* network_id, const char* ssid)
{
	char cmd[256] = { 0 };
	snprintf(cmd, sizeof(cmd), "set_network %s ssid \"%s\"", network_id, ssid);

	return wpa_cli_comm_exec(cmd);
}


static int set_password(const char* network_id, int flag, const char* password)
{
	char set_cmd[256] = { 0 };

	int res = -1;

	// 0 wep, 1 wpa, 2 无密码
	switch (flag)
	{
	case 0:
	{
		// set_network 1 key_mgmt NONE
		snprintf(set_cmd, sizeof(set_cmd), "set_network %s key_mgmt NONE", network_id);
		if (wpa_cli_comm_exec(set_cmd) != 0)
		{
			break;
		}

		// > set_network 1 wep_key0 "your ap password"
		snprintf(set_cmd, sizeof(set_cmd), "set_network %s wep_key0 \"%s\"", network_id, password);
		res = wpa_cli_comm_exec(set_cmd);
		break;
	}
	case 1:
	{
		// set_network 2 psk "your pre-shared key"
		snprintf(set_cmd, sizeof(set_cmd), "set_network %s psk \"%s\"", network_id, password);
		res = wpa_cli_comm_exec(set_cmd);
		break;
	}
	case 2:
	{
		// 无密码的wifi
		break;
	}
	default:
	{
		break;
	}
	}

	return res;
}

static int enable_network(const char* network_id)
{
	char cmd[256] = { 0 };
	snprintf(cmd, sizeof(cmd), "enable_network %s", network_id);

	return wpa_cli_comm_exec(cmd);
}

static int disable_network(const char* network_id)
{
	char cmd[256] = { 0 };
	snprintf(cmd, sizeof(cmd), "disable_network %s", network_id);

	return wpa_cli_comm_exec(cmd);
}

static int remove_network(const char* network_id)
{
	char cmd[256] = { 0 };
	snprintf(cmd, sizeof(cmd), "remove_network %s", network_id);

	return wpa_cli_comm_exec(cmd);
}

int connect_wifi(WifiConfig* config)
{
	char network_id[16] = { 0 };

	uint reuseID = 0;
	smartlist_t *wifiNetList = get_network_list();
	for (int i = 0; i < wifiNetList->num_used; i++)
	{
		WifiListItem *wli = (WifiListItem *)smartlist_pop_last(wifiNetList);
		char liid[16] = { 0 };
		
		if (strcmp(config->ssid, wli->ssid) == 0)
		{
			reuseID = 1;
			snprintf(network_id, sizeof(network_id), "%d", wli->networkID);
		}
	}
	
	do
	{
		if(reuseID==0)
			if (add_network(network_id, sizeof(network_id)) != 0)
			{
				break;
			}

		if (set_ssid(network_id, config->ssid) != 0)
		{
			break;
		}
		if (set_password(network_id, config->flag, config->password) != 0)
		{
			break;
		}

		if (enable_network(network_id) != 0)
		{

			break;
		}

		if (reassociate() != 0)
		{
			break;
		}
		
		return 0;
		

	} while (0);


	if (network_id[0] != '\0')
	{
		// TODO: delete network
		disable_network(network_id);
		remove_network(network_id);
	}

	return -1;
}

int disconnect_wifi(WifiConfig* config)
{
	
	return -1;
}

eWPAState wpaStateFromString(char *str)
{
	if (strcmp(str, "ASSOCIATED") == 0)
		return ASSOCIATED;
	if (strcmp(str, "ASSOCIATING") == 0)
		return ASSOCIATING;
	if (strcmp(str, "AUTHENTICATING") == 0)
		return AUTHENTICATING;
	if (strcmp(str, "AUTHENTICATING") == 0)
		return AUTHENTICATING;
	if (strcmp(str, "COMPLETED") == 0)
		return COMPLETED;
	if (strcmp(str, "DISCONNECTED") == 0)
		return DISCONNECTED;
	if (strcmp(str, "FOUR_WAY_HANDSHAKE") == 0)
		return FOUR_WAY_HANDSHAKE;
	if (strcmp(str, "GROUP_HANDSHAKE") == 0)
		return GROUP_HANDSHAKE;
	if (strcmp(str, "INACTIVE") == 0)
		return INACTIVE;
	if (strcmp(str, "INTERFACE_DISABLED") == 0)
		return INTERFACE_DISABLED;
	if (strcmp(str, "INVALID") == 0)
		return INVALID;
	if (strcmp(str, "SCANNING") == 0)
		return SCANNING;
	if (strcmp(str, "UNINITIALIZED") == 0)
		return UNINITIALIZED;
	return UNKNOWN;
}

WifiStatus get_wifi_status()
{
	char buf[4096] = { 0 };
	int buf_len = sizeof(buf);
	WifiStatus status;
	status.statusRetrieved = -1;
	if (wpa_cli_execute("status", buf, buf_len) < 0)
	{
		return status;
	}
	status.statusRetrieved = 1;
	char* p = NULL;
	char *saveptr = NULL;
	const char *delim = "\n";
	
	p = strtok_r(buf, delim, &saveptr);
	char* line = (char*)malloc(129);
		
	while (p != NULL)
	{
		memset(line, 0x00, 129);
		strncpy(line, p, 128);
		char *token = strsep(&line, "=");
		if (token != NULL)
		{
			char *value = line;
			if (strcmp(token, "bssid") == 0)
				strncpy(status.bssid, line, sizeof(status.bssid));
			if (strcmp(token, "freq") == 0)
				status.freq = atoi(line);
			if (strcmp(token, "ssid") == 0)
				strncpy(status.ssid, line, sizeof(status.ssid));
			if (strcmp(token, "id") == 0)
				status.network_id = atoi(line);
			if (strcmp(token, "mode") == 0)
				strncpy(status.mode, line, sizeof(status.mode));
			if (strcmp(token, "pairwise_cipher") == 0)
				strncpy(status.pairwise_cipher, line, sizeof(status.pairwise_cipher));
			if (strcmp(token, "group_cipher") == 0)
				strncpy(status.group_cipher, line, sizeof(status.group_cipher));
			if (strcmp(token, "group_cipher") == 0)
				strncpy(status.group_cipher, line, sizeof(status.group_cipher));
			if (strcmp(token, "key_mgmt") == 0)
				strncpy(status.key_mgmt, line, sizeof(status.key_mgmt));
			if (strcmp(token, "group_cipher") == 0)
				strncpy(status.group_cipher, line, sizeof(status.group_cipher));
			if (strcmp(token, "wpa_state") == 0)
				status.wpa_state = wpaStateFromString(line);
			if (strcmp(token, "ip_address") == 0)
				strncpy(status.ipaddress, line, sizeof(status.ipaddress));
			if (strcmp(token, "address") == 0)
				strncpy(status.address, line, sizeof(status.address));
			if (strcmp(token, "uuid") == 0)
				strncpy(status.uuid, line, sizeof(status.uuid));

			
		}
		p = strtok_r(NULL, delim, &saveptr);
	}
	return status;
}
