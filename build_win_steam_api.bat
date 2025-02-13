@echo off
cd /d "%~dp0"
del /Q /S release\*
rmdir /S /Q release\experimental
rmdir /S /Q release\experimental_steamclient
rmdir /S /Q release\lobby_connect
rmdir /S /Q release
mkdir release
call build_set_protobuf_directories.bat
if not exist build mkdir build

REM ================ x86 build ================

"%PROTOC_X86_EXE%" -I.\dll\ --cpp_out=.\dll\ .\dll\net.proto
call build_env_x86.bat
cd build

REM build .rc file
rc /fo dll_resources.res ..\assets\dll_resources.rc

REM build .dll
cl ../dll/rtlgenrandom.c ../dll/rtlgenrandom.def
cl /LD /DEMU_RELEASE_BUILD /DNDEBUG /wd4828 /I%PROTOBUF_X86_DIRECTORY%\include\ ../dll/*.cpp ../dll/*.cc "%PROTOBUF_X86_LIBRARY%" Iphlpapi.lib Ws2_32.lib rtlgenrandom.lib Shell32.lib User32.lib /EHsc /MP12 /Ox /GL /Gy dll_resources.res /link /debug:none /OUT:..\release\steam_api.dll
cd "%~dp0"

REM ================ x64 build ================

"%PROTOC_X64_EXE%" -I.\dll\ --cpp_out=.\dll\ .\dll\net.proto
call build_env_x64.bat
cd build

REM build .rc file
rc /fo dll_resources.res ..\assets\dll_resources.rc

REM build .dll
cl ../dll/rtlgenrandom.c ../dll/rtlgenrandom.def
cl /LD /DEMU_RELEASE_BUILD /DNDEBUG /wd4828 /I%PROTOBUF_X64_DIRECTORY%\include\ ../dll/*.cpp ../dll/*.cc "%PROTOBUF_X64_LIBRARY%" Iphlpapi.lib Ws2_32.lib rtlgenrandom.lib Shell32.lib User32.lib /EHsc /MP12 /Ox /GL /Gy dll_resources.res /link /debug:none /OUT:..\release\steam_api64.dll
cd "%~dp0"

copy Readme_release.txt release\Readme.txt
xcopy /s files_example\* release\
