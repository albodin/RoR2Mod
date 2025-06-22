#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <queue>
#include <functional>
#include <shared_mutex>
#include <map>

#include "core/MonoRuntime.h"
#include "game/GameStructs.h"
#include "utils/ModStructs.h"
#include "game/GameFunctions.h"
#include "utils/Logger.h"
#include "menu/InputControls.h"
#include "modules/PlayerModule.h"
#include "modules/ESPModule.h"
#include "modules/WorldModule.h"
#include "modules/EnemySpawningModule.h"

typedef long(__stdcall* Present)(IDXGISwapChain*, UINT, UINT);

namespace G {
    extern bool running;
    extern bool initialized;
    extern bool hooksInitialized;
    extern MonoRuntime* g_monoRuntime;
    extern HMODULE hModule;
    extern HANDLE mainThread;
    extern Present oPresent;
    extern ID3D11Device* pDevice;
    extern ID3D11DeviceContext* pContext;
    extern ID3D11RenderTargetView* mainRenderTargetView;
    extern WNDPROC oWndProc;
    extern HWND windowHwnd;
    extern Logger logger;
    extern GameFunctions* gameFunctions;

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
    extern std::vector<std::string> eliteNames; // For UI dropdown

    extern ToggleControl* showMenuControl;
    extern ButtonControl* runningButtonControl;

    extern PlayerModule* localPlayer;
    extern ESPModule* espModule;
    extern WorldModule* worldModule;
    extern EnemySpawningModule* enemySpawningModule;
    extern void* runInstance;
}