#include "WebServer.h"

WebServer::WebServer(Address addr) :
	httpEndpoint(std::make_shared<Http::Endpoint>(addr))
{	
	auto opts = Http::Endpoint::options().threads(2);
	httpEndpoint->init(opts);
	router = new Rest::Router();
	discovery = std::make_shared<Discovery>(
		1973,
		1,
		true,
		true,
		"HydroGarden",
		Version().ToJSON(),
		router,
		std::bind(&WebServer::getSettings, this, placeholders::_1),
		std::bind(&WebServer::setSettings, this, placeholders::_1),
		std::filesystem::path("Hydro.png")
	);
	createSettings();
	createRoutes();
	discovery->InitNetworkDiscovery();
	httpEndpoint->setHandler(router->handler());
	wsThread = std::thread(&WebServer::start, this);
}

WebServer::~WebServer()
{
	httpEndpoint->shutdown();
}
	
void WebServer::start()
{
	{		
		httpEndpoint->serve();
	}	
}

void WebServer::createRoutes()
{
	discovery->AddEndpoint(Endpoint("/logs", Endpoint::GET), Rest::Routes::bind(&WebServer::getLogs, this));
	discovery->AddEndpoint(Endpoint("/logs/clear", Endpoint::GET), Rest::Routes::bind(&WebServer::clearLogs, this));	
	discovery->AddEndpoint(Endpoint("/info", Endpoint::GET), Rest::Routes::bind(&WebServer::getInfo, this));
	discovery->AddEndpoint(Endpoint("/states", Endpoint::GET), Rest::Routes::bind(&WebServer::getStates, this));	
	discovery->AddEndpoint(Endpoint("/update", Endpoint::POST), Rest::Routes::bind(&WebServer::update, this));	
	discovery->AddEndpoint(Endpoint("/update", Endpoint::GET), Rest::Routes::bind(&WebServer::updateStatic, this));
}

