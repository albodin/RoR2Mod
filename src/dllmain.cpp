#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <windows.h>

#include "globals/globals.hpp"
#include "hooks/hooks.hpp"

void WaitForDebugger() {
    volatile bool g_DebuggerAttached = false;
    while (!g_DebuggerAttached) {
        Sleep(100);
    }
}

DWORD WINAPI Run(LPVOID lpParam) {
    // WaitForDebugger();
    // Init
    Hooks::Init();

    while (G::running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Close Hooks
    Hooks::Unhook();
    CloseHandle(G::mainThread);
    FreeLibraryAndExitThread(G::hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hModule);

        // Create fresh logger instance
        G::logger = Logger();
        LOG_INFO("DLL attached to process");

        G::hModule = hModule;
        G::mainThread = CreateThread(nullptr, 0, Run, nullptr, 0, nullptr);
        break;
    }
    case DLL_THREAD_ATTACH:
        LOG_INFO("DLL_THREAD_ATTACH: Thread %d attached", GetCurrentThreadId());
        break;
    case DLL_THREAD_DETACH:
        LOG_INFO("DLL_THREAD_DETACH: Thread %d detaching", GetCurrentThreadId());
        break;
    case DLL_PROCESS_DETACH:
        if (lpReserved == nullptr) {
            LOG_INFO("DLL_PROCESS_DETACH: Unloading via FreeLibrary");
        } else {
            LOG_INFO("DLL_PROCESS_DETACH: Process termination");
        }
        // Cleanup handled in Run thread
        break;
    }
    return TRUE;
}
