#include "stdafx.h"
#include "DataManager.h"
#include "Data.h"
#include <locale>



std::unordered_map<int, Proto::StatInfo> DataManager::StatMap;
std::unordered_map<int, Skill> DataManager::SkillMap;

void DataManager::LoadData()
{
    SkillMap = LoadJson<SkillData, int, Skill>("SkillData").MakeMap();
	StatMap = LoadJson<StatData, int, Proto::StatInfo>("StatData").MakeMap();
    
}

void StatData::setData(const nlohmann::json& jsonData)
{
    if (jsonData.find("stats") != jsonData.end() && jsonData["stats"].is_array())
    {
        for (const auto& stat : jsonData["stats"])
        {
            Proto::StatInfo s;
            s.set_level(std::stoi(stat["level"].get<std::string>()));
            s.set_maxhp(std::stoi(stat["maxHp"].get<std::string>()));
            s.set_attack( std::stoi(stat["attack"].get<std::string>()));
            s.set_speed( std::stof(stat["speed"].get<std::string>()));
            s.set_totalexp( std::stoi(stat["totalExp"].get<std::string>()));
            s.set_hp(s.maxhp());
            _stats.push_back(s);
        }
    }
}

void SkillData::setData(const nlohmann::json& jsonData)
{
    for (const auto& skillJson : jsonData["skills"])
    {
        Skill skill;
        skill.id = std::stoi(skillJson["id"].get<std::string>());
        skill.name = skillJson["name"].get<std::string>();
        
        skill.cooldown = std::stof(skillJson["cooldown"].get<std::string>());
        skill.damage = std::stoi(skillJson["damage"].get<std::string>());

        if (skillJson["skillType"] == "SkillAuto")
        {
            skill.skillType = Proto::SkillType::SKILL_AUTO;
        }
        else if (skillJson["skillType"] == "SkillProjectile")
        {
            skill.skillType = Proto::SkillType::SKILL_PROJECTILE;
            skill.projectile.name = skillJson["projectile"]["name"].get<std::string>();
            skill.projectile.speed = std::stof(skillJson["projectile"]["speed"].get<std::string>());
            skill.projectile.range = std::stoi(skillJson["projectile"]["range"].get<std::string>());
            skill.projectile.prefab = skillJson["projectile"]["prefab"].get<std::string>();
        }

        _skills.push_back(skill);
    }
}