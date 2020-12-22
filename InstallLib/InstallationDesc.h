#pragma once

#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "Version.h"
#include "sFile.h"

#include <json.hpp>

class InstallationDesc
{
public:
	InstallationDesc();
	void SetName(std::string name);
	std::string GetName() { return packageName; }
	std::string GetDescription() { return packageDesc; }
	sVersion GetVersion() { return packageVersion; }
	
	void SetVersion(sVersion v) { packageVersion = v; }
	void SetDescription(std::string desc);
	
	void AddFile(sFile file);
	std::vector<sFile> GetFiles();
	std::vector<std::string> GetBeforeInstallCmds() { return beforeInstallCmds;}
	void AddBeforeInstallCmd(std::string cmd) {
		beforeInstallCmds.push_back(cmd);
	}
	std::vector<std::string> GetAfterInstallCmds() { return afterInstallCmds;}
	void AddAfterInstallCmd(std::string cmd) {
		afterInstallCmds.push_back(cmd);
	}
	
	nlohmann::json ToJson();
	static InstallationDesc FromJson(nlohmann::json j);
	
private:
	std::vector<std::string> beforeInstallCmds;
	std::vector<std::string> afterInstallCmds;
	std::vector<sFile> files;
	std::string packageDesc;
	sVersion packageVersion;
	std::string packageName;
	
};