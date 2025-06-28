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
    DECLARE_INTERNAL_CALL(void, Physics, get_defaultPhysicsScene_Injected, PhysicsScene_Value* outScene);
    DECLARE_INTERNAL_CALL(bool, PhysicsScene, Internal_Raycast_Injected,
                        PhysicsScene_Value* scene, Ray_Value* ray, float maxDistance,
                        RaycastHit_Value* hitInfo, int32_t layerMask, QueryTriggerInteraction_Value queryTriggerInteraction);
    DECLARE_INTERNAL_CALL(void*, Component, get_transform, void* component);

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
    void hkRoR2InventoryRemoveItem(void*, int, int);
    int hkRoR2ItemStealControllerStolenInventoryInfoStealItem(void*, int, int, void*);
    void hkRoR2SteamworksServerManagerTagsStringUpdated(void*);
    void hkRoR2TeleporterInteractionAwake(void*);
    void hkRoR2TeleporterInteractionFixedUpdate(void*);
    void hkRoR2ConvertPlayerMoneyToExperienceFixedUpdate(void*);
    void hkRoR2CharacterBodyStart(void*);
    void hkRoR2CharacterBodyOnDestroy(void*);
    void hkRoR2CharacterMotorAddDisplacement(void*, Vector3*);
    void hkRoR2CharacterMotorApplyForce(void*, Vector3*, bool, bool);
    void hkRoR2HuntressTrackerStart(void*);
    void* hkRoR2BullseyeSearchGetResults(void*);
    void hkRoR2BullseyeSearchRefreshCandidates(void*);
    void hkRoR2PurchaseInteractionStart(void*);
    void hkRoR2BarrelInteractionStart(void*);
    void hkRoR2GenericPickupControllerStart(void*);
    void hkRoR2TimedChestControllerOnEnable(void*);
    void hkRoR2TimedChestControllerOnDisable(void*);
    void hkRoR2GenericInteractionOnEnable(void*);
    void hkRoR2PickupPickerControllerAwake(void*);
    void hkRoR2ScrapperControllerStart(void*);
    void hkRoR2RunAdvanceStage(void*, void*);
    void hkRoR2RunAwake(void*);
    void hkRoR2ChestBehaviorStart(void*);
    void hkRoR2ShopTerminalBehaviorStart(void*);
    void hkRoR2PressurePlateControllerStart(void*);
    void hkRoR2HoldoutZoneControllerUpdate(void*);
    int hkRoR2TimedChestControllerGetInteractability(void*, void*);
    void hkRoR2PortalSpawnerStart(void*);
    void hkRoR2TeamManagerOnEnable(void*);
    void hkRoR2TeamManagerOnDisable(void*);

    long __stdcall hkPresent11(IDXGISwapChain*, UINT, UINT);
}