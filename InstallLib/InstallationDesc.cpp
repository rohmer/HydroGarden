#include "InstallationDesc.h"

InstallationDesc::InstallationDesc()
{
}

void InstallationDesc::SetDescription(std::string desc)
{
	packageDesc = desc;
}

void InstallationDesc::AddFile(sFile file)
{
	bool dupe = false;
	for (int i = 0; i < files.size(); i++)
	{
		if (files[i].sourceLocation == file.sourceLocation && files[i].fileName == file.fileName)
			dupe = true;
	}
	if (!dupe)
		files.push_back(file);
}

void InstallationDesc::SetName(std::string name)
{
	packageName = name;
}

std::vector<sFile> InstallationDesc::GetFiles()
{
	return files;
}

nlohmann::json InstallationDesc::ToJson()
{
	nlohmann::json j;
	j["version"] = packageVersion.ToJson();
	j["desc"] = packageDesc;
	j["name"] = packageName;
	nlohmann::json fj;
	for (int i = 0; i < files.size(); i++)
	{
		std::stringstream ss;
		ss << i;
		fj[ss.str()] = files[i].ToJson();
	}
	j["files"] = fj;
	return j;
}

InstallationDesc InstallationDesc::FromJson(nlohmann::json j)
{
	std::string desc;
	sVersion ver;
	InstallationDesc id;
	if (j.contains("version"))
		id.SetVersion(sVersion::FromJson(j["version"]));
	if (j.contains("desc"))
		id.SetDescription(j["desc"]);
	if (j.contains("name"))
		id.SetName(j["name"]);
	if (j.contains("files"))
	{
		nlohmann::json fj = j["files"];
		int i = 0;
		std::stringstream ss;
		ss << i;
		while (fj.contains(ss.str()))
		{
			id.AddFile(sFile::FromJSON(fj[ss.str()]));
			ss.str("");
			i++;
			ss << i;
		}
	}
	if (j.contains("beforeInstall"))
	{
		
		nlohmann::json bi = j["beforeInstall"];
		int i = 0;
		std::stringstream ss;
		ss << i;
		while (bi.contains(ss.str()))
		{
			id.AddBeforeInstallCmd(bi[ss.str()]);
			ss.str("");
			i++;
			ss << i;
		}
	}
	if (j.contains("afterInstall"))
	{
		
		nlohmann::json bi = j["afterInstall"];
		int i = 0;
		std::stringstream ss;
		ss << i;
		while (bi.contains(ss.str()))
		{
			id.AddAfterInstallCmd(bi[ss.str()]);
			ss.str("");
			i++;
			ss << i;
		}
	}
	return id;
}