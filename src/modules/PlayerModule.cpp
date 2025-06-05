#include "PlayerModule.h"
#include "globals/globals.h"
#include "hooks/hooks.h"
#include "imgui.h"
#include "config/ConfigManager.h"

PlayerModule::PlayerModule() : ModuleBase(),
    godModeControl(nullptr),
    baseMoveSpeedControl(nullptr),
    baseDamageControl(nullptr),
    baseAttackSpeedControl(nullptr),
    baseCritControl(nullptr),
    baseJumpCountControl(nullptr),
    teleportToCursorControl(nullptr),
    huntressRangeControl(nullptr),
    huntressFOVControl(nullptr),
    huntressEnemyOnlyTargetingControl(nullptr),
    localInventory_cached(nullptr),
    localUser_cached(nullptr),
    cachedHuntressTracker(nullptr) {
    Initialize();
}

PlayerModule::~PlayerModule() {
    delete godModeControl;
    delete baseMoveSpeedControl;
    delete baseDamageControl;
    delete baseAttackSpeedControl;
    delete baseCritControl;
    delete baseJumpCountControl;
    delete teleportToCursorControl;
    delete huntressRangeControl;
    delete huntressFOVControl;
    delete huntressEnemyOnlyTargetingControl;

    for (auto& [index, control] : itemControls) {
        delete control;
    }
    itemControls.clear();
}

void PlayerModule::SortItemsByName() {
    std::sort(items.begin(), items.end(), [](const RoR2Item& a, const RoR2Item& b) {
        if (a.tier != b.tier) {
            return a.tier < b.tier;
        }
        return a.displayName < b.displayName;
    });
}

void PlayerModule::Initialize() {
    godModeControl = new ToggleControl("Godmode", "godMode", false);
    baseMoveSpeedControl = new FloatControl("Base Move Speed", "baseMoveSpeed", 10.0f);
    baseDamageControl = new FloatControl("Base Damage", "baseDamage", 10.0f, 0.0f, FLT_MAX, 10.0f);
    baseAttackSpeedControl = new FloatControl("Base Attack Speed", "baseAttackSpeed", 10.0f, 0.0f, FLT_MAX, 10.0f);
    baseCritControl = new FloatControl("Base Crit", "baseCrit", 10.0f, 0.0f, FLT_MAX, 10.0f);
    baseJumpCountControl = new IntControl("Base Jump Count", "baseJumpCount", 1, 0, INT_MAX, 10);
    teleportToCursorControl = new ToggleButtonControl("Teleport to Cursor", "teleportToCursor", "Teleport", false);

    huntressRangeControl = new FloatControl("Huntress Range", "huntressRange", 20.0f, 0.0f, 1000.0f, 5.0f);
    huntressFOVControl = new FloatControl("Huntress FOV", "huntressFOV", 20.0f, 0.0f, 180.0f, 5.0f);
    huntressWallPenetrationControl = new ToggleControl("Huntress Wall Penetration", "huntressWallPenetration", false);
    huntressEnemyOnlyTargetingControl = new ToggleControl("Huntress Ignore Breakables", "huntressEnemyOnlyTargeting", false);

    teleportToCursorControl->SetOnAction([this]() {
        if (localUser_cached && localUser_cached->cachedBody_backing && localUser_cached->_cameraRigController) {
            G::gameFunctions->TeleportHelper_TeleportBody(localUser_cached->cachedBody_backing,
                                                          localUser_cached->_cameraRigController->crosshairWorldPosition_backing);
        }
    });
}

void PlayerModule::Update() {
    godModeControl->Update();
    baseMoveSpeedControl->Update();
    baseDamageControl->Update();
    baseAttackSpeedControl->Update();
    baseCritControl->Update();
    baseJumpCountControl->Update();
    teleportToCursorControl->Update();
    huntressRangeControl->Update();
    huntressFOVControl->Update();
    huntressEnemyOnlyTargetingControl->Update();

    for (auto& [index, control] : itemControls) {
        control->Update();
    }
}

void PlayerModule::DrawUI() {
    godModeControl->Draw();
    baseMoveSpeedControl->Draw();
    baseDamageControl->Draw();
    baseAttackSpeedControl->Draw();
    baseCritControl->Draw();
    baseJumpCountControl->Draw();
    teleportToCursorControl->Draw();

    if (ImGui::CollapsingHeader("Huntress Settings")) {
        huntressRangeControl->Draw();
        huntressFOVControl->Draw();
        huntressWallPenetrationControl->Draw();
        huntressEnemyOnlyTargetingControl->Draw();
    }

    if (ImGui::CollapsingHeader("Items")) {
        std::shared_lock<std::shared_mutex> lock(G::itemsMutex);
        if (ImGui::CollapsingHeader("Tier1")) {
            DrawItemInputs(ItemTier_Value::Tier1);
        }
        if (ImGui::CollapsingHeader("Tier2")) {
            DrawItemInputs(ItemTier_Value::Tier2);
        }
        if (ImGui::CollapsingHeader("Tier3")) {
            DrawItemInputs(ItemTier_Value::Tier3);
        }
        if (ImGui::CollapsingHeader("Lunar")) {
            DrawItemInputs(ItemTier_Value::Lunar);
        }
    }
}

