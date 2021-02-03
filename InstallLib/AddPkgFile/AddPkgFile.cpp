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
		("w,wildcard", "Wildcard extension match (so for *.so for example)", cxxopts::value<std::string>())
		("s,sourcedir", "Source directory to use (If not set, current directory used)", cxxopts::value<std::string>())
		("d,dest", "Directory file will be installed to", cxxopts::value<std::string>())
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

	std::string source;
	if (result.count("source"))
	{
		source = result["source"].as<std::string>();
	}
	else
	{
		source = std::filesystem::current_path();
	}
		
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
	std::vector <std::string> files;
	if (result.count("wildcard"))
	{
		std::string wildcard = result["wildcard"].as<std::string>();
		if (wildcard.at(0) != '.')
		{
			std::stringstream ss;
			ss << "." << wildcard;
			wildcard = ss.str();
		}
		for (auto &p : std::filesystem::directory_iterator(source)) 
		{
			std::string ext = p.path().extension();
			if (ext == wildcard)
				files.push_back(p.path().string());
		}
	}
	
	else
	{
		if (result.count("name")) 
		{
			files.push_back(result["name"].as<std::string>());
		}
		else
		{
			std::cerr << "-n|--name or -w|--wildcard is required" << std::endl;
			return -1;
		}
	}
	
	bool error = false;
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); it++)
	{
		std::filesystem::path fp(it->c_str());
		if (!std::filesystem::exists(it->c_str()))
		{
			std::cerr << "File: " << fp << " does not exist or is not readable\n";
			error = true;
		}
		else
		{
			sFile f(fp.filename(), fp.parent_path().string(), dest, ver);
			id.AddFile(f);
			std::cout << "Adding file: " << fp << "\n";
		}
	}
	
	if (!error)
	{
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
	
	std::cerr << "Did not add any files, fix above errors\nExiting...\n\n";
}

int main(int argc, char *argv[])
{
	return exec(argc,argv);
}