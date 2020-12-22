#include <iostream>
#include <ostream>
#include "InstallationDesc.h"
#include "cxxopts.hpp"

using namespace std;

int exec(int argc, char *argv[])
{
	cxxopts::Options options("CreatePkg", "Create a package for HydroGarden");
	options.add_options()
        ("v,version", "Package Version", cxxopts::value<std::string>())
        ("n,name", "Package Name", cxxopts::value<std::string>())
		("d,desc", "Package Description", cxxopts::value<std::string>())
	    ("h,help", "Print usage")
		;

	auto result = options.parse(argc, argv);
	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		exit(0);
	}
	
	std::string out = "package.json";
	
	std::string pkgName;
	if (result.count("name"))
	{
		pkgName = result["name"].as<std::string>();
	}
	else
	{
		std::cout << "--name|-n is required" << std::endl;
		std::cout << options.help() << std::endl;
		exit(-1);
	}
	
	std::string desc;
	if (result.count("desc"))
	{
		desc = result["desc"].as<std::string>();
	}
	
	sVersion ver;
	if (result.count("version"))
	{
		std::string sVer = result["version"].as<std::string>();
		ver = sVersion::FromString(sVer);
	}
	InstallationDesc id;
	id.SetDescription(desc);
	id.SetName(pkgName);
	id.SetVersion(ver);
	
	try
	{
		std::ofstream fout(out);
		fout << id.ToJson() << std::endl;
		fout.close();
		return 0;
	}
	catch (const std::exception&)
	{
		return -1;
	}
	
}

int main(int argc, char *argv[])
{
	return exec(argc, argv);
}