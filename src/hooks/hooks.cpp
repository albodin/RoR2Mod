#include "globals/globals.h"
#include "hooks.h"
#include "kiero.h"
#include "version.h"
#include "imgui/imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <map>
#include <string>
#include <algorithm>
#include "minhook/include/MinHook.h"
#include "menu/menu.h"
#include "game/GameStructs.h"
#include "core/MonoList.h"
#include "utils/Math.h"
#include "fonts/FontManager.h"
#include "config/ConfigManager.h"

//#define DEBUG_PRINT

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::map<std::string, LPVOID> hooks;
std::map<std::string, LPVOID> hookTargets;
std::queue<std::pair<std::string, std::function<void()>>> internalCallInitQueue;

bool CreateHook(
    const char* assemblyName,
    const char* nameSpace,
    const char* className,
    const char* methodName,
    int paramCount,
    const char* returnType,
    const char** paramTypes,
    LPVOID pDetour,
    LPVOID* ppOriginal
) {
    std::string hookName = std::string(nameSpace) + std::string(className) + std::string(methodName);
    bool hook_success = false;

    while (true) {
        LPVOID pTarget = G::g_monoRuntime->GetMethodAddress(
            assemblyName, nameSpace, className, methodName, paramCount, returnType, paramTypes);

        if (!pTarget) {
            G::logger.LogError("Failed to get method address for " + std::string(assemblyName) + "::" +
                               std::string(nameSpace) + "::" + std::string(className) + "::" + std::string(methodName));
            return false;
        }

        // Try to create the hook
        MH_STATUS create_status = MH_CreateHook(pTarget, pDetour, ppOriginal);
        if (create_status != MH_OK) {
            continue;
        }

        hooks[hookName] = *ppOriginal;
        hookTargets[hookName] = pTarget;
        hook_success = true;
        break;
    }

    if (!hook_success) {
        G::logger.LogError("Hook creation failed for " + std::string(assemblyName) + "::" +
                           std::string(nameSpace) + "::" + std::string(className) + "::" + std::string(methodName));
        return false;
    }

    return true;
}

