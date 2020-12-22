#include <filesystem>
#include <iostream>
#include <sstream>
#include "InstallationDesc.h"
#include "sFile.h"
#include "cxxopts.hpp"
using namespace std;

int exec(int argc, char *argv[])
{
	cxxopts::Options options("CreatePkg", "Create a package for HydroGarden");
	options.add_options()
        ("v,version", "File Version", cxxopts::value<std::string>())
        ("n,name", "File Name", cxxopts::value<std::string>())
		("d,dest","Directory file will be installed to", cxxopts::value<std::string>())
        ("h,help", "Print usage")
		;

	auto result = options.parse(argc, argv);
	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		exit(0);
	}
	
	std::string out = "package.json";
	InstallationDesc id;
	try
	{
		std::ifstream infile(out);
		nlohmann::json j;
		infile >> j;
		infile.close();
		id = InstallationDesc::FromJson(j);
	}
	catch (std::exception &e)
	{
		std::cerr << "Error reading file (" << out << "), exception: " << e.what() << std::endl;
		return -1;
	}
	std::string dest;
	
	if (result.count("dest"))
	{
		dest = result["dest"].as<std::string>();
	}
	else
	{
		std::cerr << "-d|--dest is required" << std::endl;
		return -1;
	}
	
	sVersion ver;
	if (result.count("version"))
	{
		std::string sVer = result["version"].as<std::string>();
		ver = sVersion::FromString(sVer);
	}
	std::string file;
	if (result.count("name"))
	{
		file = result["name"].as<std::string>();
	}
	else
	{
		std::cerr << "-n|--name is required" << std::endl;
		return -1;
	}
	std::filesystem::path fp(file);
	
	if (!std::filesystem::exists(file))
	{
		std::cerr << "File: " << file << " does not exist or is not writable" << std::endl;
		return -1;
	}
	
	sFile f(fp.filename(), fp.parent_path().string(), dest , ver);
	id.AddFile(f);
	try
	{
		std::ofstream fout(out);
		fout << id.ToJson() << std::endl;
		fout.close();
		cout << "Added file: " << f.sourceLocation << "/" << f.fileName << " as: " << f.destLocation <<"/"<<f.fileName << std::endl;
		return 0;
	}
	catch (const std::exception&)
	{
		return -1;
	}
	
}

int main(int argc, char *argv[])
{
	return exec(argc,argv);
}