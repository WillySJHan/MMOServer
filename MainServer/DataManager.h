#pragma once
#include <nlohmann/json.hpp>
#include "ConfigManager.h"
#include <fstream>
#include "Data.h"




template <typename T, typename U>
class ILoader {
public:
    virtual std::unordered_map<T, U> MakeMap() = 0;

    virtual std::vector<U> getData() = 0;
    virtual void setData(const nlohmann::json& jsonData) = 0;
    
};


class DataManager
{
public:
    static std::unordered_map<int, Proto::StatInfo> StatMap;
    static std::unordered_map<int, class Skill> SkillMap;

    static void LoadData();

    template <typename Loader, typename T, typename U>
    static Loader LoadJson(const std::string& path) {
        std::string a = ConfigManager::Config.dataPath + "/" + path + ".json";
        std::ifstream file(a);
        nlohmann::json jsonData;
        Loader loader;
        if (file.is_open())
        {
            try {
                file >> jsonData;
            }
            catch (const nlohmann::json::parse_error& e) {
                std::cerr << "JSON 파일을 파싱하는 중 오류가 발생했습니다: " << e.what() << std::endl;
            }


           

            loader.setData(jsonData);

            
        }
        file.close();
        return loader;
    }
};

class StatData :public ILoader<int, Proto::StatInfo>
{
public:
    virtual std::unordered_map<int, Proto::StatInfo> MakeMap()
    {
        std::unordered_map<int, Proto::StatInfo> map;
        for (Proto::StatInfo& stat : _stats) {
            stat.set_hp(stat.maxhp());
            map.emplace(stat.level(), stat);
        }
        return map;
    }

    virtual std::vector<Proto::StatInfo> getData() { return _stats; }
    virtual void setData(const nlohmann::json& jsonData);

public:
    std::vector<Proto::StatInfo> _stats;

};

class SkillData : ILoader<int, Skill>
{
public:
    std::unordered_map<int, Skill> MakeMap()
    {
        std::unordered_map<int, Skill> dict;
        for (const Skill& skill : _skills) {
            dict.emplace(skill.id, skill);
        }
        return dict;
    }

    virtual std::vector<Skill> getData() { return _skills; }
    virtual void setData(const nlohmann::json& jsonData);
public:
    std::vector<Skill> _skills;
};