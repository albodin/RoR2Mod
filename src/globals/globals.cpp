#include "globals.h"

namespace G {
    bool running = true;
    bool initialized = false;
    bool hooksInitialized = false;
    std::unique_ptr<MonoRuntime> g_monoRuntime = nullptr;
    HMODULE hModule = nullptr;
    HANDLE mainThread = nullptr;
    Present oPresent = nullptr;
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    ID3D11RenderTargetView* mainRenderTargetView = nullptr;
    WNDPROC oWndProc = nullptr;
    HWND windowHwnd = nullptr;
    Logger logger("ror2mod.log");
    std::unique_ptr<GameFunctions> gameFunctions = nullptr;

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

    std::unique_ptr<ToggleControl> showMenuControl = std::make_unique<ToggleControl>("Show Menu", "showMenu");
    std::unique_ptr<ButtonControl> runningButtonControl = std::make_unique<ButtonControl>("Running", "running", "Unload Menu", [](){G::running = false;});

    std::unique_ptr<PlayerModule> localPlayer = std::make_unique<PlayerModule>();
    std::unique_ptr<ESPModule> espModule = std::make_unique<ESPModule>();
    std::unique_ptr<WorldModule> worldModule = std::make_unique<WorldModule>();
    std::unique_ptr<EnemySpawningModule> enemySpawningModule = std::make_unique<EnemySpawningModule>();
    void* runInstance = nullptr;
}