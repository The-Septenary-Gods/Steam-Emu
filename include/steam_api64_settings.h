#ifndef STEAM_API64_SETTINGS_H
#define STEAM_API64_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct ApiSettings {
    size_t appid;       // Steam AppID
    uint64_t steam_id;  // SteamID 64bit
    char* username;     // Steam username
    char* language;     // Game language code (see the note below)
};

// Language code: The 3rd column of the table in the link below
// https://partner.steamgames.com/doc/store/localization/languages

// Initialize the dynamic library, call once before everything starts
// Bool-style return value: 0 for failure, non-0 for success
int init_dylib(void);
// Get the settings from the dynamic library
struct ApiSettings* get_settings(void);
// Free the settings, call after everything ends
void free_settings(struct ApiSettings* settings);

#ifdef __cplusplus
}
#endif

#endif // STEAM_API64_SETTINGS_H
