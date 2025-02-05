/* Copyright (C) 2025 The Septenary Gods
   This file is part of the Steam Emu.
   Github: https://github.com/The-Septenary-Gods/Steam-Emu/

   The Steam Emu is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   The Steam Emu is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the Steam Emu; if not, see
   <http://www.gnu.org/licenses/>.  */

#pragma once
#include <string>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
const HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
#endif

const std::string LOGGING_PREFIX = 
#if defined(__x86_64__) || defined(_M_X64)
    "SteamAPI64: ";
#else
    "SteamAPI: ";
#endif
const std::wstring LOGGING_PREFIX_W = 
#if defined(__x86_64__) || defined(_M_X64)
    L"SteamAPI64: ";
#else
    L"SteamAPI: ";
#endif

inline void fatal_error(const std::wstring& error_msg_en, const std::wstring& error_msg_zh) {
    // Check system language
    bool is_zh = (GetSystemDefaultUILanguage() & 0xFF) == LANG_CHINESE;
    const std::wstring& msg = is_zh ? error_msg_zh : error_msg_en;
    const std::wstring& title = is_zh ? L"致命错误" : L"Fatal Error";
    const std::wstring& fatal_prefix = LOGGING_PREFIX_W + L"Fatal Error: ";

#if defined(_WIN32) || defined(_WIN64)
    // Print error message to console on Windows
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::wcerr << fatal_prefix << error_msg_en << std::endl;
    SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);

    // Show error message box on Windows
    MessageBoxW(NULL, msg.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
#else
    // Print error message to console on *nix
    std::wcerr << "\033[1;31m" << fatal_prefix << msg << "\033[0m" << std::endl;
#endif

    exit(1);
}

inline void logger(const std::string& msg, const std::string& type = "Display") {
    const std::string prefix = LOGGING_PREFIX + type + ": ";
    std::cout << prefix << msg << std::endl;
}

inline void logger(const std::wstring& msg, const std::wstring& type = L"Display") {
    const std::wstring prefix = LOGGING_PREFIX_W + type + L": ";
    std::wcout << prefix << msg << std::endl;
}

inline void warning_logger(const std::wstring& msg) {
    const std::wstring prefix = LOGGING_PREFIX_W + L"Warning: ";

#if defined(_WIN32) || defined(_WIN64)
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Yellow
    std::wcout << prefix << msg << std::endl;
    SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
#else
    std::wcout << prefix << msg << std::endl;
#endif
}

inline void error_logger(const std::wstring& msg) {
    const std::wstring prefix = LOGGING_PREFIX_W + L"Error: ";

#if defined(_WIN32) || defined(_WIN64)
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::wcout << prefix << msg << std::endl;
    SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);
#else
    std::wcout << prefix << msg << std::endl;
#endif
}
