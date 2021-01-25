#include "WebServer.h"

WebServer::WebServer(Address addr) :
	httpEndpoint(std::make_shared<Http::Endpoint>(addr))
{
	auto opts = Http::Endpoint::options().threads(2);
	httpEndpoint->init(opts);
	router = new Rest::Router();
	discovery = std::make_shared<Discovery>(
		router,
		std::bind(&WebServer::getSettings, this, placeholders::_1),
		std::bind(&WebServer::setSettings, this, placeholders::_1)
	);
	createSettings();
	createRoutes();
	httpEndpoint->setHandler(router->handler());
	wsThread = std::thread(&WebServer::start, this);
}

void WebServer::start()
{
	{		
		httpEndpoint->serve();
	}	
}

void WebServer::createRoutes()
{
	Rest::Routes::Get(*router, "/logs", Rest::Routes::bind(&WebServer::getLogs, this));
}
/*
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
	
	//createSettings();
}
*/
void WebServer::createSettings()
{
	discovery->AddSetting(
		Setting(
			"lightStartHour",
			"Starting hour for the grow light",
			Setting::eINT,
			0,
			23,
			Settings::GetInstance()->LightStartHour(),
			false
		)
	);
	discovery->AddSetting(
		Setting(
			"lightStartMin",
		"Starting minute for the grow light",
		Setting::eINT,
		0,
		59,
		Settings::GetInstance()->LightStartMin(),
		false)
	);
	discovery->AddSetting(
		Setting(
			"lightDuration",
		"Duration in minutes grow light will be on",
		Setting::eINT,
		0,
		1440,
		Settings::GetInstance()->LightDuration(),
		false)
);
	discovery->AddSetting(
		Setting(
			"pumpRunTime",
		"Duration in minutes to run pumps",
		Setting::eINT,
		0,
		59,
		Settings::GetInstance()->PumpRunTime(),
		false)
);
	discovery->AddSetting(
		Setting(
			"dailyMLFood",
		"Number of milliliters of food per day",
		Setting::eFLOAT,
		0.0,
		200.0,
		Settings::GetInstance()->DailyMLFood(),
		false)
);
	discovery->AddSetting(
		Setting(	
			"networkSSID1",
			"SSID of a wireless network",
			Setting::eSTR,
			"",
			false		
		)
	);
	discovery->AddSetting(
	Setting(	
		"networkSSID2",
		"SSID of a wireless network",
		Setting::eSTR,
		"",
		false)
);
	discovery->AddSetting(
	Setting(	
		"networkSSID3",
		"SSID of a wireless network",
		Setting::eSTR,
		"",
		false)
);
	discovery->AddSetting(
	Setting(	
		"networkSSID4",
		"SSID of a wireless network",
		Setting::eSTR,
		"",
		false)
);
	discovery->AddSetting(
	Setting(	
		"networkSSID5",
		"SSID of a wireless network",
		Setting::eSTR,
		"",
		false)
);
	discovery->AddSetting(
		Setting(
			"networkPassword1",
			"Password of wireless network",
		Setting::eSTR,
		"",
		false
		)
	);
	discovery->AddSetting(
		Setting(
			"networkPassword2",
		"Password of wireless network",
		Setting::eSTR,
		"",
		false)
	);
	discovery->AddSetting(
		Setting(
			"networkPassword3",
		"Password of wireless network",
		Setting::eSTR,
		"",
		false)
	);
	discovery->AddSetting(
		Setting(
			"networkPassword4",
		"Password of wireless network",
		Setting::eSTR,
		"",
		false)
	);
	discovery->AddSetting(
		Setting(
			"networkPassword5",
		"Password of wireless network",
		Setting::eSTR,
		"",
		false)
	);
	discovery->AddSetting(
		Setting(
			"networkDefault1",
		"Is network default",
		Setting::eBOOL,
		false,
		false)
	);
	discovery->AddSetting(
		Setting(
			"networkDefault2",
		"Is network default",
		Setting::eBOOL,
		false,
		false)
	);
	discovery->AddSetting(
		Setting(
			"networkDefault3",
		"Is network default",
		Setting::eBOOL,
		false,
		false)
	);
	discovery->AddSetting(
		Setting(
			"networkDefault4",
		"Is network default",
		Setting::eBOOL,
		false,
		false)
	);
	discovery->AddSetting(
		Setting(
			"networkDefault5",
		"Is network default",
		Setting::eBOOL,
		false,
		false)
	);
	discovery->AddSetting(
		Setting(
			"hostname",
			"HydroGarden device hostname",
			Setting::eSTR,
			"",
			false
		)
	);
}
/*
void WebServer::getStates(const Rest::Request &req, Http::ResponseWriter response)
{
	std::stringstream ss;
	ss << "getStates() from: " << req.address().host();
	LOGI(ss.str());
	response.send(Http::Code::Ok, StateChange::GetInstance()->ToJSON());
}

void WebServer::clearLogs(const Rest::Request &req, Http::ResponseWriter response)
{
	Logger::GetInstance()->ClearLog();
	response.send(Http::Code::Ok, "Logs Cleared");
}
*/

void WebServer::getLogs(const Rest::Request &req, Http::ResponseWriter response)
{
	std::stringstream ss;
	ss << "getLogs() from: " << req.address().host();
	LOGI(ss.str());

	response.send(Http::Code::Ok, Logger::GetInstance()->ToJSON());
}

