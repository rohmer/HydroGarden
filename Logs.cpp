#include "Logs.h"

void Logs::init(size_t thr)
{
	auto opts = Http::Endpoint::options()
	.threads(static_cast<int>(thr));
	httpEndpoint->init(opts);
	setupRoutes();
}

void Logs::setupRoutes()
{
	using namespace  Rest;
	Routes::Get(router, "/logs", Routes::bind(&Logs::onRequest, this));
}

void Logs::start() {
	httpEndpoint->setHandler(router.handler());
	httpEndpoint->serve();
}

void Logs::onRequest(const Http::Request &request, Http::ResponseWriter writer)
{
	std::stringstream ss;
	ss << "LogsEP from: " << request.address().host();
	LOGI(ss.str());
	std::deque<sLogMsg> logMessages = Logger::GetInstance()->GetLogMessages();
	nlohmann::json json;
	int i = 0;
	for (std::deque<sLogMsg>::iterator it = logMessages.begin(); it != logMessages.end(); it++)
	{
		nlohmann::json msg;
		msg["sev"] = it->level;
		msg["msg"] = it->msg;
		msg["time"] = it->logTime;
		json[i] = msg;
		i++;
	}
	
	std::stringstream sss;
	json >> sss;
	writer.send(Http::Code::Ok, sss.str());
}