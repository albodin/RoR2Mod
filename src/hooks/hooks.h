#pragma once
#include <dxgi.h>

namespace Hooks {
    void Init();
    void Unhook();

    void hkRoR2RoR2ApplicationUpdate(void*);
    bool hkRewiredPlayerGetButtonDown(void*, int);
    void hkRoR2RoR2ApplicationUpdateCursorState(void*);
    void hkRoR2MPEventSystemManagerUpdate(void*);
    void hkUnityEngineCursorset_lockState(void*, int);
    void hkUnityEngineCursorset_visible(void*, bool);
    void hkRoR2LocalUserRebuildControlChain(void*);
    void hkRoR2InventoryHandleInventoryChanged(void*);
    void hkRoR2SteamworksServerManagerTagsStringUpdated(void*);

    long __stdcall hkPresent11(IDXGISwapChain*, UINT, UINT);
}