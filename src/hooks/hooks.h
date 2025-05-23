#pragma once
#include "game/GameStructs.h"
#include <dxgi.h>

#define DECLARE_INTERNAL_CALL(ReturnType, Class, Method, ...) \
    typedef ReturnType (*Class##_##Method##_fn)(__VA_ARGS__); \
    extern Class##_##Method##_fn Class##_##Method
namespace Hooks {
    DECLARE_INTERNAL_CALL(void, Transform, get_position_Injected, void* transform, Vector3* outPosition);
    DECLARE_INTERNAL_CALL(Camera*, Camera, get_main);
    DECLARE_INTERNAL_CALL(void, Camera, WorldToScreenPoint_Injected, void* camera, Vector3* position,
                          MonoOrStereoscopicEye eye, Vector3* outPosition);

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
    void hkRoR2TeleporterInteractionAwake(void*);
    void hkRoR2TeleporterInteractionFixedUpdate(void*);

    long __stdcall hkPresent11(IDXGISwapChain*, UINT, UINT);
}