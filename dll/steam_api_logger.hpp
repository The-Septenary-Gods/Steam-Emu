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
extern HANDLE hConsole;
extern CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
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

void fatal_error(const std::wstring& error_msg_en, const std::wstring& error_msg_zh);

void logger(const std::string& msg, const std::string& type = "Display");

void logger(const std::wstring& msg, const std::wstring& type = L"Display");

void warning_logger(const std::wstring& msg);

void error_logger(const std::wstring& msg);
