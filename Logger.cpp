#include "Logger.h"

Logger *Logger::instance = nullptr;

Logger::Logger()
{	
}

Logger *Logger::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new Logger();
	}
	return instance;
}

void Logger::SetLogLevel(eLogLevel level) 
{	
	minLogLevel = level;
}

void Logger::SetLogQueueSize(uint sz)
{
	queueSize = sz;
	while (logMessages.size() > queueSize)
		logMessages.pop_back();
}

void Logger::Log(eLogLevel level, std::string msg)
{
	if (level < minLogLevel)
		return;
	sLogMsg logMsg;
	const auto now = std::chrono::system_clock::now();
	const auto epoch   = now.time_since_epoch();
	const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);
	logMsg.logTime = seconds.count();
	logMsg.level = level;
	logMsg.msg = msg;
	logMessages.push_front(logMsg);
	while (logMessages.size() > queueSize)
		logMessages.pop_back();

}

uint Logger::GetLogSize()
{
	return queueSize;
}
	
std::deque<sLogMsg> Logger::GetLogMessages()
{
	return logMessages;	
}

std::string Logger::ToJSON()
{
	nlohmann::json json=ToJSONObj();	
	std::stringstream ss;
	ss << json;
	return ss.str();
}

nlohmann::json Logger::ToJSONObj()
{
	nlohmann::json json;
	for (int i = 0; i < logMessages.size(); i++)
	{
		nlohmann::json logMsg;
		logMsg["time"] = logMessages[i].logTime;
		logMsg["msg"] = logMessages[i].msg;
		logMsg["sev"] = logMessages[i].level;
		json[i] = logMsg;
	}
	
	return json;
}

void Logger::ClearLog()
{
	logMessages.clear();
}