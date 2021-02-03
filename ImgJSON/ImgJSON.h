#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "lodepng.h"
#include <json.hpp>

#include "Image.h"

class ImgJSON
{
public:
	static Image ImageFromFile(std::filesystem::path file);
	static Image ImageFromJSON(nlohmann::json json);
	static nlohmann::json JsonFromFile(std::filesystem::path file);
};