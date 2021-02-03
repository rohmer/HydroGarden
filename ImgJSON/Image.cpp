#include "Image.h"

Image::Image(std::string imageName, std::vector<unsigned char> png, unsigned width, unsigned height)
	: width(width)
	, height(height)
	, png(png)
	, imageName(imageName)
{
}

nlohmann::json Image::ToJSON()
{
	nlohmann::json json;
	json["img"]["name"] = imageName;
	json["img"]["width"] = width;
	json["img"]["height"] = height;
	std::string encoded;
	std::stringstream ss;
	for (int i = 0; i < png.size(); i++)
		ss << png[i];
	encoded = base64_encode(ss.str());
	json["img"]["image"] = encoded;
	return json;
}	

Image Image::FromJSON(nlohmann::json json)
{
	unsigned width, height;
	std::string name;
	std::vector<unsigned char> imgData;
	if (json.contains("img"))
	{
		if (json["img"].contains("imgData"))
		{
			std::string iData = base64_decode(json["img"]["imgData"], true);
			for (int i = 0; i < iData.length(); i++)
				imgData.push_back(iData[i]);
		}
		if (json["img"].contains("width"))
			width = json["img"]["width"];
		if (json["img"].contains("height"))
			height = json["img"]["height"];
		if (json["img"].contains("name"))
			name = json["img"]["name"];
	}
	return (Image(name, imgData, width, height));
}