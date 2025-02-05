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

#include "settings.h"
#include <codecvt>

std::string wide_name_to_utf8(const std::wstring &wstr)
{
    try {
        std::string result = wide_to_utf8(wstr);

        // If the result length is less than 17 bytes, pad it to 17 bytes.
        // When result is less than 17 bytes, it shows as random garbage in game.
        // Root cause unknown, might be related to protobuf.
        if (result.length() < 17) {
            result.resize(17, '\0');
        }
        return result;
    } catch (const std::exception&) {
        return INVAILD_USER_NAME;
    }
}

std::string wide_to_utf8(const std::wstring &wstr) {
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

std::wstring utf8_to_wide(const std::string& str) {
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::string Settings::sanitize(std::string name)
{
    name.erase(std::remove(name.begin(), name.end(), '\n'), name.end());
    name.erase(std::remove(name.begin(), name.end(), '\r'), name.end());

    for (auto& i : name)
    {
        if (!isprint(i))
            i = ' ';
    }

    return name;
}

std::wstring Settings::sanitize(std::wstring name)
{
    name.erase(std::remove(name.begin(), name.end(), L'\n'), name.end());
    name.erase(std::remove(name.begin(), name.end(), L'\r'), name.end());
    for (auto &ch : name) {
        if (!iswprint(ch))
            ch = L' ';
    }
    return name;
}

Settings::Settings(CSteamID steam_id, CGameID game_id, std::wstring name, std::string language, bool offline)
{
    this->steam_id = steam_id;
    this->game_id = game_id;
    this->name = sanitize(name);
    if (this->name.size() == 0) {
        this->name = L"  ";
    }
    if (this->name.size() == 1) {
        this->name = this->name + L" ";
    }

    auto lang = sanitize(language);
    std::transform(lang.begin(), lang.end(), lang.begin(), ::tolower);
    lang.erase(std::remove(lang.begin(), lang.end(), ' '), lang.end());
    this->language = lang;
    this->lobby_id = k_steamIDNil;
    this->unlockAllDLCs = true;
    this->offline = offline;
    this->create_unknown_leaderboards = true;
}

CSteamID Settings::get_local_steam_id()
{
    return steam_id;
}

CGameID Settings::get_local_game_id()
{
    return game_id;
}

std::string Settings::get_local_name()
{
    return wide_name_to_utf8(this->name);
}

const char *Settings::get_language()
{
    return language.c_str();
}

void Settings::set_local_name(wchar_t *name)
{
    this->name = name;
}

void Settings::set_language(char *language)
{
    this->language = language;
}

void Settings::set_game_id(CGameID game_id)
{
    this->game_id = game_id;
}

void Settings::set_lobby(CSteamID lobby_id)
{
    this->lobby_id = lobby_id;
}

CSteamID Settings::get_lobby()
{
    return this->lobby_id;
}

void Settings::unlockAllDLC(bool value)
{
    this->unlockAllDLCs = value;
}

void Settings::addDLC(AppId_t appID, std::string name, bool available)
{
    auto f = std::find_if(DLCs.begin(), DLCs.end(), [&appID](DLC_entry const& item) { return item.appID == appID; });
    if (DLCs.end() != f) {
        f->name = name;
        f->available = available;
        return;
    }

    DLC_entry new_entry;
    new_entry.appID = appID;
    new_entry.name = name;
    new_entry.available = available;
    DLCs.push_back(new_entry);
}

void Settings::addMod(PublishedFileId_t id, std::string title, std::string path)
{
    auto f = std::find_if(mods.begin(), mods.end(), [&id](Mod_entry const& item) { return item.id == id; });
    if (mods.end() != f) {
        f->title = title;
        f->path = path;
        return;
    }

    Mod_entry new_entry;
    new_entry.id = id;
    new_entry.title = title;
    new_entry.path = path;
    mods.push_back(new_entry);
}

Mod_entry Settings::getMod(PublishedFileId_t id)
{
    auto f = std::find_if(mods.begin(), mods.end(), [&id](Mod_entry const& item) { return item.id == id; });
    if (mods.end() != f) {
        return *f;
    }

    return Mod_entry();
}

bool Settings::isModInstalled(PublishedFileId_t id)
{
    auto f = std::find_if(mods.begin(), mods.end(), [&id](Mod_entry const& item) { return item.id == id; });
    if (mods.end() != f) {
        return true;
    }

    return false;
}

std::set<PublishedFileId_t> Settings::modSet()
{
    std::set<PublishedFileId_t> ret_set;

    for (auto & m: mods) {
        ret_set.insert(m.id);
    }

    return ret_set;
}

unsigned int Settings::DLCCount()
{
    return this->DLCs.size();
}

bool Settings::hasDLC(AppId_t appID)
{
    if (this->unlockAllDLCs) return true;

    auto f = std::find_if(DLCs.begin(), DLCs.end(), [&appID](DLC_entry const& item) { return item.appID == appID; });
    if (DLCs.end() == f)
        return false;

    return f->available;
}

bool Settings::getDLC(unsigned int index, AppId_t &appID, bool &available, std::string &name)
{
    if (index >= DLCs.size()) return false;

    appID = DLCs[index].appID;
    available = DLCs[index].available;
    name = DLCs[index].name;
    return true;
}

void Settings::setAppInstallPath(AppId_t appID, std::string path)
{
    app_paths[appID] = path;
}

std::string Settings::getAppInstallPath(AppId_t appID)
{
    return app_paths[appID];
}

void Settings::setLeaderboard(std::string leaderboard, enum ELeaderboardSortMethod sort_method, enum ELeaderboardDisplayType display_type)
{
    Leaderboard_config leader;
    leader.sort_method = sort_method;
    leader.display_type = display_type;

    leaderboards[leaderboard] = leader;
}

int Settings::add_image(std::string data, uint32 width, uint32 height)
{
    int last = images.size() + 1;
    struct Image_Data dt;
    dt.width = width;
    dt.height = height;
    dt.data = data;
    images[last] = dt;
    return last;
}
