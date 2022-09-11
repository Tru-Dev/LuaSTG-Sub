﻿#include "Debugger/Logger.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/wincolor_sink.h"
#include "spdlog/sinks/msvc_sink.h"
#include "platform/KnownDirectory.hpp"
#include "platform/CommandLine.hpp"
#include "platform/HResultChecker.hpp"

namespace LuaSTG::Debugger
{
    inline std::wstring getLogFilePath()
    {
        std::wstring path = L"";
    #ifdef USING_SYSTEM_DIRECTORY
        if (platform::KnownDirectory::makeAppDataW(APP_COMPANY, APP_PRODUCT, path))
        {
            path.append(L"\\");
        }
    #endif
        path.append(L"engine.log");
        return std::move(path);
    }

    static bool enable_console = false;
    static bool open_console = false;
    static bool wait_console = false;

#if defined(USING_CONSOLE_OUTPUT) || defined(USE_CONSOLE_MAIN)
    static void openWin32Console();
    static void closeWin32Console();
#endif // defined(USING_CONSOLE_OUTPUT) || defined(USE_CONSOLE_MAIN)

    void Logger::create()
    {
    #if defined(USING_CONSOLE_OUTPUT) && !defined(USE_CONSOLE_MAIN)
        std::vector<std::string> args(platform::CommandLine::get());
        for (auto const& v : args)
        {
            if (v == "--log-window")
            {
                enable_console = true;
            }
            if (v == "--log-window-wait")
            {
                wait_console = true;
            }
        }
    #endif // defined(USING_CONSOLE_OUTPUT) && !defined(USE_CONSOLE_MAIN)
    #if defined(USING_CONSOLE_OUTPUT) || defined(USE_CONSOLE_MAIN)
        openWin32Console();
    #endif // defined(USING_CONSOLE_OUTPUT) || defined(USE_CONSOLE_MAIN)

        std::vector<spdlog::sink_ptr> sinks;

        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(getLogFilePath(), true);
        sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%L] %v");
        sinks.emplace_back(sink);

    #if 0
        auto sink_debugger = std::make_shared<spdlog::sinks::windebug_sink_mt>();
        sink_debugger->set_pattern("[%Y-%m-%d %H:%M:%S] [%L] %v");
        sinks.emplace_back(sink_debugger);
    #endif

    #if defined(USING_CONSOLE_OUTPUT) || defined(USE_CONSOLE_MAIN)
        if (open_console)
        {
            auto sink_console = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
            sink_console->set_pattern("%^[%Y-%m-%d %H:%M:%S] [%L]%$ %v");
            sinks.emplace_back(sink_console);
        }
    #endif // defined(USING_CONSOLE_OUTPUT) || defined(USE_CONSOLE_MAIN)

        auto logger = std::make_shared<spdlog::logger>("luastg", sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::trace);
        //logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%L] %v");
        //logger->set_pattern("%^[%Y-%m-%d %H:%M:%S] [%L]%$ %v");
        //logger->set_pattern("%^[%Y-%m-%d %H:%M:%S] [%L] [%n]%$ %v");
        logger->flush_on(spdlog::level::info);

        spdlog::set_default_logger(logger);

        struct HResultCheckerCallback
        {
            static void WriteError(std::string_view const message)
            {
                spdlog::error("[luastg] {}", message);
            }
        };

        platform::HResultChecker::SetPrintCallback(&HResultCheckerCallback::WriteError);
    }
    void Logger::destroy()
    {
        if (auto logger = spdlog::get("luastg"))
        {
            logger->flush();
        }
        
        platform::HResultChecker::SetPrintCallback();

        spdlog::drop_all();
        spdlog::shutdown();

    #if defined(USING_CONSOLE_OUTPUT) && !defined(USE_CONSOLE_MAIN)
        closeWin32Console();
    #endif // defined(USING_CONSOLE_OUTPUT) && !defined(USE_CONSOLE_MAIN)
    }
};

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace LuaSTG::Debugger
{
#if defined(USING_CONSOLE_OUTPUT) || defined(USE_CONSOLE_MAIN)
    void openWin32Console()
    {
#ifndef USE_CONSOLE_MAIN
        if (enable_console)
        {
            if (!AllocConsole())
                return;
            HWND window = GetConsoleWindow();
            HMENU menu = GetSystemMenu(window, FALSE);
            RemoveMenu(menu, SC_CLOSE, MF_BYCOMMAND);
            SetWindowTextW(window, L"" LUASTG_INFO);
            ShowWindow(window, SW_NORMAL);
#endif // !USE_CONSOLE_MAIN
            open_console = true;
            SetConsoleOutputCP(CP_UTF8);
#ifndef USE_CONSOLE_MAIN
        }
#endif // !USE_CONSOLE_MAIN
    }
    void closeWin32Console()
    {
#ifndef USE_CONSOLE_MAIN
        if (open_console)
        {
            if (wait_console)
            {
                std::wstring_view const exit_msg(L"按 ESC 关闭引擎日志窗口 | Press ESC to close the engine log window\n");
                WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), exit_msg.data(), (DWORD)exit_msg.length(), NULL, NULL);
                while ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0x8000)
                {
                    Sleep(1);
                }
            }
            FreeConsole();
        }
#endif // !USE_CONSOLE_MAIN
    }
#endif // defined(USING_CONSOLE_OUTPUT) || defined(USE_CONSOLE_MAIN)
}
