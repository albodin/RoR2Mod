#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <cstring>

void PrintLastError(const char* prefix) {
    DWORD error = GetLastError();
    char errorMessage[256];
    
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        error,
        0,
        errorMessage,
        sizeof(errorMessage),
        NULL
    );
    
    std::cout << prefix << " Error: " << error << " - " << errorMessage << std::endl;
}

DWORD GetProcessIdByName(const char* processName) {
    PROCESSENTRY32 processEntry = {0};
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        PrintLastError("Failed to create toolhelp snapshot");
        return 0;
    }

    DWORD pid = 0;
    if (Process32First(hSnapshot, &processEntry)) {
        do {
            if (strcmp(processEntry.szExeFile, processName) == 0) {
                pid = processEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &processEntry));
    }
    CloseHandle(hSnapshot);
    return pid;
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "Usage: injector.exe <process_name> <dll_path>" << std::endl;
        return 1;
    }

    const char* processName = argv[1];
    const char* dllPath = argv[2];
    size_t dllPathLen = strlen(dllPath) + 1; // Include null terminator

    std::cout << "Target Process: " << processName << std::endl;
    std::cout << "DLL Path: " << dllPath << " (Length: " << dllPathLen << " bytes)" << std::endl;

    DWORD pid = GetProcessIdByName(processName);
    if (pid == 0) {
        std::cout << "Process not found." << std::endl;
        return 1;
    }
    std::cout << "Process ID: " << pid << std::endl;

    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
                                 PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 
                                 FALSE, pid);
    if (!hProcess) {
        PrintLastError("Failed to open process");
        return 1;
    }
    std::cout << "Process handle: " << hProcess << std::endl;

    // Allocate space for the DLL path in the target process.
    LPVOID allocMem = VirtualAllocEx(hProcess, NULL, dllPathLen,
                                    MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!allocMem) {
        PrintLastError("Failed to allocate memory in target process");
        CloseHandle(hProcess);
        return 1;
    }
    std::cout << "Allocated memory address: " << allocMem << std::endl;
    
    DWORD oldProtect;
    if (!VirtualProtectEx(hProcess, allocMem, dllPathLen, PAGE_READWRITE, &oldProtect)) {
        PrintLastError("Failed to set memory protection");
    }

    // Try multiple times to write the path
    SIZE_T bytesWritten = 0;
    BOOL writeResult = WriteProcessMemory(hProcess, allocMem, dllPath, dllPathLen, &bytesWritten);
    if (writeResult) {
        std::cout << "Successfully wrote " << bytesWritten << " bytes." << std::endl;
    } else {
        std::cout << "All write attempts failed." << std::endl;
        VirtualFreeEx(hProcess, allocMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32) {
        PrintLastError("Failed to get kernel32.dll handle");
        VirtualFreeEx(hProcess, allocMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }
    
    LPVOID loadLibraryAddr = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryA");
    if (!loadLibraryAddr) {
        PrintLastError("Failed to get address of LoadLibraryA");
        VirtualFreeEx(hProcess, allocMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }
    std::cout << "LoadLibraryA address: " << loadLibraryAddr << std::endl;

    // Create a remote thread that calls LoadLibraryA with the DLL path.
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
                        (LPTHREAD_START_ROUTINE)loadLibraryAddr,
                        allocMem, 0, NULL);
    if (!hThread) {
        PrintLastError("Failed to create remote thread in target process");
        VirtualFreeEx(hProcess, allocMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }
    std::cout << "Remote thread created with handle: " << hThread << std::endl;

    // Wait until the remote thread finishes with timeout
    DWORD waitResult = WaitForSingleObject(hThread, 5000); // 5 second timeout
    
    if (waitResult == WAIT_OBJECT_0) {
        // Get thread exit code (which is the DLL handle or NULL)
        DWORD exitCode = 0;
        if (GetExitCodeThread(hThread, &exitCode)) {
            if (exitCode) {
                std::cout << "DLL injected successfully. Handle: 0x" << std::hex << exitCode << std::dec << std::endl;
            } else {
                std::cout << "LoadLibrary returned NULL - injection failed." << std::endl;
            }
        }
    } else if (waitResult == WAIT_TIMEOUT) {
        std::cout << "Warning: Thread did not complete within timeout." << std::endl;
    }

    // Clean up
    VirtualFreeEx(hProcess, allocMem, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    return 0;
}