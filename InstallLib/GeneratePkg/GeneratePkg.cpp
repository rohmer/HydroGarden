#include <iostream>
#include <ostream>
#include "cxxopts.hpp"
#include "InstallationDesc.h"
#include "Package.h"

using namespace std;

int exec(int argc, char *argv[])
{
	cxxopts::Options options("CreatePkg", "Create a package for HydroGarden");
	options.add_options()
        ("d,desc", "Package Description", cxxopts::value<std::string>())
	    ("h,help", "Print usage")
		;

	auto result = options.parse(argc, argv);
	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		exit(0);
	}
	
	if (!result.count("desc"))
	{
		std:cerr << "-d|--desc required" << std::endl;
		return -1;
	}
	
	sPackageStatus stat = Package::CreatePackage(result["desc"].as<std::string>());
	for (int i = 0; i < stat.msgs.size(); i++)
	{
		if (stat.msgs[i].success)
			std::cout << "GOOD	";
		else
			std::cout << "FAIL	";
		std::cout << stat.msgs[i].msg << std::endl;
	}	
	if (stat.success) 
	{
		std::cout << "Package creation successful" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "FAILED to create package" << std::endl;
		return -1;
	}
}

int main(int argc, char *argv[])
{
 	Package::InstallPackage("HydroGarden");
	return exec(argc, argv);
}