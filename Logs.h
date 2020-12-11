#pragma once

#include <algorithm>
#include <deque>
#include <string>
#include <sstream>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <json.hpp>
#include "Logger.h"

using namespace Pistache;

class Logs 
{
public:
	explicit Logs(Address addr)
		: httpEndpoint(std::make_shared<Http::Endpoint>(addr))
	{}
	void init(size_t thr = 2);
	void start();
private:
	std::shared_ptr<Http::Endpoint> httpEndpoint;
	Rest::Router router;
	


	void setupRoutes();	
	void onRequest(const Http::Request &request, Http::ResponseWriter writer);
};