#pragma once
#include "VersionNo.h"
#define VERSION_RELEASE VERSION_BUILDNO
#define VERSION_BUILD VERSION_EXTEND

#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <json.hpp>

struct Version
{
public:
	Version() :
		major(VERSION_MAJOR),
		minor(VERSION_MINOR),
		build(VERSION_BUILD),
		extended(VERSION_EXTEND)
	{
	}
	
	Version(int Major, int Minor, int Build, int Extended) :
		major(Major),
		minor(Minor),
		build(Build),
		extended(Extended)
	{
	}
	
	static Version FromJSON(nlohmann::json json)
	{
		int major, minor, build, ext;
		if (json.contains("version"))
		{
			if (json["version"].contains("major"))
				major = json["version"]["major"];
			if (json["version"].contains("minor"))
				minor = json["version"]["minor"];
			if (json["version"].contains("build"))
				build = json["version"]["build"];
			if (json["version"].contains("ext"))
				ext = json["version"]["ext"];
		}
		return Version(major, minor, build, ext);
	}
	
	nlohmann::json ToJSON()
	{
		nlohmann::json json;
		json["version"]["major"] = major;
		json["version"]["minor"] = minor;
		json["version"]["build"] = build;
		json["version"]["ext"] = extended;
		return json;
	}
	
	std::string ToString()
	{
		std::stringstream ss;
		ss << major << "." << minor << "." << build << "." << extended;
		return ss.str();
	}	
	
	static Version FromString(std::string verStr)
	{
		std::stringstream ss(verStr);
		std::vector<std::string> results;
		std::string token;

		while (std::getline(ss, token, '.'))
		{
			results.push_back(token);
		}
		int i = 0;
		int major = 0, minor = 0, build = 0, ext = 0;
		while (i < results.size() && i < 4)
		{
			int val;
			try
			{
				val = std::atoi(results[i].c_str());
			}
			catch (const std::exception&)
			{
			}
			switch (i)
			{
			case 0:
				major = val;
				break;
			case 1:
				minor = val;
				break;
			case 2:
				build = val;
				break;
			default:
				ext = val;
				break;				
			}
		}
		return Version(major, minor, build, ext);
	}

private:	
	int major, minor, build, extended;
};