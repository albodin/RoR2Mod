#pragma once
#include "ModuleBase.h"
#include "menu/InputControls.h"
#include "game/GameStructs.h"
#include "utils/ModStructs.h"
#include <map>
#include <vector>
#include <shared_mutex>
#include <queue>
#include <mutex>

class PlayerModule : public ModuleBase {
private:
    ToggleControl* godModeControl;
    FloatControl* baseMoveSpeedControl;
    FloatControl* baseDamageControl;
    FloatControl* baseAttackSpeedControl;
    FloatControl* baseCritControl;
    IntControl* baseJumpCountControl;
    ToggleButtonControl* teleportToCursorControl;

    FloatControl* huntressRangeControl;
    FloatControl* huntressFOVControl;
    ToggleControl* huntressWallPenetrationControl;
    ToggleControl* huntressEnemyOnlyTargetingControl;
    ToggleControl* huntressTargetingModeOverrideControl;
    ComboControl* huntressTargetingModeControl;

    ToggleControl* blockPhysicsEffectsControl;
    ToggleControl* blockPullsControl;

    ToggleControl* flightControl;

    std::mutex queuedGiveItemsMutex;
    std::queue<std::tuple<int, int>> queuedGiveItems;
    std::shared_mutex itemsMutex;
    std::vector<RoR2Item> items;
    std::vector<int> itemStacks;

    std::map<int, IntControl*> itemControls;
    LocalUser* localUser_cached;

    // Huntress tracker cache management
    std::map<CharacterBody*, HuntressTracker*> playerHuntressTrackers;
    std::mutex trackerCacheMutex;

    Vector3 playerPosition;
    bool isProvidingFlight;

    void SortItemsByName();

public:
    PlayerModule();
    ~PlayerModule() override;

    void Initialize() override;
    void Update() override;
    void DrawUI() override;

    void OnLocalUserUpdate(void* localUser);
    void OnInventoryChanged(void* inventory);
    void InitializeItems();
    void InitializeAllItemControls();

    ToggleControl* GetGodModeControl() { return godModeControl; }
    FloatControl* GetBaseMoveSpeedControl() { return baseMoveSpeedControl; }
    FloatControl* GetBaseDamageControl() { return baseDamageControl; }
    FloatControl* GetBaseAttackSpeedControl() { return baseAttackSpeedControl; }
    FloatControl* GetBaseCritControl() { return baseCritControl; }
    IntControl* GetBaseJumpCountControl() { return baseJumpCountControl; }
    FloatControl* GetHuntressRangeControl() { return huntressRangeControl; }
    FloatControl* GetHuntressFOVControl() { return huntressFOVControl; }
    ToggleControl* GetHuntressWallPenetrationControl() { return huntressWallPenetrationControl; }
    ToggleControl* GetHuntressEnemyOnlyTargetingControl() { return huntressEnemyOnlyTargetingControl; }
    ToggleControl* GetHuntressTargetingModeOverrideControl() { return huntressTargetingModeOverrideControl; }
    ComboControl* GetHuntressTargetingModeControl() { return huntressTargetingModeControl; }
    ToggleControl* GetBlockPhysicsEffectsControl() { return blockPhysicsEffectsControl; }
    ToggleControl* GetBlockPullsControl() { return blockPullsControl; }
    ToggleControl* GetFlightControl() { return flightControl; }
    HuntressTracker* GetCurrentLocalTracker();
    std::map<int, IntControl*>& GetItemControls() { return itemControls; }

    void SetItemCount(int itemIndex, int count);
    int GetItemCount(int itemIndex);

    void DrawItemInputs(ItemTier_Value tier);

    Vector3 GetPlayerPosition() { return playerPosition; }
    LocalUser* GetLocalUser() { return localUser_cached; }
    CharacterBody* GetLocalPlayerBody() {
        return localUser_cached ? localUser_cached->cachedBody_backing : nullptr;
    }

    void OnHuntressTrackerStart(void* huntressTracker);
    void OnCharacterBodyDestroyed(void* characterBody);
};