#pragma once
#include <nlohmann/json.hpp>



struct ServerConfig
{
	std::string dataPath;
};



class ConfigManager
{
public:
	static void LoadConfig();

public:
	static ServerConfig Config;

};

