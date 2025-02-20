#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <cstdlib>

#include "config.h"
#include "../../../external/json/json.hpp"

namespace internal_config
{
    class ConfigManager
    {
    private:

        static std::filesystem::path GetConfigFolder()
        {
            char* userProfile = nullptr;
            size_t len = 0;
            errno_t err = _dupenv_s(&userProfile, &len, "USERPROFILE");

            std::filesystem::path folder;
            if (err != 0 || userProfile == nullptr || len == 0)
            {
                folder = ".templeware";
            }
            else
            {
                folder = userProfile;
                free(userProfile);
                folder /= ".templeware";
            }
            folder /= "internal";

            std::error_code ec;
            std::filesystem::create_directories(folder, ec);

            return folder;
        }

        static std::filesystem::path GetConfigPath(const std::string& configName)
        {
            auto folder = GetConfigFolder();
            return folder / (configName + ".json");
        }

    public:

        static std::vector<std::string> ListConfigs()
        {
            std::vector<std::string> list;
            auto folder = GetConfigFolder();
            if (!std::filesystem::exists(folder))
                return list;

            for (const auto& entry : std::filesystem::directory_iterator(folder))
            {
                if (entry.is_regular_file())
                {
                    auto path = entry.path();
                    if (path.extension() == ".json")
                    {
                        list.push_back(path.stem().string());
                    }
                }
            }
            return list;
        }

        static void Save(const std::string& configName)
        {
            nlohmann::json j;
            j["esp"] = Config::esp;
            j["showHealth"] = Config::showHealth;
            j["teamCheck"] = Config::teamCheck;
            j["espFill"] = Config::espFill;
            j["espThickness"] = Config::espThickness;
            j["espFillOpacity"] = Config::espFillOpacity;
            j["espColor"] = {
                Config::espColor.x,
                Config::espColor.y,
                Config::espColor.z,
                Config::espColor.w
            };

            auto filePath = GetConfigPath(configName);
            std::ofstream ofs(filePath);
            if (ofs.is_open())
            {
                ofs << j.dump(4);
                ofs.close();
            }
        }

        static void Load(const std::string& configName)
        {
            auto filePath = GetConfigPath(configName);
            if (!std::filesystem::exists(filePath))
                return;

            std::ifstream ifs(filePath);
            if (!ifs.is_open())
                return;

            nlohmann::json j;
            ifs >> j;

            Config::esp = j.value("esp", false);
            Config::showHealth = j.value("showHealth", false);
            Config::teamCheck = j.value("teamCheck", false);
            Config::espFill = j.value("espFill", false);
            Config::espThickness = j.value("espThickness", 1.0f);
            Config::espFillOpacity = j.value("espFillOpacity", 0.5f);

            if (j.contains("espColor") && j["espColor"].is_array() && j["espColor"].size() == 4)
            {
                auto arr = j["espColor"];
                Config::espColor.x = arr[0].get<float>();
                Config::espColor.y = arr[1].get<float>();
                Config::espColor.z = arr[2].get<float>();
                Config::espColor.w = arr[3].get<float>();
            }

            ifs.close();
        }

        static void Remove(const std::string& configName)
        {
            auto filePath = GetConfigPath(configName);
            if (std::filesystem::exists(filePath))
            {
                std::error_code ec;
                std::filesystem::remove(filePath, ec);
            }
        }
    };
}