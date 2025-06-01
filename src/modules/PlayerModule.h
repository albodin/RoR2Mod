#pragma once
#include "ModuleBase.h"
#include "menu/InputControls.h"
#include "game/GameStructs.h"
#include <map>
#include <vector>
#include <shared_mutex>
#include <queue>

class PlayerModule : public ModuleBase {
private:
    ToggleControl* godModeControl;
    FloatControl* baseMoveSpeedControl;
    FloatControl* baseDamageControl;
    FloatControl* baseAttackSpeedControl;
    FloatControl* baseCritControl;
    IntControl* baseJumpCountControl;
    ToggleButtonControl* teleportToCursorControl;

    std::mutex queuedGiveItemsMutex;
    std::queue<std::tuple<int, int>> queuedGiveItems;
    std::shared_mutex itemsMutex;
    std::vector<RoR2Item> items;
    std::vector<int> itemStacks;

    std::map<int, IntControl*> itemControls;
    void* localInventory_cached;
    LocalUser* localUser_cached;

    Vector3 playerPosition;

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
    std::map<int, IntControl*>& GetItemControls() { return itemControls; }

    void SetCachedInventory(void* inventory) { localInventory_cached = inventory; }
    void* GetCachedInventory() { return localInventory_cached; }

    void SetItemCount(int itemIndex, int count);
    int GetItemCount(int itemIndex);

    void DrawItemInputs(ItemTier_Value tier);

    Vector3 GetPlayerPosition() { return playerPosition; }
    CharacterBody* GetLocalPlayerBody() {
        return localUser_cached ? localUser_cached->cachedBody_backing : nullptr;
    }
};