void WebServer::createSettings()
{
	discovery->AddSettingGroup("Status", ImgJSON::ImageFromFile("status.png"));
	discovery->AddSetting(
		Setting(
			"Light",
			"Light Status",
			Setting::eBOOL,
			false,
			false
			),
			"Status"
		);
	discovery->AddSetting(
		Setting(
			"Water",
			"The water level",
			Setting::UNKNOWN),
		"Status");
	discovery->AddSetting(
		Setting(
			"Feed Level",
		"The level of plant food",
		Setting::UNKNOWN),
		"Status");
	discovery->AddSetting(
		Setting(
			"Pump",
		"Pump Status",
		Setting::eBOOL,
		false,
			false),
		"Status"
	);
	discovery->AddSetting(
		Setting(
			"Light Duration",
			"Time until light toggles",
		Setting::eSTR,
		"",
		true),
		"Status");
	discovery->AddSetting(
		Setting(
			"Pump Duration",
		"Time until pump toggles",
		Setting::eSTR,
		"",
			true),
		"Status");
	discovery->AddSetting(
		Setting(
			"Feeding",
		"Time to next feeding",
		Setting::eSTR,
		"",
		true));
	discovery->AddSettingGroup("Logs", ImgJSON::ImageFromFile("log.png"));
	discovery->AddSetting(
		Setting(
			"Logs",
			"Application Logs",
			Setting::eSTRINGLIST),
		"Logs");
	discovery->AddSetting(
		Setting(
			"Clear",
			"Clear Application Logs",
		Setting::eBOOL,
		false,
		false
		),
		"Logs"
	);
	discovery->AddSettingGroup("General", ImgJSON::ImageFromFile("settings.png"));
	discovery->AddSetting(
		Setting(
			"lightStartHour",
			"Starting hour for the grow light",
			Setting::eINT,
			0,
			23,
			Settings::GetInstance()->LightStartHour(),
			false
		),
		"General"
	);
	discovery->AddSetting(
		Setting(
		"lightStartMin",
		"Starting minute for the grow light",
		Setting::eINT,
		0,
		59,
		Settings::GetInstance()->LightStartMin(),
			false),
		"General"
	);
	discovery->AddSetting(
		Setting(
			"lightDuration",
		"Duration in minutes grow light will be on",
		Setting::eINT,
		0,
		1440,
		Settings::GetInstance()->LightDuration(),
			false),
		"General"
);
	discovery->AddSetting(
		Setting(
			"pumpRunTime",
		"Duration in minutes to run pumps",
		Setting::eINT,
		0,
		59,
		Settings::GetInstance()->PumpRunTime(),
			false),
		"General"
);
	discovery->AddSetting(
		Setting(
			"dailyMLFood",
		"Number of milliliters of food per day",
		Setting::eFLOAT,
		0.0,
		200.0,
		Settings::GetInstance()->DailyMLFood(),
			false),
		"General"
);
	discovery->AddSetting(
		Setting(	
			"networkSSID1",
			"SSID of a wireless network",
			Setting::eSTR,
			"",
			false),
		"General"
	);
	discovery->AddSetting(
	Setting(	
		"networkSSID2",
		"SSID of a wireless network",
		Setting::eSTR,
		"",
			false),
		"General"
);
	discovery->AddSetting(
	Setting(	
		"networkSSID3",
		"SSID of a wireless network",
		Setting::eSTR,
		"",
			false),
		"General"
);
	discovery->AddSetting(
	Setting(	
		"networkSSID4",
		"SSID of a wireless network",
		Setting::eSTR,
		"",
			false),
		"General"
);
	discovery->AddSetting(
	Setting(	
		"networkSSID5",
		"SSID of a wireless network",
		Setting::eSTR,
		"",
			false),
		"General"
);
	discovery->AddSetting(
		Setting(
			"networkPassword1",
			"Password of wireless network",
		Setting::eSTR,
		"",
		false),
		"General"
	);
	discovery->AddSetting(
		Setting(
			"networkPassword2",
		"Password of wireless network",
		Setting::eSTR,
		"",
			false),
		"General"
	);
	discovery->AddSetting(
		Setting(
			"networkPassword3",
		"Password of wireless network",
		Setting::eSTR,
		"",
			false),
		"General"
	);
	discovery->AddSetting(
		Setting(
			"networkPassword4",
		"Password of wireless network",
		Setting::eSTR,
		"",
			false),
		"General"
	);
	discovery->AddSetting(
		Setting(
			"networkPassword5",
		"Password of wireless network",
		Setting::eSTR,
		"",
			false),
		"General"
	);
	discovery->AddSetting(
		Setting(
			"networkDefault1",
		"Is network default",
		Setting::eBOOL,
		false,
			false),
		"General"
	);
	discovery->AddSetting(
		Setting(
			"networkDefault2",
		"Is network default",
		Setting::eBOOL,
		false,
			false),
		"General"
	);
	discovery->AddSetting(
		Setting(
			"networkDefault3",
		"Is network default",
		Setting::eBOOL,
		false,
			false),
		"General"
	);
	discovery->AddSetting(
		Setting(
			"networkDefault4",
		"Is network default",
		Setting::eBOOL,
		false,
			false),
		"General"
	);
	discovery->AddSetting(
		Setting(
			"networkDefault5",
		"Is network default",
		Setting::eBOOL,
		false,
			false),
		"General"
	);
	discovery->AddSetting(
		Setting(
			"hostname",
			"HydroGarden device hostname",
			Setting::eSTR,
			"",
			false),
		"General"
	);
	
	
}

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

