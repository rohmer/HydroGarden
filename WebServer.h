/* rest_description.cc
   Mathieu Stefani, 27 février 2016
   
   Example of how to use the Description mechanism
*/

#include <pistache/http.h>
#include <pistache/description.h>
#include <pistache/endpoint.h>
#include <thread>
#include <fstream>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

using namespace std;
using namespace Pistache;

#include "Logs.h"
#include "SettingsEP.h"
#include "StateChange.h"
#include "Info.h"

class WebServer {
public:
	WebServer(Address addr)
		: httpEndpoint(std::make_shared<Http::Endpoint>(addr))
		, desc("HydroGarden API", "0.1")
	{}

	void init(size_t thr = 2);

	void start();

private:
	void getSettings(const Rest::Request &request, Http::ResponseWriter response);
	void getLogs(const Rest::Request &request, Http::ResponseWriter response);
	void clearLogs(const Rest::Request &request, Http::ResponseWriter response);
	void getStates(const Rest::Request &request, Http::ResponseWriter response);
	void getInfo(const Rest::Request &request, Http::ResponseWriter response);

	void updateStatic(const Rest::Request &request, Http::ResponseWriter response);
	void update(const Rest::Request &request, Http::ResponseWriter response);
	
	void createDescription();
	

	
	std::thread wsThread;
	
	std::shared_ptr<Http::Endpoint> httpEndpoint;
	Rest::Description desc;
	Rest::Router router;
};

