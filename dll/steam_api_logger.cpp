#include "steam_api_logger.hpp"

#if defined(_WIN32) || defined(_WIN64)
HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
#endif

void fatal_error(const std::wstring& error_msg_en, const std::wstring& error_msg_zh) {
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

void logger(const std::string& msg, const std::string& type) {
    const std::string prefix = LOGGING_PREFIX + type + ": ";
    std::cout << prefix << msg << std::endl;
}

void logger(const std::wstring& msg, const std::wstring& type) {
    const std::wstring prefix = LOGGING_PREFIX_W + type + L": ";
    std::wcout << prefix << msg << std::endl;
}

void warning_logger(const std::wstring& msg) {
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

void error_logger(const std::wstring& msg) {
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