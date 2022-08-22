#include "Core/InitializeConfigure.hpp"
#include <fstream>
#include <filesystem>
#include "nlohmann/json.hpp"
#include "utility/encoding.hpp"

namespace Core
{


    void InitializeConfigure::reset()
    {
        gpu.clear();
        windowed_width = 1600;
        windowed_height = 900;
        fullscreen_width = 1920;
        fullscreen_height = 1080;
        refresh_rate_numerator = 0;
        refresh_rate_denominator = 0;
        windowed = true;
        vsync = false;
        dgpu_trick = false;
    }
    bool InitializeConfigure::load(std::string_view const& source) noexcept
    {
        nlohmann::json json = nlohmann::json::parse(source);
        gpu = json["gpu"].is_string() ? json["gpu"].get<std::string>() : "";
        if (json["window_size"].is_object())
        {
            windowed_width = json["window_size"]["width"].is_number() ? json["window_size"]["width"].get<int>() : 1600;
            windowed_height = json["window_size"]["height"].is_number() ? json["window_size"]["height"].get<int>() : 900;
        }
        else
        {
            windowed_width = json["width"].is_number() ? json["width"].get<int>() : 1600;
            windowed_height = json["height"].is_number() ? json["height"].get<int>() : 900;
        }
        if (json["fullscreen_resolution"].is_object())
        {
            fullscreen_width = json["fullscreen_resolution"]["width"].is_number() ? json["fullscreen_resolution"]["width"].get<int>() : 1920;
            fullscreen_height = json["fullscreen_resolution"]["height"].is_number() ? json["fullscreen_resolution"]["height"].get<int>() : 1080;
        }
        else
        {
            fullscreen_width = json["width"].is_number() ? json["width"].get<int>() : 1920;
            fullscreen_height = json["height"].is_number() ? json["height"].get<int>() : 1080;
        }
        refresh_rate_numerator = json["refresh_rate_numerator"].is_number() ? json["refresh_rate_numerator"].get<int>() : 0;
        refresh_rate_denominator = json["refresh_rate_denominator"].is_number() ? json["refresh_rate_denominator"].get<int>() : 0;
        windowed = json["windowed"].is_boolean() ? json["windowed"].get<bool>() : true;
        vsync = json["vsync"].is_boolean() ? json["vsync"].get<bool>() : false;
        dgpu_trick = json["dgpu_trick"].is_boolean() ? json["dgpu_trick"].get<bool>() : false;
        return true;
    }
    bool InitializeConfigure::save(std::string& buffer)
    {
        try
        {
            nlohmann::json json;
            json["gpu"] = gpu;
            json["window_size"]["width"] = windowed_width;
            json["window_size"]["height"] = windowed_height;
            json["fullscreen_resolution"]["width"] = fullscreen_width;
            json["fullscreen_resolution"]["height"] = fullscreen_height;
            json["refresh_rate_numerator"] = refresh_rate_numerator;
            json["refresh_rate_denominator"] = refresh_rate_denominator;
            json["windowed"] = windowed;
            json["vsync"] = vsync;
            json["dgpu_trick"] = dgpu_trick;
            buffer = std::move(json.dump());
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    bool InitializeConfigure::loadFromFile(std::string_view const& path) noexcept
    {
        std::wstring wpath(utility::encoding::to_wide(path));
        if (!std::filesystem::is_regular_file(wpath))
        {
            return false;
        }
        std::ifstream file(wpath, std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            return false;
        }
        nlohmann::json json;
        file >> json;
        gpu = json["gpu"].is_string() ? json["gpu"].get<std::string>() : "";
        if (json["window_size"].is_object())
        {
            windowed_width = json["window_size"]["width"].is_number() ? json["window_size"]["width"].get<int>() : 1600;
            windowed_height = json["window_size"]["height"].is_number() ? json["window_size"]["height"].get<int>() : 900;
        }
        else
        {
            windowed_width = json["width"].is_number() ? json["width"].get<int>() : 1600;
            windowed_height = json["height"].is_number() ? json["height"].get<int>() : 900;
        }
        if (json["fullscreen_resolution"].is_object())
        {
            fullscreen_width = json["fullscreen_resolution"]["width"].is_number() ? json["fullscreen_resolution"]["width"].get<int>() : 1920;
            fullscreen_height = json["fullscreen_resolution"]["height"].is_number() ? json["fullscreen_resolution"]["height"].get<int>() : 1080;
        }
        else
        {
            fullscreen_width = json["width"].is_number() ? json["width"].get<int>() : 1920;
            fullscreen_height = json["height"].is_number() ? json["height"].get<int>() : 1080;
        }
        refresh_rate_numerator = json["refresh_rate_numerator"].is_number() ? json["refresh_rate_numerator"].get<int>() : 0;
        refresh_rate_denominator = json["refresh_rate_denominator"].is_number() ? json["refresh_rate_denominator"].get<int>() : 0;
        windowed = json["windowed"].is_boolean() ? json["windowed"].get<bool>() : true;
        vsync = json["vsync"].is_boolean() ? json["vsync"].get<bool>() : false;
        dgpu_trick = json["dgpu_trick"].is_boolean() ? json["dgpu_trick"].get<bool>() : false;
        return true;
    }
    bool InitializeConfigure::saveToFile(std::string_view const& path) noexcept
    {
        try
        {
            nlohmann::json json;
            json["gpu"] = gpu;
            json["window_size"]["width"] = windowed_width;
            json["window_size"]["height"] = windowed_height;
            json["fullscreen_resolution"]["width"] = fullscreen_width;
            json["fullscreen_resolution"]["height"] = fullscreen_height;
            json["refresh_rate_numerator"] = refresh_rate_numerator;
            json["refresh_rate_denominator"] = refresh_rate_denominator;
            json["windowed"] = windowed;
            json["vsync"] = vsync;
            json["dgpu_trick"] = dgpu_trick;
            std::wstring wpath(utility::encoding::to_wide(path));
            std::ofstream file(wpath, std::ios::out | std::ios::binary | std::ios::trunc);
            if (!file.is_open())
            {
                return false;
            }
            file << json;
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
}
