/* rest_description.cc
   Mathieu Stefani, 27 février 2016
   
   Example of how to use the Description mechanism
*/

#include <chrono>
#include <pistache/http.h>
#include <pistache/description.h>
#include <pistache/endpoint.h>
#include <pistache/net.h>
#include <thread>
#include <fstream>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <vector>

using namespace std;

#include "Logs.h"
#include "SettingsEP.h"
#include "StateChange.h"
#include "Info.h"
#include "Package.h"
#include "Discovery/Discovery.h"

class WebServer {
public:
	WebServer(Address addr);
	~WebServer();
	
	void start();

private:
	std::string toDuration(time_t time);
	
	void getLogs(const Rest::Request &request, Http::ResponseWriter response);
	void clearLogs(const Rest::Request &request, Http::ResponseWriter response);
	void getStates(const Rest::Request &request, Http::ResponseWriter response);
	void getInfo(const Rest::Request &request, Http::ResponseWriter response);

	void updateStatic(const Rest::Request &request, Http::ResponseWriter response);
	void update(const Rest::Request &request, Http::ResponseWriter response);
	
	void createRoutes();
	
	void createSettings();
	
	std::map<std::string, SettingGroup> getSettings(std::map<std::string, SettingGroup> settings);
	std::map<std::string, bool> setSettings(std::map<std::string, SettingGroup> settings);
	
	std::thread wsThread;
	Pistache::Rest::Router *router;
	std::shared_ptr<Http::Endpoint> httpEndpoint;
	std::shared_ptr<Discovery> discovery;
};

