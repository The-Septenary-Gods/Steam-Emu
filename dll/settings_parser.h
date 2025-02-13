/* Copyright (C) 2019 Mr Goldberg
   Copyright (C) 2025 The Septenary Gods
   This file is part of the Goldberg Emulator and TSG's Steam Emu.

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

#ifndef SETTINGS_PARSER_INCLUDE
#define SETTINGS_PARSER_INCLUDE

// If the name length is greater than this value,
// it will cause unknown backend errors and API call failures.
#define MAX_NAME_LEN 32

//returns appid
uint32 create_localstorage_settings(Settings **settings_client_out, Settings **settings_server_out, Local_Storage **local_storage_out);

#endif
