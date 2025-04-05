#pragma once
#include <dxgi.h>

namespace Hooks {
    void Init();

    bool hkRewiredPlayerGetButtonDown(void*, int);
    void hkRoR2LocalUserRebuildControlChain(void*);

    long __stdcall hkPresent11(IDXGISwapChain*, UINT, UINT);
}