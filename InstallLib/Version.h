#pragma once
#include <sstream>
#include <string>
#include <json.hpp>

struct sVersion
{
private:
	unsigned int major, minor, buildNum;
	
public:
	sVersion()
		: major(0)
		, minor(0)
		, buildNum(0)
	{
	}	
	sVersion(int major, int minor, int buildNum)
		: major(major)
		, minor(minor)
		, buildNum(buildNum)
	{
	}
	
	bool operator>(const sVersion& other)
	{
		if (major > other.major)
			return true;
		if (major == other.major && minor > other.minor)
			return true;
		if (major == other.major&&minor == other.minor&&buildNum > other.buildNum)
			return true;
		return false;
	}
	
	bool operator<(const sVersion& other)
	{
		if (major < other.major)
			return true;
		if (major == other.major && minor < other.minor)
			return true;
		if (major == other.major&&minor == other.minor&&buildNum < other.buildNum)
			return true;
		return false;
	}
	
	bool operator==(const sVersion& other)
	{
		if (major == other.major&&minor == other.minor&&buildNum == other.buildNum)
			return true;
		return false;
	}
	
	std::string ToString()
	{
		std::stringstream ss;
		ss << major << "." << minor << "." << buildNum;
		return ss.str();
	}
	
	static sVersion FromString(std::string s)
	{
		unsigned int major = 0, minor = 0, buildNum = 0;
		int i = 0, pos = 0;
		std::string token;
		try
		{
			int fields[3];
			std::istringstream ss(s);
			std::string token;
			while (std::getline(ss, token, '.'))
			{
				fields[i] = std::atoi(token.c_str());
				i++;
			}
			if (i > 0)
				major = fields[0];
			if (i > 1)
				minor = fields[1];
			if (i > 2)
				buildNum = fields[2];
		}
		catch (std::exception &e)
		{
			return (sVersion());
		}
		return sVersion(major, minor, buildNum);
	}
	
	nlohmann::json ToJson()
	{
		nlohmann::json vj;
		vj["major"] = major;
		vj["minor"] = minor;
		vj["buildNum"] = buildNum;
		return vj;
	}
	
	static sVersion FromJson(nlohmann::json vj)
	{
		unsigned int major = 0, minor = 0, buildNum = 0;
		if (vj.contains("major"))
			major = vj["major"];
		if (vj.contains("minor"))
			minor = vj["minor"];
		if (vj.contains("buildNum"))
			buildNum = vj["buildNum"];
		return sVersion(major, minor, buildNum);		
	}
};