void PlayerModule::OnLocalUserUpdate(void* localUser) {
    LocalUser* localUser_ptr = (LocalUser*)localUser;
    if (!localUser_ptr->cachedMaster_backing || !localUser_ptr->cachedBody_backing ||
        !localUser_ptr->cachedBody_backing->healthComponent_backing) {
        return;
    }
    localUser_cached = localUser_ptr;

    Hooks::Transform_get_position_Injected(localUser_ptr->cachedBody_backing->transform, &playerPosition);

    localUser_ptr->cachedMaster_backing->godMode = godModeControl->IsEnabled();
    localUser_ptr->cachedBody_backing->healthComponent_backing->godMode_backing = godModeControl->IsEnabled();

    if (baseMoveSpeedControl->IsEnabled()) {
        localUser_ptr->cachedBody_backing->baseMoveSpeed = baseMoveSpeedControl->GetValue();
    }
    if (baseDamageControl->IsEnabled()) {
        localUser_ptr->cachedBody_backing->baseDamage = baseDamageControl->GetValue();
    }
    if (baseAttackSpeedControl->IsEnabled()) {
        localUser_ptr->cachedBody_backing->baseAttackSpeed = baseAttackSpeedControl->GetValue();
    }
    if (baseCritControl->IsEnabled()) {
        localUser_ptr->cachedBody_backing->baseCrit = baseCritControl->GetValue();
    }
    if (baseJumpCountControl->IsEnabled()) {
        localUser_ptr->cachedBody_backing->baseJumpCount = baseJumpCountControl->GetValue();
    }

    if (cachedHuntressTracker) {
        if (huntressRangeControl->IsEnabled()) {
            cachedHuntressTracker->maxTrackingDistance = huntressRangeControl->GetValue();
        }

        if (huntressFOVControl->IsEnabled()) {
            cachedHuntressTracker->maxTrackingAngle = huntressFOVControl->GetValue();
        }
    }

    if (!localUser_ptr->cachedBody_backing->inventory_backing) {
        return;
    }

    localInventory_cached = localUser_ptr->cachedBody_backing->inventory_backing;

    // Process any queued item changes
    std::unique_lock<std::mutex> lock(queuedGiveItemsMutex);
    int* arrayData = (int*)(localUser_ptr->cachedBody_backing->inventory_backing->itemStacks + 8); // Adjusted offset of array with header
    for (; !queuedGiveItems.empty(); queuedGiveItems.pop()) {
        auto item = queuedGiveItems.front();
        int itemIndex = std::get<0>(item);
        int count = std::get<1>(item) - arrayData[itemIndex];
        G::gameFunctions->Inventory_GiveItem(localUser_ptr->cachedBody_backing->inventory_backing, itemIndex, count);
    }
}

void PlayerModule::OnInventoryChanged(void* inventory) {
    Inventory* inventory_ptr = (Inventory*)inventory;
    if (!inventory_ptr || !inventory_ptr->itemStacks || inventory != localInventory_cached) {
        return;
    }

    std::unique_lock<std::shared_mutex> lock(G::itemsMutex);
    int* arrayData = (int*)(inventory_ptr->itemStacks + 8); // Adjusted offset of array with header
    for (int i = 0; i < itemStacks.size(); i++) {
        itemStacks[i] = arrayData[i];
        if (itemControls.count(i) > 0 && itemControls[i]->GetValue() != arrayData[i]) {
            itemControls[i]->SetValue(arrayData[i]);
        }
    }
}

void PlayerModule::InitializeItems() {
    int itemCount = -1;
    do {
        itemCount = G::gameFunctions->LoadItems();
        if (itemCount == -1) {
            Sleep(2000);
        }
    } while (itemCount == -1);
    G::logger.LogInfo("Items loaded successfully");

    {
        std::unique_lock<std::shared_mutex> lock(itemsMutex);
        std::shared_lock<std::shared_mutex> lockG(G::itemsMutex);
        items = G::items;
        SortItemsByName();
        itemStacks.resize(itemCount);
        InitializeAllItemControls();
    }
}

void PlayerModule::InitializeAllItemControls() {
    G::logger.LogInfo("Initializing controls for all %zu items", items.size());

    for (const auto& item : items) {
        int index = item.index;
        int currentCount = (index < itemStacks.size()) ? itemStacks[index] : 0;

        auto control = new IntControl(item.displayName, "item_" + std::to_string(index),
                                     currentCount, 0, INT_MAX, 1, false, false);
        control->SetValueProtected(true);

        control->SetOnChange([this, index](int newValue) {
            if (index < itemStacks.size()) {
                itemStacks[index] = newValue;
            }
            std::unique_lock<std::mutex> lock(queuedGiveItemsMutex);
            queuedGiveItems.push(std::make_tuple(index, newValue));
        });

        itemControls[index] = control;
    }

    G::logger.LogInfo("Initialized %zu item controls", itemControls.size());
}

void PlayerModule::SetItemCount(int itemIndex, int count) {
    if (itemControls.count(itemIndex) > 0) {
        itemControls[itemIndex]->SetValue(count);
    }
}

int PlayerModule::GetItemCount(int itemIndex) {
    if (itemControls.count(itemIndex) > 0) {
        return itemControls[itemIndex]->GetValue();
    }
    return 0;
}

void PlayerModule::DrawItemInputs(ItemTier_Value tier) {
    std::shared_lock<std::shared_mutex> lock(itemsMutex);

    for (auto& item : items) {
        if (item.tier != tier) continue;
        int index = item.index;

        if (itemControls.count(index) > 0) {
            if (itemControls[index]->GetValue() != itemStacks[index]) {
                itemControls[index]->SetValue(itemStacks[index]);
            }
            itemControls[index]->Draw();
        }
    }
}

void PlayerModule::OnHuntressTrackerStart(void* huntressTracker) {
    if (!huntressTracker) return;
    HuntressTracker* tracker = (HuntressTracker*)huntressTracker;
    cachedHuntressTracker = tracker;
}