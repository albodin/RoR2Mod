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
#include "game/GameFunctions.h"
#include "utils/Logger.h"
#include "menu/InputControls.h"

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

    extern std::mutex queuedActionsMutex;
    extern std::queue<std::function<void()>> queuedActions;

    extern void* localInventory_cached;
    extern std::mutex queuedGiveItemsMutex;
    extern std::queue<std::tuple<int, int>> queuedGiveItems;
    extern std::shared_mutex itemsMutex;
    extern std::vector<RoR2Item> items;
    extern std::vector<int> itemStacks;
    extern std::map<int, IntControl*> itemControls;

    extern ToggleControl* showMenuControl;
    extern ButtonControl* runningButtonControl;

    extern ToggleControl* godModeControl;
    extern FloatControl* baseMoveSpeedControl;
    extern FloatControl* baseDamageControl;
    extern FloatControl* baseAttackSpeedControl;
    extern FloatControl* baseCritControl;
    extern IntControl* baseJumpCountControl;
}