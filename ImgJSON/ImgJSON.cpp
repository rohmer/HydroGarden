#include "ImgJSON.h"

Image ImgJSON::ImageFromFile(std::filesystem::path file)
{
	std::vector<unsigned char> imageData;
	if (!std::filesystem::exists(file))
		return Image();
	unsigned width, height;
	unsigned error = lodepng::decode(imageData, width, height, file.c_str());
	if (error)
		return Image();
	return (Image(file.filename(), imageData, width, height));
}

Image ImgJSON::ImageFromJSON(nlohmann::json json)
{
	return Image::FromJSON(json);
}

nlohmann::json ImgJSON::JsonFromFile(std::filesystem::path file)
{
	return ImageFromFile(file).ToJSON();
}