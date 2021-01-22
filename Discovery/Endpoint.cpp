#include "Endpoint.h"

Endpoint::Endpoint()
{
}

Endpoint::Endpoint(
	std::string path, 
	eEPMethod method)
	: endpointPath(path)
	, endpointMethod(method)
{
}

std::string Endpoint::ToJSON()
{
	nlohmann::json json;
	
	json["endpoint"]["path"] = endpointPath.c_str();
	json["endpoint"]["method"] = endpointMethod;
	return json.dump();
}

Endpoint Endpoint::FromJSON(std::string jStr)
{
	Endpoint ep;
	nlohmann::json json = nlohmann::json::parse(jStr);
	if (!json.is_object() || json.empty())
		return ep;
	if (json.contains("endpoint"))
	{
		if (json["endpoint"].contains("path"))
			ep.endpointPath = json["endpoint"]["path"];
		if (json["endpoint"].contains("method"))
		{
			ep.endpointMethod = json["endpoint"]["method"];
		}	
			
	}
	return ep;
}