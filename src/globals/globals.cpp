#include "globals.h"

namespace G {
    bool running = true;
    bool initialized = false;
    bool hooksInitialized = false;
    bool showMenu = false;
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

    std::mutex queuedActionsMutex;
    std::queue<std::function<void()>> queuedActions;

    void* localInventory_cached = nullptr;
    std::mutex queuedGiveItemsMutex;
    std::queue<std::tuple<int, int>> queuedGiveItems;
    std::shared_mutex itemsMutex;
    std::vector<RoR2Item> items;
    std::vector<int> itemStacks;

    bool godMode = true;
    float baseMoveSpeed = 10.0f;
    float baseDamage = 10.0f;
    float baseAttackSpeed = 1.0f;
    float baseCrit = 1.0f;
    int32_t baseJumpCount = 1;
}