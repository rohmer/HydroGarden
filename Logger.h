#pragma once

#include <chrono>
#include <deque>
#include <vector>
#include <string>
#include <sstream>

#include <json.hpp>
#include <lvgl.h>


#define LOGI(msg) (Logger::GetInstance()->Log(eLogLevel::INFO, msg))
#define LOGW(msg) (Logger::GetInstance()->Log(eLogLevel::WARN, msg))
#define LOGE(msg) (Logger::GetInstance()->Log(eLogLevel::ERROR, msg))

enum eLogLevel
{
	INFO,
	WARN,
	ERROR
};

struct sLogMsg
{
	time_t logTime;
	std::string msg;
	eLogLevel level;
};


class Logger
{
private:
	Logger();
	static Logger *instance;
	static lv_log_print_g_cb_t log_cb(lv_log_level_t level, const char * file, int line, const char * fn_name, const char * dsc);
	
	uint queueSize = 50;
	eLogLevel minLogLevel = eLogLevel::INFO;
	
	std::deque<sLogMsg> logMessages;
	
	
public:
	static Logger *GetInstance();
	void Log(eLogLevel level, std::string message);
	void SetLogLevel(eLogLevel level);
	void SetLogQueueSize(uint size);
	uint GetLogSize();
	
	std::deque<sLogMsg> GetLogMessages();
	std::string ToJSON();
	nlohmann::json ToJSONObj();
	
	void ClearLog();
};