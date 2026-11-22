#include "SearchEngine.h"
#include <iostream>
#include <vector>
#include <string>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
// Enable ANSI/VT100 escape code processing on Windows at program start.
// This must be done before any output so that ANSI color codes render
// correctly in both the VS Code integrated terminal and Windows Terminal.
static void enableWindowsAnsi() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE && hOut != NULL) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }
    // Also enable on CONOUT$ directly (handles VS Code ConPTY scenarios)
    HANDLE hConOut = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL, OPEN_EXISTING, 0, NULL);
    if (hConOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hConOut, &dwMode)) {
            SetConsoleMode(hConOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
        CloseHandle(hConOut);
    }
}
#endif

int main(int argc, char* argv[]) {
#ifdef _WIN32
    enableWindowsAnsi();
#endif
    std::vector<std::string> sampleDocuments = {
        "documents/algorithms.txt",
        "documents/computer_networks.txt",
        "documents/cpp.txt",
        "documents/database_systems.txt",
        "documents/data_structures.txt",
        "documents/dsa.txt",
        "documents/oop.txt",
        "documents/operating_systems.txt",
        "documents/python.txt",
        "documents/web_technologies.txt"
    };

    SearchEngine engine;

    if (!engine.initialize(sampleDocuments)) {
        std::cerr << "\nApplication failed to start because documents could not be loaded.\n";
        return 1;
    }

    if (argc > 1) {
        std::string query;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--search" || arg == "-s") {
                continue;
            }
            if (!query.empty()) {
                query += " ";
            }
            query += arg;
        }
        if (!query.empty()) {
            engine.search(query);
            return 0;
        }
    }

    engine.run();

    return 0;
}
