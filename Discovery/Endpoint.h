#pragma once

#include <string>
#include <json.hpp>
#include <pistache/description.h>
#include <pistache/endpoint.h>

class Endpoint
{
public:
	enum eEPMethod
	{
		GET,
		POST,
		BOTH		
	};
	NLOHMANN_JSON_SERIALIZE_ENUM( eEPMethod,
		{
			{ GET, "GET" },
			{ POST, "POST" },
			{ BOTH, "BOTH" }
		});
	
	Endpoint() ;
	
	Endpoint(std::string path,
		eEPMethod method);
	std::string ToJSON();
	static Endpoint FromJSON(std::string json);
	
	eEPMethod endpointMethod;
	std::string endpointPath;
};