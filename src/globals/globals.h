#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>

#include "core/MonoRuntime.h"
#include "game/GameFunctions.h"
#include "utils/Logger.h"

typedef long(__stdcall* Present)(IDXGISwapChain*, UINT, UINT);

namespace G {
    extern bool running;
    extern bool initialized;
    extern bool showMenu;
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

    extern bool godMode;
    extern float baseMoveSpeed;
    extern float baseDamage;
    extern float baseAttackSpeed;
    extern float baseCrit;
    extern int32_t baseJumpCount;
}