#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <windows.h>

#include "core/MonoRuntime.h"
#include "game/GameFunctions.h"
#include "game/GameStructs.h"
#include "helper/CSharpHelper.h"
#include "menu/InputControls.h"
#include "modules/ESPModule.h"
#include "modules/EnemyModule.h"
#include "modules/EnemySpawningModule.h"
#include "modules/InteractableSpawningModule.h"
#include "modules/PlayerModule.h"
#include "modules/WorldModule.h"
#include "utils/Logger.h"
#include "utils/ModStructs.h"

typedef long(__stdcall* Present)(IDXGISwapChain*, UINT, UINT);

namespace G {
extern bool running;
extern bool initialized;
extern bool hooksInitialized;
extern std::unique_ptr<MonoRuntime> g_monoRuntime;
extern HMODULE hModule;
extern HANDLE mainThread;
extern Present oPresent;
extern ID3D11Device* pDevice;
extern ID3D11DeviceContext* pContext;
extern ID3D11RenderTargetView* mainRenderTargetView;
extern WNDPROC oWndProc;
extern HWND windowHwnd;
extern Logger logger;
extern std::unique_ptr<GameFunctions> gameFunctions;

extern int worldLayer;
extern int playerBodyLayer;
extern int enemyBodyLayer;
extern int entityPreciseLayer;
extern int ignoreRaycastLayer;

extern std::mutex queuedActionsMutex;
extern std::queue<std::function<void()>> queuedActions;

extern std::shared_mutex itemsMutex;
extern std::vector<RoR2Item> items;
extern std::map<std::string, int> specialItems; // name -> index for special items like UseAmbientLevel

extern std::shared_mutex enemiesMutex;
extern std::vector<RoR2Enemy> enemies;

extern std::map<std::string, int> eliteBuffIndices; // name -> BuffIndex for elite buffs
extern std::vector<std::string> eliteNames;         // For UI dropdown

extern std::unique_ptr<ToggleControl> showMenuControl;
extern std::unique_ptr<ButtonControl> runningButtonControl;

extern std::unique_ptr<PlayerModule> localPlayer;
extern std::unique_ptr<ESPModule> espModule;
extern std::unique_ptr<WorldModule> worldModule;
extern std::unique_ptr<EnemySpawningModule> enemySpawningModule;
extern std::unique_ptr<EnemyModule> enemyModule;
extern std::unique_ptr<InteractableSpawningModule> interactableSpawningModule;
extern std::unique_ptr<CSharpHelper> csHelper;

extern void* runInstance;
} // namespace G
