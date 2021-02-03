#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <json.hpp>
#include <base64.h>

struct Image
{
public:
	Image() :
		width(0),
		height(0)
	{
	}
		
	Image(std::string imageName, std::vector<unsigned char> png, unsigned width, unsigned height);
	nlohmann::json ToJSON();
	static Image FromJSON(nlohmann::json j);
	unsigned Width() { return width; }
	unsigned Height() { return height; }
	std::string Name()
	{
		return imageName;
	} 
	std::vector<unsigned char> PNGData()
	{
		return png;
	}
		
private:
	std::string imageName;
	std::vector<unsigned char> png;
	unsigned width=0, height=0;
};