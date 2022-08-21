#pragma once
#include <string>
#include <string_view>

namespace Core
{
    struct InitializeConfigure
    {
        std::string gpu;
        int windowed_width = 1600;
        int windowed_height = 900;
        int fullscreen_width = 1920;
        int fullscreen_height = 1080;
        int refresh_rate_numerator = 0;
        int refresh_rate_denominator = 0;
        bool windowed = true;
        bool vsync = false;
        bool dgpu_trick = false;
        
        void reset();
        bool load(std::string_view const& source) noexcept;
        bool save(std::string& buffer);
        bool loadFromFile(std::string_view const& path) noexcept;
        bool saveToFile(std::string_view const& path) noexcept;
    };
}
