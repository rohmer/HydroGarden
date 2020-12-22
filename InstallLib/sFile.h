#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <json.hpp>
#include <filesystem>
#include "picosha2.h"
#include "Version.h"

struct sFile
{		
public :
	sVersion fileVersion;
	std::string fileName;
	std::string sourceLocation; 		// Location in archive, including file name
	std::string destLocation; 		// Destination location of the file;
	std::string fileHash;
	
	sFile()
		: fileName("")
		, sourceLocation("")
		, destLocation("")
		, fileVersion(sVersion())
		, fileHash("")
	{
	}
	
	sFile(std::string fName, std::string source, std::string dest, sVersion fVersion, std::string hash="")	
		: fileName(fName)
		, sourceLocation(source)
		, destLocation(dest)
		, fileVersion(fVersion)
		, fileHash(hash)
	{
	}
	
	bool GenerateHash()
	{
		std::filesystem::path path;
		path = sourceLocation;
		path /= fileName;
		if (!std::filesystem::exists(path))
			return false;
		std::ifstream f(path, std::ios::binary);
		std::vector<unsigned char> s(picosha2::k_digest_size);
		picosha2::hash256(f, s.begin(), s.end());
		fileHash=picosha2::bytes_to_hex_string(s.begin(), s.end());
		f.close();
		return true;
	}
	
	static std::string GenerateHash(std::filesystem::path file)
	{
		if (!std::filesystem::exists(file))
			return "";
		std::ifstream f(file, std::ios::binary);
		std::vector<unsigned char> s(picosha2::k_digest_size);
		picosha2::hash256(f, s.begin(), s.end());
		std::string fileHash = picosha2::bytes_to_hex_string(s.begin(), s.end());
		f.close();
		
		return fileHash;
	}
	
	static sFile FromJSON(nlohmann::json json)
	{
		std::string name, source, dest, fileHash;
		sVersion version;
		if (json.contains("file"))
		{
			nlohmann::json fj = json["file"];
			name = fj["name"];
			source = fj["source"];
			dest = fj["dest"];
			version = sVersion::FromJson(fj["version"]);
			fileHash = fj["hash"];
			return sFile(name, source, dest, version, fileHash);
		}
		return sFile();
	}
	
	nlohmann::json ToJson()
	{
		nlohmann::json j;
		j["file"]["name"] = fileName;
		j["file"]["source"] = sourceLocation;
		j["file"]["dest"] = destLocation;
		j["file"]["version"] = fileVersion.ToJson();
		if (GenerateHash())
			j["file"]["hash"] = fileHash;
		return j;		
	}
};