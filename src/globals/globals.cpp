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

    int worldLayer = -1;
    int playerBodyLayer = -1;
    int enemyBodyLayer = -1;
    int entityPreciseLayer = -1;
    int ignoreRaycastLayer = -1;

    std::mutex queuedActionsMutex;
    std::queue<std::function<void()>> queuedActions;
    std::shared_mutex itemsMutex;
    std::vector<RoR2Item> items;
    std::map<std::string, int> specialItems;
    std::shared_mutex enemiesMutex;
    std::vector<RoR2Enemy> enemies;

    std::map<std::string, int> eliteBuffIndices;
    std::vector<std::string> eliteNames;

    ToggleControl* showMenuControl = new ToggleControl("Show Menu", "showMenu");
    ButtonControl* runningButtonControl = new ButtonControl("Running", "running", "Unload Menu", [](){G::running = false;});

    PlayerModule* localPlayer = new PlayerModule();
    ESPModule* espModule = new ESPModule();
    WorldModule* worldModule = new WorldModule();
    EnemySpawningModule* enemySpawningModule = new EnemySpawningModule();
    void* runInstance = nullptr;
}