#define STRINGIFY(x) #x
#define HOOK(assembly, ns, class, method, paramcount, returntype, ...) \
{ \
    const char* paramTypes[] = __VA_ARGS__; \
    void (*fp ## ns ## class ## method)(void); \
    if (CreateHook(#assembly, #ns, #class, #method, paramcount, returntype, paramTypes, \
               reinterpret_cast<LPVOID>(&Hooks::hk ## ns ## class ## method), \
               reinterpret_cast<LPVOID*>(&fp ## ns ## class ## method))) { \
        G::logger.LogInfo(STRINGIFY(Hooked ns##class##method)); \
    } \
    else { \
        G::logger.LogError(STRINGIFY(Failed to hook ns##class##method)); \
        G::running = false; \
    } \
}

#define HOOK_NESTED(assembly, ns, parentclass, nestedclass, method, paramcount, returntype, ...) \
{ \
    const char* paramTypes[] = __VA_ARGS__; \
    void (*fp ## ns ## parentclass ## nestedclass ## method)(void); \
    if (CreateHook(#assembly, #ns, #parentclass "+" #nestedclass, #method, paramcount, returntype, paramTypes, \
               reinterpret_cast<LPVOID>(&Hooks::hk ## ns ## parentclass ## nestedclass ## method), \
               reinterpret_cast<LPVOID*>(&fp ## ns ## parentclass ## nestedclass ## method))) { \
        G::logger.LogInfo(STRINGIFY(Hooked ns##parentclass##nestedclass##method)); \
    } \
    else { \
        G::logger.LogError(STRINGIFY(Failed to hook ns##parentclass##nestedclass##method)); \
        G::running = false; \
    } \
}

#define DEFINE_INTERNAL_CALL(Assembly, Namespace, Class, Method, ParamCount, ReturnType, ...) \
    typedef ReturnType (*Class##_##Method##_fn)(__VA_ARGS__); \
    Class##_##Method##_fn Hooks::Class##_##Method = nullptr; \
    \
    static void Init##Class##_##Method() { \
        if (Hooks::Class##_##Method) return; \
        \
        MonoClass* klass = G::g_monoRuntime->GetClass(#Assembly, #Namespace, #Class); \
        if (!klass) { \
            G::logger.LogError("Failed to find class " #Namespace "." #Class); \
            G::running = false; \
            return; \
        } \
        \
        MonoMethod* method = G::g_monoRuntime->GetMethod(klass, #Method, ParamCount); \
        if (!method) { \
            G::logger.LogError("Failed to find method " #Namespace "." #Class "::" #Method); \
            G::running = false; \
            return; \
        } \
        \
        void* funcPtr = G::g_monoRuntime->GetInternalCallPointer(method); \
        if (!funcPtr) { \
            G::logger.LogError("Failed to get internal call pointer for " #Namespace "." #Class "::" #Method); \
            G::running = false; \
            return; \
        } \
        \
        Hooks::Class##_##Method = (Class##_##Method##_fn)funcPtr; \
        G::logger.LogInfo("Successfully initialized " #Namespace "." #Class "::" #Method " at %p", funcPtr); \
    } \
    static const bool Class##_##Method##_registered = []() { \
        internalCallInitQueue.push(std::make_pair(#Namespace "." #Class "::" #Method, Init##Class##_##Method)); \
        return true; \
    }()

DEFINE_INTERNAL_CALL(UnityEngine.CoreModule, UnityEngine, Transform, get_position_Injected, 1, void,
                   void* transform, Vector3* outPosition);
DEFINE_INTERNAL_CALL(UnityEngine.CoreModule, UnityEngine, Camera, get_main, 0, Camera*, void);
DEFINE_INTERNAL_CALL(UnityEngine.CoreModule, UnityEngine, Camera, WorldToScreenPoint_Injected, 3, void,
                   void* camera, Vector3* position, MonoOrStereoscopicEye eye, Vector3* outPosition);
DEFINE_INTERNAL_CALL(UnityEngine.PhysicsModule, UnityEngine, Physics, get_defaultPhysicsScene_Injected, 1, void, PhysicsScene_Value* outScene);
DEFINE_INTERNAL_CALL(UnityEngine.PhysicsModule, UnityEngine, PhysicsScene, Internal_Raycast_Injected, 6, bool,
                   PhysicsScene_Value*, Ray_Value*, float, RaycastHit_Value*, int32_t, QueryTriggerInteraction_Value);
DEFINE_INTERNAL_CALL(UnityEngine.CoreModule, UnityEngine, Component, get_transform, 0, void*,
                   void* component);

void Hooks::Init() {
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK) {
        G::logger.LogError("Failed to initialize MinHook: %s", MH_StatusToString(status));
        G::running = false;
        return;
    }

    G::g_monoRuntime = new MonoRuntime();
    while (!G::g_monoRuntime->Initialize()) {
        Sleep(100);
    }
    G::gameFunctions = new GameFunctions(G::g_monoRuntime);
    while (G::gameFunctions->RoR2Application_IsLoading() && !G::gameFunctions->RoR2Application_IsLoadFinished() &&
            G::gameFunctions->RoR2Application_GetLoadGameContentPercentage() < 10) {
        G::logger.LogInfo("Waiting for RoR2 to load...");
        Sleep(1000);
    }

    bool hooked = false;
    while (!hooked) {
        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
            kiero::bind(8, (void**)&G::oPresent, (void*)hkPresent11);
            hooked = true;
        }
    }

    HOOK(RoR2, RoR2, RoR2Application, Update, 0, "System.Void", {});
    HOOK(Rewired_Core, Rewired, Player, GetButtonDown, 1, "System.Boolean", {"System.Int32"});
    HOOK(RoR2, RoR2, RoR2Application, UpdateCursorState, 0, "System.Void", {});
    HOOK(RoR2, RoR2, MPEventSystemManager, Update, 0, "System.Void", {});
    HOOK(UnityEngine.CoreModule, UnityEngine, Cursor, set_lockState, 1, "System.Void", {"UnityEngine.CursorLockMode"});
    HOOK(UnityEngine.CoreModule, UnityEngine, Cursor, set_visible, 1, "System.Void", {"System.Boolean"});
    HOOK(RoR2, RoR2, LocalUser, RebuildControlChain, 0, "System.Void", {});
    HOOK(RoR2, RoR2, Inventory, HandleInventoryChanged, 0, "System.Void", {});
    HOOK(RoR2, RoR2, Inventory, RemoveItem, 2, "System.Void", {"RoR2.ItemIndex", "System.Int32"});
    HOOK_NESTED(RoR2, RoR2, ItemStealController, StolenInventoryInfo, StealItem, 3, "System.Int32", {"RoR2.ItemIndex", "System.Int32", "System.Nullable<System.Boolean>"});
    HOOK(RoR2, RoR2, SteamworksServerManager, TagsStringUpdated, 0, "System.Void", {});
    HOOK(RoR2, RoR2, TeleporterInteraction, Awake, 0, "System.Void", {});
    HOOK(RoR2, RoR2, TeleporterInteraction, FixedUpdate, 0, "System.Void", {});
    HOOK(RoR2, RoR2, CharacterBody, Start, 0, "System.Void", {});
    HOOK(RoR2, RoR2, CharacterBody, OnDestroy, 0, "System.Void", {});
    HOOK(RoR2, RoR2, HuntressTracker, Start, 0, "System.Void", {});
    HOOK(RoR2, RoR2, BullseyeSearch, GetResults, 0, "System.Collections.Generic.IEnumerable<RoR2.HurtBox>", {});
    HOOK(RoR2, RoR2, BullseyeSearch, RefreshCandidates, 0, "System.Void", {});
    HOOK(RoR2, RoR2, PurchaseInteraction, Start, 0, "System.Void", {});
    HOOK(RoR2, RoR2, BarrelInteraction, Start, 0, "System.Void", {});
    HOOK(RoR2, RoR2, GenericPickupController, Start, 0, "System.Void", {});
    HOOK(RoR2, RoR2, TimedChestController, OnEnable, 0, "System.Void", {});
    HOOK(RoR2, RoR2, TimedChestController, OnDisable, 0, "System.Void", {});
    HOOK(RoR2, RoR2, GenericInteraction, OnEnable, 0, "System.Void", {});
    HOOK(RoR2, RoR2, PickupPickerController, Awake, 0, "System.Void", {});
    HOOK(RoR2, RoR2, ScrapperController, Start, 0, "System.Void", {});
    HOOK(RoR2, RoR2, Run, AdvanceStage, 1, "System.Void", {"RoR2.SceneDef"});
    HOOK(RoR2, RoR2, Run, Awake, 0, "System.Void", {});
    HOOK(RoR2, RoR2, ChestBehavior, Start, 0, "System.Void", {});
    HOOK(RoR2, RoR2, ShopTerminalBehavior, Start, 0, "System.Void", {});
    HOOK(RoR2, RoR2, PressurePlateController, Start, 0, "System.Void", {});
    HOOK(RoR2, RoR2, HoldoutZoneController, Update, 0, "System.Void", {});
    HOOK(RoR2, RoR2, TimedChestController, GetInteractability, 1, "RoR2.Interactability", {"RoR2.Interactor"});
    HOOK(RoR2, RoR2, PortalSpawner, Start, 0, "System.Void", {});


    for (auto& target: hookTargets) {
        MH_STATUS enable_status = MH_EnableHook(target.second);
        if (enable_status == MH_OK) {
            G::logger.LogInfo("Hook successfully enabled for " + target.first);
        } else {
            G::logger.LogError("Hook enabling failed for " + target.first + ", error: " +
                             MH_StatusToString(enable_status) + " (code: " + MH_StatusToString(enable_status) + ")");
        }
    }

    G::logger.LogInfo("Initializing %zu internal calls", internalCallInitQueue.size());
    while (!internalCallInitQueue.empty()) {
        auto [name, func] = internalCallInitQueue.front();
        internalCallInitQueue.pop();

        G::logger.LogInfo("Initializing internal call: %s", name.c_str());
        func();
    }

    G::localPlayer->InitializeItems();

#ifdef DEBUG_PRINT
    std::shared_lock<std::shared_mutex> lock(G::itemsMutex);
    for (auto& item: G::items) {
        G::logger.LogInfo("Item: " + item.displayName + ", name: " + item.name + ", index: " + std::to_string(item.index) + ", tier: " + std::to_string((int)item.tier) +
                          ", nameToken: " + item.nameToken + ", pickupToken: " + item.pickupToken +
                          ", descriptionToken: " + item.descriptionToken + ", loreToken: " + item.loreToken + ", tierName: " + item.tierName + ", isDroppable: " + std::to_string(item.isDroppable) +
                          ", canScrap: " + std::to_string(item.canScrap) + ", canRestack: " + std::to_string(item.canRestack) +
                          ", canRemove: " + std::to_string(item.canRemove) + ", isConsumed: " + std::to_string(item.isConsumed) +
                          ", hidden: " + std::to_string(item.hidden) + ", tags: " + std::to_string(item.tags.size()));
        for (auto& tag: item.tags) {
            G::logger.LogInfo("Tag: " + std::to_string(tag));
        }
        G::logger.LogInfo("-----------------------------------------------------");
    }
#endif // DEBUG_PRINT

    // Load and cache all pickup names
    int pickupCount = G::gameFunctions->LoadPickupNames();
    if (pickupCount > 0) {
        G::logger.LogInfo("Loaded %d pickup names", pickupCount);
    } else {
        G::logger.LogError("Failed to load pickup names");
    }

    MonoClass* layerMaskClass = G::g_monoRuntime->GetClass("UnityEngine.CoreModule", "UnityEngine", "LayerMask");
    if (layerMaskClass) {
        G::logger.LogInfo("Getting layer indices using LayerMask.NameToLayer...");

        MonoMethod* nameToLayerMethod = G::g_monoRuntime->GetMethod(layerMaskClass, "NameToLayer", 1);
        if (nameToLayerMethod) {
            auto getLayerByName = [&](const char* layerName) -> int {
                MonoString* layerNameStr = G::g_monoRuntime->CreateString(layerName);
                MonoObject* result = G::g_monoRuntime->InvokeMethod(nameToLayerMethod, nullptr, (void**)&layerNameStr);
                if (result) {
                    return *(int*)G::g_monoRuntime->m_mono_object_unbox(result);
                }
                return -1;
            };

            G::worldLayer = getLayerByName("World");
            G::playerBodyLayer = getLayerByName("PlayerBody");
            G::enemyBodyLayer = getLayerByName("EnemyBody");
            G::entityPreciseLayer = getLayerByName("EntityPrecise");
            G::ignoreRaycastLayer = getLayerByName("Ignore Raycast");

            G::logger.LogInfo("RoR2 Layers - World: %d, PlayerBody: %d, EnemyBody: %d, EntityPrecise: %d, IgnoreRaycast: %d",
                            G::worldLayer, G::playerBodyLayer, G::enemyBodyLayer, G::entityPreciseLayer, G::ignoreRaycastLayer);
        } else {
            G::logger.LogError("Failed to find LayerMask.NameToLayer method");
        }
    } else {
        G::logger.LogError("Failed to find LayerMask class");
    }


    G::showMenuControl->SetOnChange([](bool enabled) {
        if (enabled) {
            G::gameFunctions->Cursor_SetLockState(2); // CursorLockMode.Confined
            G::gameFunctions->Cursor_SetVisible(true);
        }
    });
    G::showMenuControl->SetHotkey(ImGuiKey_Insert);
    G::showMenuControl->SetSaveEnabledState(false);
    G::runningButtonControl->SetSaveEnabledState(false);

    G::hooksInitialized = true;

    ConfigManager::Initialize();

    G::gameFunctions->RoR2Application_SetModded(true);
    G::logger.LogInfo("Modded: " + std::to_string(G::gameFunctions->RoR2Application_IsModded()));
}

void Hooks::Unhook() {
    G::logger.LogInfo("Starting unhook process...");

    if (G::oWndProc && G::windowHwnd) {
        G::logger.LogInfo("Restoring window procedure...");
        G::oWndProc = (WNDPROC)SetWindowLongPtr(G::windowHwnd, GWLP_WNDPROC, (LONG_PTR)G::oWndProc);
    }

    if (G::initialized) {
        G::logger.LogInfo("Shutting down ImGui...");
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    if (G::mainRenderTargetView) {
        G::mainRenderTargetView->Release();
        G::mainRenderTargetView = nullptr;
    }
    if (G::pContext) {
        G::pContext->Release();
        G::pContext = nullptr;
    }
    if (G::pDevice) {
        G::pDevice->Release();
        G::pDevice = nullptr;
    }

    G::logger.LogInfo("Disabling all hooks...");
    for (auto& target: hookTargets) {
        MH_STATUS disable_status = MH_DisableHook(target.second);
        if (disable_status == MH_OK) {
            G::logger.LogInfo("Hook successfully disabled for " + target.first);
        } else {
            G::logger.LogError("Hook disabling failed for " + target.first + ", error: " +
                             MH_StatusToString(disable_status) + " (code: " + MH_StatusToString(disable_status) + ")");
        }
    }
    G::logger.LogInfo("All hooks disabled");

    G::logger.LogInfo("Removing all hooks...");
    for (auto& target: hookTargets) {
        MH_STATUS remove_status = MH_RemoveHook(target.second);
        if (remove_status == MH_OK) {
            G::logger.LogInfo("Hook successfully removed for " + target.first);
        } else {
            G::logger.LogError("Hook removal failed for " + target.first + ", error: " +
                             MH_StatusToString(remove_status) + " (code: " + MH_StatusToString(remove_status) + ")");
        }
    }
    G::logger.LogInfo("All hooks removed");

    kiero::shutdown();
    G::logger.LogInfo("Kiero successfully shut down");

    MH_STATUS uninit_status = MH_Uninitialize();
    if (uninit_status == MH_OK) {
        G::logger.LogInfo("MinHook successfully uninitialized");
    } else {
        G::logger.LogError(std::string("MinHook uninitialization failed, error: ") +
                         MH_StatusToString(uninit_status) + " (code: " + MH_StatusToString(uninit_status) + ")");
    }

    delete G::g_monoRuntime;
    delete G::gameFunctions;
    delete G::runningButtonControl;

    delete G::localPlayer;
    delete G::espModule;
}


void Hooks::hkRoR2RoR2ApplicationUpdate(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2RoR2ApplicationUpdate"]);
    originalFunc(instance);

    if (!G::hooksInitialized) {
        return;
    }

    std::unique_lock<std::mutex> lock(G::queuedActionsMutex);
    for (; !G::queuedActions.empty(); G::queuedActions.pop()) {
        auto action = G::queuedActions.front();
        action();
    }

    G::espModule->OnGameUpdate();
}

bool Hooks::hkRewiredPlayerGetButtonDown(void* instance, int key) {
    if (G::showMenuControl->IsEnabled())
        return false;

    static auto originalFunc = reinterpret_cast<bool(*)(void*, int)>(hooks["RewiredPlayerGetButtonDown"]);
    return originalFunc(instance, key);
}

void Hooks::hkRoR2RoR2ApplicationUpdateCursorState(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2RoR2ApplicationUpdateCursorState"]);
    if (!G::showMenuControl->IsEnabled()) {
        originalFunc(instance);
    }
}

void Hooks::hkRoR2MPEventSystemManagerUpdate(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2MPEventSystemManagerUpdate"]);
    if (!G::showMenuControl->IsEnabled()) {
        originalFunc(instance);
    }
}

void Hooks::hkUnityEngineCursorset_lockState(void* instance, int lockState) {
    static auto originalFunc = reinterpret_cast<void(*)(void*, int)>(hooks["UnityEngineCursorset_lockState"]);
    if (G::showMenuControl->IsEnabled()) {
        lockState = 2; // CursorLockMode.Confined
    }
    originalFunc(instance, lockState);
}

void Hooks::hkUnityEngineCursorset_visible(void* instance, bool visible) {
    static auto originalFunc = reinterpret_cast<void(*)(void*, bool)>(hooks["UnityEngineCursorset_visible"]);
    if (G::showMenuControl->IsEnabled()) {
        visible = true;
    }
    originalFunc(instance, visible);
}

void Hooks::hkRoR2LocalUserRebuildControlChain(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2LocalUserRebuildControlChain"]);
    originalFunc(instance);

    if (!G::hooksInitialized) {
        return;
    }

    G::localPlayer->OnLocalUserUpdate(instance);
}

void Hooks::hkRoR2InventoryHandleInventoryChanged(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2InventoryHandleInventoryChanged"]);
    originalFunc(instance);

    if (!G::hooksInitialized) {
        return;
    }

    G::localPlayer->OnInventoryChanged(instance);
}

void Hooks::hkRoR2InventoryRemoveItem(void* instance, int itemIndex, int count) {
    static auto originalFunc = reinterpret_cast<void(*)(void*, int, int)>(hooks["RoR2InventoryRemoveItem"]);
    if (!G::hooksInitialized || !G::localPlayer) {
        originalFunc(instance, itemIndex, count);
        return;
    }

    LocalUser* localUser = G::localPlayer->GetLocalUser();
    if (!localUser || !localUser->cachedBody_backing ||
        instance != localUser->cachedBody_backing->inventory_backing) {
        originalFunc(instance, itemIndex, count);
        return;
    }

    // Don't remove item if it's protected
    auto& itemControls = G::localPlayer->GetItemControls();
    if (itemControls.count(itemIndex) > 0 && itemControls[itemIndex]->IsEnabled()) {
        return;
    }

    originalFunc(instance, itemIndex, count);
}

int Hooks::hkRoR2ItemStealControllerStolenInventoryInfoStealItem(void* instance, int itemIndex, int maxStackToSteal, bool useOrbOverride) {
    static auto originalFunc = reinterpret_cast<int(*)(void*, int, int, bool)>(hooks["RoR2ItemStealControllerStolenInventoryInfoStealItem"]);

    if (!G::hooksInitialized || !G::localPlayer) {
        return originalFunc(instance, itemIndex, maxStackToSteal, useOrbOverride);
    }

    // Check if this item is protected
    auto& itemControls = G::localPlayer->GetItemControls();
    if (itemControls.count(itemIndex) > 0 && itemControls[itemIndex]->IsEnabled()) {
        // Return 0 to indicate no items were stolen
        return 0;
    }

    return originalFunc(instance, itemIndex, maxStackToSteal, useOrbOverride);
}

void Hooks::hkRoR2SteamworksServerManagerTagsStringUpdated(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2SteamworksServerManagerTagsStringUpdated"]);
    originalFunc(instance);

    ServerManagerBase* serverManager_ptr = (ServerManagerBase*)instance;
    if (serverManager_ptr && serverManager_ptr->tags) {
        MonoList list((MonoObject*)serverManager_ptr->tags);
        list.AddItem("ror2mod");
    }
}

void Hooks::hkRoR2TeleporterInteractionAwake(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2TeleporterInteractionAwake"]);
    originalFunc(instance);

    if (!G::hooksInitialized) {
        return;
    }

    G::espModule->OnTeleporterAwake(instance);
    G::worldModule->OnTeleporterInteractionAwake(instance);
}

void Hooks::hkRoR2TeleporterInteractionFixedUpdate(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2TeleporterInteractionFixedUpdate"]);
    originalFunc(instance);

    if (!G::hooksInitialized) {
        return;
    }

    G::worldModule->OnTeleporterInteractionFixedUpdate(instance);
}

void Hooks::hkRoR2CharacterBodyStart(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2CharacterBodyStart"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnCharacterBodySpawned(instance);
}

void Hooks::hkRoR2CharacterBodyOnDestroy(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2CharacterBodyOnDestroy"]);

    if (G::hooksInitialized) {
        G::espModule->OnCharacterBodyDestroyed(instance);
    }

    originalFunc(instance);
}

void Hooks::hkRoR2HuntressTrackerStart(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2HuntressTrackerStart"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::localPlayer->OnHuntressTrackerStart(instance);
}

void Hooks::hkRoR2BullseyeSearchRefreshCandidates(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2BullseyeSearchRefreshCandidates"]);

    if (!G::hooksInitialized) {
        originalFunc(instance);
        return;
    }

    // Check if this is the cached HuntressTracker's search
    HuntressTracker* huntressTracker = G::localPlayer ? G::localPlayer->GetCachedHuntressTracker() : nullptr;
    if (huntressTracker && huntressTracker->search == instance) {
        BullseyeSearch* search = static_cast<BullseyeSearch*>(instance);
        uint8_t originalTeamMask = search->teamMaskFilter;

        // Apply enemy-only targeting if enabled
        if (G::localPlayer->GetHuntressEnemyOnlyTargetingControl() &&
            G::localPlayer->GetHuntressEnemyOnlyTargetingControl()->IsEnabled()) {
            // Only target Monster, Lunar, and Void teams (actual enemies)
            // Exclude Neutral (breakables), Player (allies)
            static const uint8_t enemyOnlyMask = (1 << static_cast<int>(TeamIndex_Value::Monster)) |
                                                (1 << static_cast<int>(TeamIndex_Value::Lunar)) |
                                                (1 << static_cast<int>(TeamIndex_Value::Void));
            search->teamMaskFilter = enemyOnlyMask;
        }

        originalFunc(instance);

        // Restore the original team mask
        search->teamMaskFilter = originalTeamMask;
        return;
    }

    originalFunc(instance);
}

void* Hooks::hkRoR2BullseyeSearchGetResults(void* instance) {
    static auto originalFunc = reinterpret_cast<void*(*)(void*)>(hooks["RoR2BullseyeSearchGetResults"]);

    if (!G::hooksInitialized) {
        return originalFunc(instance);
    }

    // If huntress wall penetration is enabled and this is the cached huntress tracker's search
    if (G::localPlayer && G::localPlayer->GetHuntressWallPenetrationControl() &&
        G::localPlayer->GetHuntressWallPenetrationControl()->IsEnabled()) {

        // If this matches the cached HuntressTracker's search, we will temporarily
        // disable line-of-sight filtering
        HuntressTracker* huntressTracker = G::localPlayer->GetCachedHuntressTracker();
        if (huntressTracker && huntressTracker->search == instance) {
            BullseyeSearch* search = static_cast<BullseyeSearch*>(instance);
            bool originalFilterByLoS = search->filterByLoS;
            search->filterByLoS = false;

            void* result = originalFunc(instance);

            // Restore the original filterByLoS value
            search->filterByLoS = originalFilterByLoS;

            return result;
        }
    }

    return originalFunc(instance);
}

void Hooks::hkRoR2PurchaseInteractionStart(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2PurchaseInteractionStart"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnPurchaseInteractionSpawned(instance);
}

void Hooks::hkRoR2BarrelInteractionStart(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2BarrelInteractionStart"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnBarrelInteractionSpawned(instance);
}

void Hooks::hkRoR2GenericPickupControllerStart(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2GenericPickupControllerStart"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnGenericPickupControllerSpawned(instance);
}

void Hooks::hkRoR2TimedChestControllerOnEnable(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2TimedChestControllerOnEnable"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnTimedChestControllerSpawned(instance);
}

void Hooks::hkRoR2TimedChestControllerOnDisable(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2TimedChestControllerOnDisable"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnTimedChestControllerDespawned(instance);
}

void Hooks::hkRoR2GenericInteractionOnEnable(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2GenericInteractionOnEnable"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnGenericInteractionSpawned(instance);
}

void Hooks::hkRoR2PickupPickerControllerAwake(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2PickupPickerControllerAwake"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnPickupPickerControllerSpawned(instance);
}

void Hooks::hkRoR2ScrapperControllerStart(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2ScrapperControllerStart"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnScrapperControllerSpawned(instance);
}

void Hooks::hkRoR2RunAdvanceStage(void* instance, void* nextScene) {
    static auto originalFunc = reinterpret_cast<void(*)(void*, void*)>(hooks["RoR2RunAdvanceStage"]);

    // Clear ESP data before advancing to the next stage
    if (G::hooksInitialized) {
        G::espModule->OnStageStart(instance);
    }

    originalFunc(instance, nextScene);
}

void Hooks::hkRoR2RunAwake(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2RunAwake"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::runInstance = instance;
}

void Hooks::hkRoR2ChestBehaviorStart(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2ChestBehaviorStart"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnChestBehaviorSpawned(instance);
}

void Hooks::hkRoR2ShopTerminalBehaviorStart(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2ShopTerminalBehaviorStart"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnShopTerminalBehaviorSpawned(instance);
}

void Hooks::hkRoR2PressurePlateControllerStart(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2PressurePlateControllerStart"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::espModule->OnPressurePlateControllerSpawned(instance);
}

void Hooks::hkRoR2HoldoutZoneControllerUpdate(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2HoldoutZoneControllerUpdate"]);
    originalFunc(instance);

    if (!G::hooksInitialized) return;

    G::worldModule->OnHoldoutZoneControllerUpdate(instance);
}

void Hooks::hkRoR2PortalSpawnerStart(void* instance) {
    static auto originalFunc = reinterpret_cast<void(*)(void*)>(hooks["RoR2PortalSpawnerStart"]);

    originalFunc(instance);

    if (!G::hooksInitialized) return;

    PortalSpawner* portalSpawner = (PortalSpawner*)instance;
    if (!portalSpawner) return;

    if (G::worldModule->GetForceAllPortalsControl()->IsEnabled()) {
        portalSpawner->willSpawn = true;
    }
}

int Hooks::hkRoR2TimedChestControllerGetInteractability(void* instance, void* activator) {
    static auto originalFunc = reinterpret_cast<int(*)(void*, void*)>(hooks["RoR2TimedChestControllerGetInteractability"]);
    int result = originalFunc(instance, activator);

    if (!G::hooksInitialized) return result;

    if (G::worldModule->GetOpenExpiredTimedChestsControl()->IsEnabled() &&
        result == (int)Interactability_Value::ConditionsNotMet) {

        TimedChestController* tcc = (TimedChestController*)instance;
        // Only override if it's expired (not purchased)
        if (!tcc->purchased) {
            return (int)Interactability_Value::Available;
        }
    }

    return result;
}

LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    if (G::showMenuControl->IsEnabled()) {
        return TRUE;
    }
    return CallWindowProc(G::oWndProc, hWnd, uMsg, wParam, lParam);
}

long __stdcall Hooks::hkPresent11(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    if (!G::running) {
        return G::oPresent(pSwapChain, SyncInterval, Flags);
    }

    if (!G::initialized) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&G::pDevice))) {
            ImGui::CreateContext();

            ImGuiIO& io = ImGui::GetIO();
            FontManager::InitializeFonts(io.Fonts);

            G::pDevice->GetImmediateContext(&G::pContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            G::windowHwnd = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer = NULL;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            G::pDevice->CreateRenderTargetView(pBackBuffer, NULL, &G::mainRenderTargetView);
            pBackBuffer->Release();

            ImGui_ImplWin32_Init(G::windowHwnd);
            ImGui_ImplDX11_Init(G::pDevice, G::pContext);
            ImGui_ImplDX11_CreateDeviceObjects();

            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

            G::oWndProc = (WNDPROC)SetWindowLongPtr(G::windowHwnd, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
            G::initialized = true;
        }
        else {
            return G::oPresent(pSwapChain, SyncInterval, Flags);
        }
    }

    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();

    G::showMenuControl->Update();
    G::runningButtonControl->Update();

    G::localPlayer->Update();
    G::espModule->Update();

    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = G::showMenuControl->IsEnabled();

    if (const auto& displaySize = ImGui::GetIO().DisplaySize; displaySize.x > 0.0f && displaySize.y > 0.0f) {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos;
        window_pos.x = work_pos.x + PAD;
        window_pos.y = work_pos.y + PAD;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
        window_flags |= ImGuiWindowFlags_NoMove;
        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGui::Begin("Watermark", 0, window_flags);
        if (G::hooksInitialized) {
            ImGui::Text("RoR2Mod V" VERSION_STRING);
        }
        else {
            ImGui::Text("RoR2Mod V" VERSION_STRING " - NOT READY");
        }
        ImGui::End();
    }

    G::espModule->OnFrameRender();

    if (G::showMenuControl->IsEnabled()) {
        DrawMenu();
    }

    ImGui::Render();

    G::pContext->OMSetRenderTargets(1, &G::mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return G::oPresent(pSwapChain, SyncInterval, Flags);
}