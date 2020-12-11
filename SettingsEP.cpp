#include "SettingsEP.h"


void SettingsEP::init(size_t thr)
{
	auto opts = Http::Endpoint::options()
	.threads(static_cast<int>(thr));
	httpEndpoint->init(opts);
	setupRoutes();
}

void SettingsEP::setupRoutes()
{
	using namespace  Rest;
	Routes::Get(router, "/logs", Routes::bind(&SettingsEP::onRequest, this));
}

void SettingsEP::start() {
	httpEndpoint->setHandler(router.handler());
	httpEndpoint->serve();
}

void SettingsEP::onRequest(const Http::Request &request, Http::ResponseWriter writer)
{
	std::stringstream ss;
	ss << "SettingsEP from: " << request.address().host();
	LOGI(ss.str());
	writer.send(Http::Code::Ok, Settings::GetInstance()->ToJSON());
}
