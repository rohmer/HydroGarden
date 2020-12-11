#include "WebServer.h"

void WebServer::init(size_t thr)
{
	auto opts = Http::Endpoint::options()
	    .threads(static_cast<int>(thr));
	httpEndpoint->init(opts);
	createDescription();
	
	wsThread = std::thread(&WebServer::start, this);
}

void WebServer::start()
{
	{
		router.initFromDescription(desc);

		Rest::Swagger swagger(desc);
		swagger
		    .uiPath("/doc")
		    .uiDirectory("/home/octal/code/web/swagger-ui-2.1.4/dist")
		    .apiPath("/hydrogarden-api.json")
		    .install(router);

		httpEndpoint->setHandler(router.handler());
		httpEndpoint->serve();
	}	
}

void WebServer::createDescription()
{
	desc
		.info()
	    .license("Apache", "http://www.apache.org/licenses/LICENSE-2.0");

	auto backendErrorResponse =
	    desc.response(Http::Code::Internal_Server_Error, "An error occured with the backend");

	desc
		.schemes(Rest::Scheme::Http)
		.basePath("/v1")
		.produces(MIME(Application, Json))
		.consumes(MIME(Application, Json));

	auto versionPath = desc.path("/v1");

	desc
		.route(desc.post("/update"))
		.bind(&WebServer::update, this)
		.produces(MIME(Application, Json), MIME(Application, Xml))
		.response(Http::Code::Ok, "HydroGarden Update Processor");
	desc
		.route(desc.get("/update"))
		.bind(&WebServer::updateStatic, this)
		.produces(MIME(Text, Html))
		.response(Http::Code::Ok, "HydroGarden Update");
	
	desc
	    .route(desc.get("/settings"))
	    .bind(&WebServer ::getSettings, this)
	    .produces(MIME(Application, Json), MIME(Application, Xml))
	    .response(Http::Code::Ok, "HydroGarden Settings");

	desc
		.route(desc.get("/logs"))
		.bind(&WebServer::getLogs, this)
		.produces(MIME(Application, Json), MIME(Application, Xml))
		.response(Http::Code::Ok, "HydroGarden Logs");
	
	desc
		.route(desc.get("/logs/clear"))
		.bind(&WebServer::clearLogs, this)
		.produces(MIME(Application, Json), MIME(Application, Xml))
		.response(Http::Code::Ok, "HydroGarden Clear Logs");
	
	desc
		.route(desc.get("/states"))
		.bind(&WebServer::getStates, this)
		.produces(MIME(Application, Json), MIME(Application, Xml))
		.response(Http::Code::Ok, "HydroGarden State Changes");
	desc
		.route(desc.get("/info"))
		.bind(&WebServer::getInfo, this)
		.produces(MIME(Application, Json), MIME(Application, Xml))
		.response(Http::Code::Ok, "HydroGarden Info");
}

void WebServer::getStates(const Rest::Request &req, Http::ResponseWriter response)
{
	std::stringstream ss;
	ss << "getStates() from: " << req.address().host();
	LOGI(ss.str());
	response.send(Http::Code::Ok, StateChange::GetInstance()->ToJSON());
}

void WebServer::getSettings(const Rest::Request &req, Http::ResponseWriter response)
{
	std::stringstream ss;
	ss << "getSetttings() from: " << req.address().host();
	LOGI(ss.str());
	response.send(Http::Code::Ok, Settings::GetInstance()->ToJSON());
}

void WebServer::clearLogs(const Rest::Request &req, Http::ResponseWriter response)
{
	Logger::GetInstance()->ClearLog();
	response.send(Http::Code::Ok, "Logs Cleared");
}

void WebServer::getLogs(const Rest::Request &req, Http::ResponseWriter response)
{
	std::stringstream ss;
	ss << "getLogs() from: " << req.address().host();
	LOGI(ss.str());

	response.send(Http::Code::Ok, Logger::GetInstance()->ToJSON());
}

void WebServer::getInfo(const Rest::Request &req, Http::ResponseWriter response)
{
	response.send(Http::Code::Ok, Info::GetInfo());
}

void WebServer::updateStatic(const Rest::Request &req, Http::ResponseWriter response)
{
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);
	std::string ipAddr;
	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) 
	{
		if (!ifa->ifa_addr) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET) {
			// check it is IP4
		   // is a valid IP4 Address
		   tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			
			if (strcmp(addressBuffer, "127.0.0.1") != 0)
			{
				std::stringstream tmp;
				tmp << addressBuffer;
				ipAddr = tmp.str();
				std::stringstream ss;
				ss << ifa->ifa_name << " IP Addr: " << ipAddr;
				LOGI(ss.str());
			}
		}
	}
	std::stringstream html;
	html << "<HTML><HEAD><TITLE>HydroGarden Upgrade</TITLE></HEAD>";
	html << "<BODY><FORM ACTION=\"http://" << ipAddr << ":8080/update\" method=\"post\"  enctype=\"multipart/form-data\">";
	html << "<INPUT TYPE=\"file\" name=\"updateFile\" id=\"updateFile\"><br>";
	html << "<INPUT TYPE=\"submit\" value=\"Update HydroGarden\">";
	html << "</FORM></BODY></HTML>";
	response.send(Http::Code::Ok, html.str());
}


void WebServer::update(const Rest::Request &req, Http::ResponseWriter response)
{
	try
	{
		int len = req.body().length();
		std::string reqBody = req.body();
		std::string tag = "gzip";
		int start = reqBody.find(tag);
		int end= reqBody.rfind("WebKitFormBoundary");
		std::string file = reqBody.substr(start+tag.length()+4, end-start-16);
		std::ofstream out;
		out.open("/tmp/HydroGarden.tar.gz");
		out << file;
		out.close();
		LOGI("Received Update Successfully");
	}
	catch (const std::exception &e)
	{
		std::stringstream ss;
		ss << "Error receiveing Update, exception: " << e.what();
		LOGE(ss.str());
		return;
	}
	std::stringstream html;
	html << "<HTML><HEAD><TITLE>HydroGarden Upgrade</TITLE></HEAD>";
	html << "<body><h2>Update queued</h2></body></html>";
	response.send(Http::Code::Ok, html.str());
}