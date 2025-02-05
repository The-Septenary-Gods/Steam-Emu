/* Copyright (C) 2019 Mr Goldberg
   This file is part of the Goldberg Emulator

   The Goldberg Emulator is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   The Goldberg Emulator is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the Goldberg Emulator; if not, see
   <http://www.gnu.org/licenses/>.  */

#include "settings_parser.h"
#include "steam_api_logger.hpp"

static void consume_bom(std::ifstream &input)
{
    int bom[3];
    bom[0] = input.get();
    bom[1] = input.get();
    bom[2] = input.get();
    if (bom[0] != 0xEF || bom[1] != 0xBB || bom[2] != 0xBF) {
        input.seekg(0);
    }
}

static void load_custom_broadcasts(std::string broadcasts_filepath, std::set<IP_PORT> &custom_broadcasts)
{
    PRINT_DEBUG("Broadcasts file path: %s\n", broadcasts_filepath.c_str());
    std::ifstream broadcasts_file(utf8_decode(broadcasts_filepath));
    consume_bom(broadcasts_file);
    if (broadcasts_file.is_open()) {
        std::string line;
        while (std::getline(broadcasts_file, line)) {
            std::set<IP_PORT> ips = Networking::resolve_ip(line);
            custom_broadcasts.insert(ips.begin(), ips.end());
        }
    }
}

template<typename Out>
static void split_string(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

static void load_gamecontroller_settings(Settings *settings)
{
    std::string path = Local_Storage::get_game_settings_path() + "controller";
    std::vector<std::string> paths = Local_Storage::get_filenames_path(path);

    for (auto & p: paths) {
        size_t length = p.length();
        if (length < 4) continue;
        if ( std::toupper(p.back()) != 'T') continue;
        if ( std::toupper(p[length - 2]) != 'X') continue;
        if ( std::toupper(p[length - 3]) != 'T') continue;
        if (p[length - 4] != '.') continue;

        PRINT_DEBUG("controller config %s\n", p.c_str());
        std::string action_set_name = p.substr(0, length - 4);
        std::transform(action_set_name.begin(), action_set_name.end(), action_set_name.begin(),[](unsigned char c){ return std::toupper(c); });

        std::string controller_config_path = path + PATH_SEPARATOR + p;
        std::ifstream input( utf8_decode(controller_config_path) );
        if (input.is_open()) {
            consume_bom(input);
            std::map<std::string, std::pair<std::set<std::string>, std::string>> button_pairs;

            for( std::string line; getline( input, line ); ) {
                if (!line.empty() && line[line.length()-1] == '\n') {
                    line.pop_back();
                }

                if (!line.empty() && line[line.length()-1] == '\r') {
                    line.pop_back();
                }

                std::string action_name;
                std::string button_name;
                std::string source_mode;

                std::size_t deliminator = line.find("=");
                if (deliminator != 0 && deliminator != std::string::npos && deliminator != line.size()) {
                    action_name = line.substr(0, deliminator);
                    std::size_t deliminator2 = line.find("=", deliminator + 1);

                    if (deliminator2 != std::string::npos && deliminator2 != line.size()) {
                        button_name = line.substr(deliminator + 1, deliminator2 - (deliminator + 1));
                        source_mode = line.substr(deliminator2 + 1);
                    } else {
                        button_name = line.substr(deliminator + 1);
                        source_mode = "";
                    }
                }

                std::transform(action_name.begin(), action_name.end(), action_name.begin(),[](unsigned char c){ return std::toupper(c); });
                std::transform(button_name.begin(), button_name.end(), button_name.begin(),[](unsigned char c){ return std::toupper(c); });
                std::pair<std::set<std::string>, std::string> button_config = {{}, source_mode};
                split_string(button_name, ',', std::inserter(button_config.first, button_config.first.begin()));
                button_pairs[action_name] = button_config;
                PRINT_DEBUG("Added %s %s %s\n", action_name.c_str(), button_name.c_str(), source_mode.c_str());
            }

            settings->controller_settings.action_sets[action_set_name] = button_pairs;
            PRINT_DEBUG("Added %u action names to %s\n", button_pairs.size(), action_set_name.c_str());
        }
    }

    settings->glyphs_directory = path + (PATH_SEPARATOR "glyphs" PATH_SEPARATOR);
}

uint32 create_localstorage_settings(Settings **settings_client_out, Settings **settings_server_out, Local_Storage **local_storage_out)
{
    std::string program_path = Local_Storage::get_program_path(), save_path = Local_Storage::get_user_appdata_path();;

    PRINT_DEBUG("Current Path %s save_path: %s\n", program_path.c_str(), save_path.c_str());

    // ------------------ Load settings from dynamic library -------------------
    struct ApiSettings {
        size_t appid;
        uint64 steam_id;
        char* username;
        char* language;
    };
    // The language and locale don't matter, but encoding must be UTF-8
    // to correctly parse characters from the dynamic library.
    setlocale(LC_ALL, "en_US.UTF-8");

    // Load the dynamic library
#if defined(_WIN32) || defined(_WIN64)
    // Get current DLL path
    wchar_t dllPath[MAX_PATH];
    HMODULE hCurrentModule;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&load_gamecontroller_settings), // Any function from the current module
        &hCurrentModule
    );
    GetModuleFileNameW(hCurrentModule, dllPath, MAX_PATH);

    // Get directory path 
    wchar_t* lastSlash = wcsrchr(dllPath, L'\\');
    if (lastSlash) *(lastSlash + 1) = 0;

    // Append target DLL name
    wcscat_s(dllPath, MAX_PATH, L"steam_api64_settings.dll");

    HMODULE hDylib = LoadLibraryW(dllPath);
