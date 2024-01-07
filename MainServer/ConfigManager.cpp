#include "stdafx.h"
#include "ConfigManager.h"
#include <fstream>

ServerConfig ConfigManager::Config;



void ConfigManager::LoadConfig()
{
    std::ifstream configFile("Config/config.json");
    if (configFile.is_open()) {
        nlohmann::json jsonConfig;
        configFile >> jsonConfig;

        if (jsonConfig.find("dataPath") != jsonConfig.end()) {
            Config.dataPath = jsonConfig["dataPath"];
        }
        else {

            std::cout << "dataPath field doesn't exist in the config." << std::endl;
        }


    }
    else {

        std::cout << "Unable to open the config file." << std::endl;
    }
}
