#include "Info.h"

std::string Info::GetInfo()
{
	nlohmann::json json;
	
	time_t now = time(nullptr);
	json["time"] = ctime(&now);
	json["version"]["major"] = VERSION_MAJOR;
	json["version"]["minor"] = VERSION_MINOR;
	json["version"]["release"] = VERSION_RELEASE;
	json["version"]["build"] = VERSION_BUILD;
	
	Scheduler *sched = Scheduler::GetInstance();
	time_t npon = sched->GetNextPumpOn();
	time_t npoff = sched->GetNextPumpOff();
	json["status"]["pump"]["nextOn"] = ctime(&npon);
	json["status"]["pump"]["nextOff"] = ctime(&npoff);
	json["status"]["pump"]["state"] = sched->IsPumpOn(now);
	time_t nlon = sched->GetNextLightOn();
	time_t nloff = sched->GetNextLightOff();
	json["status"]["light"]["nextOn"] = ctime(&nlon);
	json["status"]["light"]["nextOff"] = ctime(&nloff);
	json["status"]["light"]["state"] = sched->IsLightOn(now);
	
	std::string s;
	if (Hardware::FeedLevel())
		s = "OK";
	else
		s = "LOW";
	json["status"]["feedLevel"] = s;
	if (Hardware::WaterLevel())
		s = "OK";
	else
		s = "LOW";
	json["status"]["waterLevel"] = s;
	time_t nfeed = sched->GetNextFeeding();
	time_t feedDur = sched->GetFeedDuration();
	json["status"]["feed"]["next"] = ctime(&nfeed);
	json["status"]["feed"]["feedDuration"] = feedDur;
	json["status"]["feed"]["state"] = sched->IsFeeding(now);
	
	json["logs"] = Logger::GetInstance()->ToJSONObj();
	
	std::stringstream ss;
	ss << json;
	return ss.str();
}