#else
    void* hDylib = dlopen("./libsteam_api64_settings.so", RTLD_LAZY);
#endif

    // Check if the dynamic library was loaded
    if (!hDylib) {
        fatal_error(
            L"Failed to load Steam API settings",
            L"加载 Steam API 设置失败"
        );
    }

    // Init dynamic library
    {
        auto init_settings_dylib = (int (*)(void))GetProcAddress(hDylib, "init_dylib");
        if (!init_settings_dylib) {
            FreeLibrary(hDylib);
            fatal_error(
                L"Settings library does not have an init method, this may not be a valid settings library",
                L"设置库不存在初始化方法，这可能不是一个合法的设置库"
            );
        }
        if (!init_settings_dylib()) {
            FreeLibrary(hDylib);
            fatal_error(
                L"Failed to initialize settings library, the library may have issues",
                L"初始化设置库失败，库可能存在问题"
            );
        }
    }

    // Get function pointers
    auto get_settings   = (ApiSettings* (*)(void))          GetProcAddress(hDylib, "get_settings");
    auto free_info_func = (void (*)(ApiSettings* settings)) GetProcAddress(hDylib, "free_settings");
    if (!get_settings || !free_info_func) {
        FreeLibrary(hDylib);
        fatal_error(
            L"Failed to get pointer, the settings dylib may corrupted",
            L"获取设置库指针失败，库可能已损坏"
        );
    }

    // Get settings
    ApiSettings* apiSettings = get_settings();
    if (!apiSettings) {
        FreeLibrary(hDylib);
        fatal_error(
            L"Failed to get Steam API settings",
            L"获取 Steam API 设置失败"
        );
    }
    // -------------------------------------------------------------------------

    // Appid
    const uint32 appid = apiSettings->appid;
    logger(L"Appid got - " + std::to_wstring(appid));

    // Set save path and create Local_Storage object
    bool local_save = false;
    {
        char array[33] = {};
        if (Local_Storage::get_file_data(program_path + "local_save.txt", array, sizeof(array) - 1) != -1) {
            save_path = program_path + Settings::sanitize(array);
            local_save = true;
        }
    }
    Local_Storage *local_storage = new Local_Storage(save_path);
    local_storage->setAppId(appid);

    // Listen port - unused in our case
    const uint16 port = DEFAULT_PORT;

    // Custom broadcasts - unused in our case
    std::set<IP_PORT> custom_broadcasts;

    // Acount name
    std::wstring name = {};
    char* name_cstr = apiSettings->username;
    if (name_cstr) {
        std::string last_call_str = name_cstr;
        name = utf8_to_wide(last_call_str);
    } else {
        fatal_error(
            L"Failed to get username from Steam API",
            L"Steam API 获取用户名失败"
        );
    }
    // If the name length is greater than this MAX_NAME_LEN,
    // it will cause unknown backend errors and API call failures.
    if (name.length() > MAX_NAME_LEN) {
        name.resize(MAX_NAME_LEN);
    }
    logger(L"Username got - " + name);

    // Language
    std::string language;
    if (apiSettings->language) {
        language = apiSettings->language;
    } else {
        fatal_error(
            L"Failed to get language from Steam API",
            L"Steam API 获取语言失败"
        );
    }
    logger("Game Language got - " + std::string(language));

    // Steam ID
    CSteamID user_id;
    if (apiSettings->steam_id) {
        user_id = CSteamID(apiSettings->steam_id);
    } else {
        fatal_error(
            L"Failed to get Steam ID from Steam API",
            L"Steam API 获取 Steam ID 失败"
        );
    }
    logger("SteamID got - " + std::to_string(user_id.ConvertToUint64()));

    // Some settings - unused in our case
    bool steam_offline_mode = false;
    bool disable_networking = false;
    bool disable_overlay = false;
    bool disable_lobby_creation = false;

    // Don't know what this is for, but keep it for now
    int build_id = 10;

    bool warn_forced = false;

    // Free the settings, collect garbage
    free_info_func(apiSettings);
    FreeLibrary(hDylib);

    Settings *settings_client = new Settings(user_id, CGameID(appid), name, language, steam_offline_mode);
    Settings *settings_server = new Settings(generate_steam_id_server(), CGameID(appid), name, language, steam_offline_mode);
    settings_client->set_port(port);
    settings_server->set_port(port);
    settings_client->custom_broadcasts = custom_broadcasts;
    settings_server->custom_broadcasts = custom_broadcasts;
    settings_client->disable_networking = disable_networking;
    settings_server->disable_networking = disable_networking;
    settings_client->disable_overlay = disable_overlay;
    settings_server->disable_overlay = disable_overlay;
    settings_client->disable_lobby_creation = disable_lobby_creation;
    settings_server->disable_lobby_creation = disable_lobby_creation;
    settings_client->build_id = build_id;
    settings_server->build_id = build_id;
    settings_client->warn_forced = warn_forced;
    settings_server->warn_forced = warn_forced;
    settings_client->warn_local_save = local_save;
    settings_server->warn_local_save = local_save;

    load_gamecontroller_settings(settings_client);

    *settings_client_out = settings_client;
    *settings_server_out = settings_server;
    *local_storage_out = local_storage;

    reset_LastError();
    return appid;
}
