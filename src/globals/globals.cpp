#include "globals.h"

namespace G {
    bool running = true;
    bool initialized = false;
    bool showMenu = true;
    MonoRuntime* g_monoRuntime = nullptr;
    HMODULE hModule = nullptr;
    HANDLE mainThread = nullptr;
    Present oPresent = nullptr;
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    ID3D11RenderTargetView* mainRenderTargetView = nullptr;
    WNDPROC oWndProc = nullptr;
    HWND windowHwnd = nullptr;
    Logger logger("ror2mod.log");
    GameFunctions* gameFunctions = nullptr;
    std::queue<std::function<void()>> queuedActions;

    bool godMode = false;
    float baseMoveSpeed = 10.0f;
    float baseDamage = 10.0f;
    float baseAttackSpeed = 1.0f;
    float baseCrit = 1.0f;
    int32_t baseJumpCount = 1;
}