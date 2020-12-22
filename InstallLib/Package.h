#pragma once

#include <filesystem>
#include <string>
#include <sstream>
#include <stdio.h>
#include <archive_entry.h>
#include <ostream>
#include <iostream>
#include <json.hpp>
#include "../External/libuuid/uuid.h"
#include <archive.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sFile.h"
#include "InstallationDesc.h"

struct sPackageMsg
{
public:
	sPackageMsg(bool successful, std::string message)
		: success(successful)
		, msg(message)
	{
	}
	bool success;
	std::string msg;
};

struct sPackageStatus
{
public:
	bool success;
	std::vector<sPackageMsg> msgs;
};

class Package
{
public:
	static sPackageStatus CreatePackage(std::string packageDescription);
	static sPackageStatus InstallPackage(std::filesystem::path installPackage);
	
private:
	static InstallationDesc readDesc(std::string packageDescription);
	static std::string genUUID();
	static int copy_data(struct archive *ar, struct archive *aw);
	
	static sPackageStatus uncompressFile(std::filesystem::path archive, std::filesystem::path dest, sPackageStatus status);
};