std::map<std::string, bool> WebServer::setSettings(std::map<std::string, SettingGroup> settings)
{
	std::vector<sNetwork> networks;
	for (int i = 0; i < 5; i++)
		networks.push_back(sNetwork());
	
	std::map<std::string, bool> ret;
	for (std::map<std::string, SettingGroup>::iterator it = settings.begin(); it != settings.end(); it++)
		for (std::map<std::string, Setting>::iterator i2 = it->second.GetSettings().begin();
			i2 != it->second.GetSettings().end();
			i2++)
			ret.emplace(i2->second.SettingName(), false);
	
	if (settings.find("Logs") != settings.end())
	{
		std::map<std::string,Setting> set = settings["Logs"].GetSettings();
		if(set.find("Logs")!=set.end())
		{		
			if (set["Logs"].GetBoolVal())
				{
					Logger::GetInstance()->ClearLog();
					ret.emplace("Clear", true);
				}
			}
	}
	if (settings.find("Status") != settings.end()) 
	{
		std::map<std::string, Setting> status = settings["Status"].GetSettings();
		

		if (status.find("Light") != status.end())
		{
			if (Hardware::LightStatus())
				Hardware::LightsOff();
			else
				Hardware::LightsOn();
		}
		if (status.find("Pump")!=status.end())
		{
			if (Hardware::PumpStatus())
				Hardware::PumpsOff();
			else
				Hardware::PumpsOn();
		}
	}
	
	
	if (settings.find("General") != settings.end())
	{
		SettingGroup sg = settings["General"];
		if (sg.HasSetting("lightStartHour"))
		{
			Settings::GetInstance()->SetLightStartHour(sg.GetSetting("lightStartHour").GetIntVal());
		}
		if (sg.HasSetting("lightStartMin"))
		{
			Settings::GetInstance()->SetLightStartMinute(sg.GetSetting("lightStartMinute").GetIntVal());
		}
		if (sg.HasSetting("lightDuration"))
		{
			Settings::GetInstance()->SetLightDuration(sg.GetSetting("lightDuration").GetIntVal());
		}
		if (sg.HasSetting("pumpDuration"))
		{
			Settings::GetInstance()->SetPumpRunTime(sg.GetSetting("pumpDuration").GetIntVal());
		}
		if (sg.HasSetting("hostname"))
		{
			Settings::GetInstance()->SetHostname(sg.GetSetting("hostname").GetStrVal());
		}
		if (sg.HasSetting("dailyMLFood"))
		{
			Settings::GetInstance()->SetDailyMLFood(sg.GetSetting("dailyMLFood").GetIntVal());
		}
		
		for (int i = 1; i <= 5; i++)
		{
			std::stringstream s1, s2, s3;
			s1 << "networkSSID" << i;
			s2 << "networkPassword" << i;
			s3 << "networkDefault" << i;
			
			if (sg.HasSetting(s1.str()))
			{
				networks[i].ssid = sg.GetSetting(s1.str()).GetStrVal();
				ret.emplace(s1.str(), true);
			} 
			if (sg.HasSetting(s2.str()))
			{
				networks[i].password = sg.GetSetting(s2.str()).GetStrVal();
				ret.emplace(s2.str(), true);
			}
			if (sg.HasSetting(s3.str()))
			{
				networks[i].isDefault = sg.GetSetting(s3.str()).GetBoolVal();
				ret.emplace(s3.str(), true);
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
	
	return ret;
}
	
std::map<std::string, SettingGroup> WebServer::getSettings(std::map<std::string, SettingGroup> settings)
{
	std::vector<Setting> set, logs, status;
	
	if (settings.find("Logs") != settings.end())
	{
		std::deque<sLogMsg> logs = Logger::GetInstance()->GetLogMessages();
		std::vector<std::string> logMsgs;
		for (int i = logs.size()-1; i >= 0; i--)
		{
			std::stringstream ss;
			sLogMsg logMsg = logs.at(i);
			char mbstr[100];
			std::strftime(mbstr, 100, "%d/%m/%Y %T", std::localtime(&logMsg.logTime));
			ss << " " << mbstr;
			switch (logMsg.level)
			{
			case eLogLevel::ERROR:
				ss << " ERROR";
				break;
			case eLogLevel::WARN:
				ss << " WARN";
				break;
			case eLogLevel::INFO:
				ss << " INFO";
				break;
			}
			
			ss << " " << logMsg.msg;
			logMsgs.push_back(ss.str());
		}
		
		if (settings.find("Logs") != settings.end())
		{
			SettingGroup sg = settings["Logs"];
			if (sg.HasSetting("Logs"))
			{
				sg.GetSetting("Logs").SetValue(logMsgs);
			}
			settings["Logs"] = sg;
		}
	
		
		if (settings.find("General") != settings.end())
		{
			SettingGroup sg = settings["General"];
			if (sg.HasSetting("lightStartHour"))
				sg.SetSettingValue("lightStartHour", (int)Settings::GetInstance()->LightStartHour());
			if (sg.HasSetting("lightStartMin"))
				sg.SetSettingValue("lightStartMin", (int)Settings::GetInstance()->LightStartMin());
			
			if (sg.HasSetting("lightDuration"))
				sg.SetSettingValue("lightDuration", (int)Settings::GetInstance()->LightDuration());
			if (sg.HasSetting("pumpDuration"))
				sg.SetSettingValue("pumpDuration", (int)Settings::GetInstance()->PumpRunTime());
			if (sg.HasSetting("dailyMLFood"))
				sg.SetSettingValue("dailyMLFood", Settings::GetInstance()->DailyMLFood());
			if (sg.HasSetting("hostname"))
				sg.SetSettingValue("hostname", Settings::GetInstance()->GetHostname());

			std::vector<sNetwork> networks = Settings::GetInstance()->GetNetworks();
			int topVal = 5;
			if (networks.size() < topVal)
				topVal = networks.size();
			
			for (int i = 1; i <= topVal; i++)
			{
				std::stringstream s1;
				s1 << "networkSSID" << i;
				sg.SetSettingValue(s1.str(), networks[i].ssid);
				std::stringstream s2;
				s2 << "networkPassword" << i;
				#ifdef NETWORKPASS_CLEARTEXT
				sg.SetSettingValue(s2.str(), networks[i].password);
				#else
				sg.SetSettingValue(s2.str(), "***");
				#endif
				std::stringstream s3;
				s3 << "networkDefault" << i;
				sg.SetSettingValue(s3.str(), networks[i].isDefault);					
			}
			settings["General"] = sg;
		}
		
		if (settings.find("Status") != settings.end())
		{
			
			SettingGroup status = settings["Status"];
			if (status.HasSetting("Pump"))
				status.SetSettingValue("Pump", Hardware::PumpStatus());
			if (status.HasSetting("Light"))
			{
				status.SetSettingValue("Light", Hardware::LightStatus());
			}
			if (status.HasSetting("Light Duration"))
			{
				time_t t;
				if (Hardware::LightStatus())
					t = Scheduler::GetInstance()->GetNextLightOff();
				else
					t = Scheduler::GetInstance()->GetNextLightOff();
				status.SetSettingValue("Light Duration", toDuration(t - time(nullptr)));
			}
			if (status.HasSetting("Pump Duration"))
			{
				time_t t;
				if (Hardware::PumpStatus())
					t = Scheduler::GetInstance()->GetNextPumpOff();
				else
					t = Scheduler::GetInstance()->GetNextPumpOn();
				status.SetSettingValue("Pump Duration", toDuration(t - time(nullptr)));
			}
			if (status.HasSetting("Feeding"))
			{
				status.SetSettingValue("Feeding", toDuration(Scheduler::GetInstance()->GetNextFeeding() - time(nullptr)));
			}
			if (status.HasSetting("Water"))
			{
				if (Hardware::WaterLevel())
					status.SetSettingValue("Water", Setting::GREEN);
				else
					status.SetSettingValue("Water", Setting::RED);
			}
			if (status.HasSetting("Feed"))
			{
				if(Hardware::FeedLevel())
					status.SetSettingValue("Feed", Setting::GREEN);
				else
					status.SetSettingValue("Feed", Setting::RED);
			}
			settings["Status"] = status;
			
		}
		
	}
	return settings;
}

std::string WebServer::toDuration(time_t time)
{
	int remain=time, hour, min, sec;
	hour = time / (60 * 60);
	remain -= hour*60*60;
	min = remain / 60;
	sec = remain - min*60;
	std::stringstream ss;
	ss << printf("%02d", hour) << ":" << printf("%02d", min) << ":" << printf("%02d", sec);
	return ss.str();
	
}