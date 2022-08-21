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
        gpu = json.value("gpu", "");
        if (json.contains("window_size"))
        {
            windowed_width = json["window_size"].value("width", 1600);
            windowed_height = json["window_size"].value("height", 900);
        }
        else
        {
            windowed_width = json.value("width", 1600);
            windowed_height = json.value("height", 900);
        }
        if (json.contains("fullscreen_resolution"))
        {
            fullscreen_width = json["fullscreen_resolution"].value("width", 1920);
            fullscreen_height = json["fullscreen_resolution"].value("height", 1080);
        }
        else
        {
            fullscreen_width = json.value("width", 1920);
            fullscreen_height = json.value("height", 1080);
        }
        refresh_rate_numerator = json.value("refresh_rate_numerator", 0);
        refresh_rate_denominator = json.value("refresh_rate_denominator", 0);
        windowed = json.value("windowed", true);
        vsync = json.value("vsync", false);
        dgpu_trick = json.value("dgpu_trick", false);
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
        gpu = json.value("gpu", "");
        if (json.contains("window_size"))
        {
            windowed_width = json["window_size"].value("width", 1600);
            windowed_height = json["window_size"].value("height", 900);
        }
        else
        {
            windowed_width = json.value("width", 1600);
            windowed_height = json.value("height", 900);
        }
        if (json.contains("fullscreen_resolution"))
        {
            fullscreen_width = json["fullscreen_resolution"].value("width", 1920);
            fullscreen_height = json["fullscreen_resolution"].value("height", 1080);
        }
        else
        {
            fullscreen_width = json.value("width", 1920);
            fullscreen_height = json.value("height", 1080);
        }
        refresh_rate_numerator = json.value("refresh_rate_numerator", 0);
        refresh_rate_denominator = json.value("refresh_rate_denominator", 0);
        windowed = json.value("windowed", true);
        vsync = json.value("vsync", false);
        dgpu_trick = json.value("dgpu_trick", false);
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
