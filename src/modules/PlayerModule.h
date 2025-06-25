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
#include <memory>

class PlayerModule : public ModuleBase {
private:
    std::unique_ptr<ToggleControl> godModeControl;
    std::unique_ptr<FloatControl> baseMoveSpeedControl;
    std::unique_ptr<FloatControl> baseDamageControl;
    std::unique_ptr<FloatControl> baseAttackSpeedControl;
    std::unique_ptr<FloatControl> baseCritControl;
    std::unique_ptr<IntControl> baseJumpCountControl;
    std::unique_ptr<ToggleButtonControl> teleportToCursorControl;

    std::unique_ptr<FloatControl> huntressRangeControl;
    std::unique_ptr<FloatControl> huntressFOVControl;
    std::unique_ptr<ToggleControl> huntressWallPenetrationControl;
    std::unique_ptr<ToggleControl> huntressEnemyOnlyTargetingControl;
    std::unique_ptr<ToggleControl> huntressTargetingModeOverrideControl;
    std::unique_ptr<ComboControl> huntressTargetingModeControl;

    std::unique_ptr<ToggleControl> blockPhysicsEffectsControl;
    std::unique_ptr<ToggleControl> blockPullsControl;

    std::unique_ptr<ToggleControl> flightControl;

    std::mutex queuedGiveItemsMutex;
    std::queue<std::tuple<int, int>> queuedGiveItems;
    std::shared_mutex itemsMutex;
    std::vector<RoR2Item> items;
    std::vector<int> itemStacks;

    std::map<int, std::unique_ptr<IntControl>> itemControls;
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

    ToggleControl* GetGodModeControl() { return godModeControl.get(); }
    FloatControl* GetBaseMoveSpeedControl() { return baseMoveSpeedControl.get(); }
    FloatControl* GetBaseDamageControl() { return baseDamageControl.get(); }
    FloatControl* GetBaseAttackSpeedControl() { return baseAttackSpeedControl.get(); }
    FloatControl* GetBaseCritControl() { return baseCritControl.get(); }
    IntControl* GetBaseJumpCountControl() { return baseJumpCountControl.get(); }
    FloatControl* GetHuntressRangeControl() { return huntressRangeControl.get(); }
    FloatControl* GetHuntressFOVControl() { return huntressFOVControl.get(); }
    ToggleControl* GetHuntressWallPenetrationControl() { return huntressWallPenetrationControl.get(); }
    ToggleControl* GetHuntressEnemyOnlyTargetingControl() { return huntressEnemyOnlyTargetingControl.get(); }
    ToggleControl* GetHuntressTargetingModeOverrideControl() { return huntressTargetingModeOverrideControl.get(); }
    ComboControl* GetHuntressTargetingModeControl() { return huntressTargetingModeControl.get(); }
    ToggleControl* GetBlockPhysicsEffectsControl() { return blockPhysicsEffectsControl.get(); }
    ToggleControl* GetBlockPullsControl() { return blockPullsControl.get(); }
    ToggleControl* GetFlightControl() { return flightControl.get(); }
    HuntressTracker* GetCurrentLocalTracker();
    std::map<int, std::unique_ptr<IntControl>>& GetItemControls() { return itemControls; }

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