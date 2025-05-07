#include "globals.h"

namespace G {
    bool running = true;
    bool initialized = false;
    bool hooksInitialized = false;
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
    std::map<int, IntControl*> itemControls;

    ToggleControl* showMenuControl = new ToggleControl("Show Menu", "showMenu");
    ButtonControl* runningButtonControl = new ButtonControl("Running", "running", "Unload Menu", [](){G::running = false;});

    ToggleControl* godModeControl = new ToggleControl("Godmode", "godMode", true);
    FloatControl* baseMoveSpeedControl = new FloatControl("Base Move Speed", "baseMoveSpeed", 10.0f);
    FloatControl* baseDamageControl = new FloatControl("Base Damage", "baseDamage", 10.0f, 0.0f, FLT_MAX, 10.0f);
    FloatControl* baseAttackSpeedControl = new FloatControl("Base Attack Speed", "baseAttackSpeed", 10.0f, 0.0f, FLT_MAX, 10.0f);
    FloatControl* baseCritControl = new FloatControl("Base Crit", "baseCrit", 10.0f, 0.0f, FLT_MAX, 10.0f);
    IntControl* baseJumpCountControl = new IntControl("Base Jump Count", "baseJumpCount", 1, 0, INT_MAX, 10);
}