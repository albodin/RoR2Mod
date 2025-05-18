#include <windows.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>

#include "hooks/hooks.h"
#include "globals/globals.h"

#define MAX_STACK_FRAMES 100

void WaitForDebugger() {
    volatile bool g_DebuggerAttached = false;
    while (!g_DebuggerAttached) {
        Sleep(100);
    }
}

DWORD WINAPI Run(LPVOID lpParam) {
    //WaitForDebugger();
    //Init
    Hooks::Init();

    while (G::running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    //Close Hooks
    Hooks::Unhook();
    CloseHandle(G::mainThread);
    FreeLibraryAndExitThread(G::hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        G::logger.LogInfo("DLL attached to process");

        G::hModule = hModule;
        G::mainThread = CreateThread(nullptr, 0, Run, nullptr, 0, nullptr);
        break;
    case DLL_THREAD_ATTACH:
        //G::logger.LogInfo("Thread attached");
        break;
    case DLL_THREAD_DETACH:
        //G::logger.LogInfo("Thread detached");
        break;
    case DLL_PROCESS_DETACH:
        G::logger.LogInfo("DLL detached from process");
        break;
    }
    return TRUE;
}