/*
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
	bool received = false;
	try
	{
		int len = req.body().length();
		std::string reqBody = req.body();
		std::string tag = "\r\n\r\n";
		int start = reqBody.find(tag);
		int end= reqBody.rfind("WebKitFormBoundary");
		std::string file = reqBody.substr(start+tag.length(), end-start);
		std::ofstream out;
		out.open("/tmp/HydroGarden.pkg",ios_base::openmode::_S_bin);
		out << file;
		out.close();
		LOGI("Received Update Successfully");		
		received = true;
	}
	catch (const std::exception &e)
	{
		std::stringstream ss;
		ss << "Error receiveing Update, exception: " << e.what();
		LOGE(ss.str());
		return;
	}
	if (received)
	{
		sPackageStatus status = Package::InstallPackage("/tmp/HydroGarden.pkg");
		std::stringstream html;
	
		html << "<HTML><HEAD><TITLE>HydroGarden Upgrade</TITLE></HEAD>";
		html << "<body><h2>Update ";
		if (status.success)
			html << "Successful</h2>";
		else
			html << "Failed</h2><br><hr>";
		html << "<h3>Logs:</h3><br>";
		for (int i = 0; i < status.msgs.size(); i++)
		{
			sPackageMsg msg = status.msgs[i];
			std::string color = "#00BB00";
			if (!msg.success)
				color = "#BB0000";
			html << "<span style=\"foreground-color: " << color << "\">" << msg.msg << "</span><br>";
		}
		html << "</body></html>";
		response.send(Http::Code::Ok, html.str());
			
	}
}
*/
std::map<std::string, bool> WebServer::setSettings(std::map<std::string, Setting> settings)
{
	std::vector<sNetwork> networks;
	for (int i = 0; i < 5; i++)
		networks.push_back(sNetwork());
	
	for (std::map<std::string, Setting>::iterator it = settings.begin(); it != settings.end(); it++)
	{
		if (it->first == "lightStartHour")
			Settings::GetInstance()->SetLightStartHour(it->second.GetIntVal());
		if (it->first == "lightStartMin")
			Settings::GetInstance()->SetLightStartMinute(it->second.GetIntVal());
		if (it->first == "lightDuration")
			Settings::GetInstance()->SetLightDuration(it->second.GetIntVal());
		if (it->first == "pumpDuration")
			Settings::GetInstance()->SetPumpRunTime(it->second.GetIntVal());
		if (it->first == "hostname")
			Settings::GetInstance()->SetHostname(it->second.GetStrVal());
		if (it->first == "dailyMLFood")
			Settings::GetInstance()->SetDailyMLFood(it->second.GetFloatVal());
		for (int i = 1; i <= 5; i++)
		{
			std::stringstream s1, s2, s3;
			s1 << "networkSSID" << i;
			s2 << "networkPassword" << i;
			s3 << "networkDefault" << i;
			
			if (it->first == s1.str())
			{
				networks[i].ssid = it->second.GetStrVal();
			} 
			if (it->first == s2.str())
			{
				networks[i].password = it->second.GetStrVal(); 
			}
			if (it->first == s3.str())
			{
				networks[i].isDefault = it->second.GetBoolVal();
			}
		}
		
		if (networks.size() > 0)
		{
			Settings::GetInstance()->ClearNetworks();
			for (int i = 0; i < networks.size(); i++)
			{
				Settings::GetInstance()->AddNetwork(networks[i].ssid, networks[i].password, networks[i].isDefault);
			}
				
		}
	}
}
	
std::map<std::string, Setting> WebServer::getSettings(std::map<std::string, Setting> settings)
{
	settings["lightStartHour"].SetValue(Settings::GetInstance()->LightStartHour());
	settings["lightStartMin"].SetValue(Settings::GetInstance()->LightStartMin());
	settings["lightDuration"].SetValue(Settings::GetInstance()->LightDuration());
	settings["pumpDuration"].SetValue(Settings::GetInstance()->PumpRunTime());
	settings["dailyMLFood"].SetValue(Settings::GetInstance()->DailyMLFood());
	settings["hostname"].SetValue(Settings::GetInstance()->GetHostname());
	std::vector<sNetwork> networks = Settings::GetInstance()->GetNetworks();
	for (int i = networks.size(); i < 5; i++)
	{
		std::stringstream s1;
		int ctr = i + 1;
		s1 << "networkSSID" << ctr;
		settings[s1.str()].SetValue("");
		s1.str("");
		s1 << "networkPassword" << ctr;
		settings[s1.str()].SetValue("");
		s1.str("");
		s1 << "networkDefault" << ctr;
		settings[s1.str()].SetValue(false);
	}
	
	for (int i = 0; i < networks.size(); i++)
	{
		if (i > 4)
			break;
		std::stringstream s1;
		int ctr = i + 1;
		s1 << "networkSSID" << ctr;
		settings[s1.str()].SetValue(networks[i].ssid);
		s1.str("");
		s1 << "networkPassword" << ctr;
#ifdef NETWORKPASS_CLEARTEXT
		settings[s1.str()].SetValue(networks[i].password);
#else
		settings[s1.str()].SetValue("*");
#endif
		s1.str("");
		s1 << "networkDefault" << ctr;
		settings[s1.str()].SetValue(networks[i].isDefault);
	}
	
	return settings;
}
