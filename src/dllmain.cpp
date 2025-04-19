#include <windows.h>
#include <psapi.h>
#include <dbghelp.h>
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

void CaptureStackTrace(EXCEPTION_POINTERS* exceptionInfo) {
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    
    // Initialize stack frame
    STACKFRAME64 stackFrame;
    ZeroMemory(&stackFrame, sizeof(STACKFRAME64));
    DWORD machine = IMAGE_FILE_MACHINE_AMD64;
    CONTEXT* context = exceptionInfo->ContextRecord;
    
    // Set up the initial stack frame
    stackFrame.AddrPC.Offset = context->Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context->Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context->Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    
    // Capture each stack frame
    for (ULONG frameNum = 0; frameNum < MAX_STACK_FRAMES; frameNum++) {
        BOOL result = StackWalk64(
            machine,
            process,
            thread,
            &stackFrame,
            context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL
        );
        
        if (!result || stackFrame.AddrPC.Offset == 0) {
            break;  // End of stack
        }
        
        // Get module information
        DWORD64 moduleBase = SymGetModuleBase64(process, stackFrame.AddrPC.Offset);
        char moduleName[MAX_PATH] = "<unknown module>";
        if (moduleBase) {
            if (GetModuleFileNameA((HINSTANCE)moduleBase, moduleName, MAX_PATH)) {
                // Extract just the filename from the path
                char* fileName = strrchr(moduleName, '\\');
                if (fileName) {
                    strcpy(moduleName, fileName + 1);
                }
            }
        }
        
        // Symbol lookup
        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)symbolBuffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        
        DWORD64 displacement = 0;
        BOOL gotSymbol = SymFromAddr(process, stackFrame.AddrPC.Offset, &displacement, symbol);
        
        // Line info lookup
        IMAGEHLP_LINE64 line;
        DWORD lineDisplacement = 0;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        BOOL gotLine = SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &lineDisplacement, &line);
        
        // Format and log the frame
        std::string frameInfo = "Frame #" + std::to_string(frameNum) + ": ";
        frameInfo += std::string(moduleName) + "!";
        
        if (gotSymbol) {
            frameInfo += std::string(symbol->Name) + "+0x" + std::to_string(displacement);
        } else {
            frameInfo += "0x" + std::to_string(stackFrame.AddrPC.Offset);
        }
        
        if (gotLine) {
            frameInfo += " [" + std::string(line.FileName) + ":" + std::to_string(line.LineNumber) + "]";
        }
        
        G::logger.LogError(frameInfo);
    }
    G::logger.LogError("End of stack trace");
}

void InitSymbols() {
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    SymInitialize(GetCurrentProcess(), NULL, TRUE);
}


LONG WINAPI CrashHandler(EXCEPTION_POINTERS* exceptionInfo) {
    G::logger.LogError("Crash detected! Exception code: " + 
                     std::to_string(exceptionInfo->ExceptionRecord->ExceptionCode));
    
    // Log the call stack if possible
    G::logger.LogError("Stack trace:");
    CaptureStackTrace(exceptionInfo);

    G::logger.LogError("Start Basic Trace");
    DWORD exceptionCode = exceptionInfo->ExceptionRecord->ExceptionCode;
    void* exceptionAddress = exceptionInfo->ExceptionRecord->ExceptionAddress;
    
    // Log basic info
    G::logger.LogError("Crash detected! Exception code: " + std::to_string(exceptionCode));
    G::logger.LogError("Exception at address: 0x" + std::to_string((uint64_t)exceptionAddress));
    
    // For access violations, log extra info
    if (exceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        ULONG_PTR readWriteFlag = exceptionInfo->ExceptionRecord->ExceptionInformation[0];
        ULONG_PTR memoryAddress = exceptionInfo->ExceptionRecord->ExceptionInformation[1];
        
        std::string accessType = (readWriteFlag == 0) ? "Read" : 
                               (readWriteFlag == 1) ? "Write" : "Execute";
        
        G::logger.LogError("Access violation: Attempted to " + accessType + 
                         " memory at address 0x" + std::to_string(memoryAddress));
    }
    
    // Log register state
    CONTEXT* context = exceptionInfo->ContextRecord;
    G::logger.LogError("Register state:");
    G::logger.LogError("  RIP: 0x" + std::to_string(context->Rip));
    G::logger.LogError("  RSP: 0x" + std::to_string(context->Rsp));
    G::logger.LogError("  RBP: 0x" + std::to_string(context->Rbp));
    G::logger.LogError("End Basic Trace");
    
    return EXCEPTION_CONTINUE_SEARCH;
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
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        InitSymbols();
        SetUnhandledExceptionFilter(CrashHandler);
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