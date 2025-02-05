# Steam Emulator

This is a fork of [Mr_Goldberg/goldberg_emulator](https://gitlab.com/Mr_Goldberg/goldberg_emulator) for [The Septenary Gods](https://github.com/orgs/The-Septenary-Gods) team's projects.

Some of the program logic in this project has been customized according to the usage requirements of [The Septenary Gods](https://github.com/orgs/The-Septenary-Gods) team. As for the common logic that is identical to the upstream project, please refer to the [general](https://github.com/The-Septenary-Gods/Steam-Emu/tree/general) branch.

For the original README file based on Mr_Goldberg's repository, see [README-original.md](./README-original.md).

## Building and Installing

### Building Dependencies

- Visual Studio 2019 or later, or Microsoft C++ Build Tools (Windows)
- CMake
- Git
- Vcpkg

### Building

#### Windows

``` Batch
vcpkg install --triplet x86-windows-static
vcpkg install --triplet x64-windows-static
.\build_win_steam_api.bat
```

> This repository has only been tested on Windows platform, and may not work properly on other platforms.

### Installing

Copy `release\steam_api64.dll` to `<Game Root Directory>\Engine\Binaries\ThirdParty\Steamworks\Steamv***\Win64` and overwrite the existing file.

> It is recommended to create a copy of the game for installing this API to avoid affecting the functionality of the original Steam game.
