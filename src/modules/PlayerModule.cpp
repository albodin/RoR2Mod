#include "PlayerModule.h"
#include "globals/globals.h"
#include "hooks/hooks.h"
#include "imgui.h"
#include "config/ConfigManager.h"

PlayerModule::PlayerModule() : ModuleBase(),
    localUser_cached(nullptr),
    isProvidingFlight(false),
    isMoneyConversionActive(false) {
    Initialize();
}

PlayerModule::~PlayerModule() {
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
    godModeControl = std::make_unique<ToggleControl>("Godmode", "godMode", false);
    baseMoveSpeedControl = std::make_unique<FloatControl>("Base Move Speed", "baseMoveSpeed", 7.0f, 0.0f, FLT_MAX, 1.0f, false, false, true);
    baseDamageControl = std::make_unique<FloatControl>("Base Damage", "baseDamage", 12.0f, 0.0f, FLT_MAX, 1.0f, false, false, true);
    baseAttackSpeedControl = std::make_unique<FloatControl>("Base Attack Speed", "baseAttackSpeed", 1.0f, 0.0f, FLT_MAX, 0.1f, false, false, true);
    baseCritControl = std::make_unique<FloatControl>("Base Crit", "baseCrit", 1.0f, 0.0f, FLT_MAX, 1.0f, false, false, true);
    baseJumpCountControl = std::make_unique<IntControl>("Base Jump Count", "baseJumpCount", 1, 0, INT_MAX, 1, false, false);
    teleportToCursorControl = std::make_unique<ToggleButtonControl>("Teleport to Cursor", "teleportToCursor", "Teleport", false);

    huntressRangeControl = std::make_unique<FloatControl>("Huntress Range", "huntressRange", 20.0f, 0.0f, 1000.0f, 5.0f);
    huntressFOVControl = std::make_unique<FloatControl>("Huntress FOV", "huntressFOV", 20.0f, 0.0f, 180.0f, 5.0f);
    huntressWallPenetrationControl = std::make_unique<ToggleControl>("Huntress Wall Penetration", "huntressWallPenetration", false);
    huntressEnemyOnlyTargetingControl = std::make_unique<ToggleControl>("Huntress Ignore Breakables", "huntressEnemyOnlyTargeting", false);

    huntressTargetingModeOverrideControl = std::make_unique<ToggleControl>("Override Huntress Targeting Mode", "huntressTargetingModeOverride", false);
    huntressTargetingModeControl = std::make_unique<ComboControl>("Targeting Mode", "huntressTargetingMode",
        std::vector<std::string>{"None", "Distance", "Angle", "Distance + Angle"},
        std::vector<int>{0, 1, 2, 3}, 3);

    blockPhysicsEffectsControl = std::make_unique<ToggleControl>("Block Enemy Forces", "blockPhysicsEffects", false);
    blockPullsControl = std::make_unique<ToggleControl>("Block Displacements", "blockPulls", true);

    flightControl = std::make_unique<ToggleControl>("Flight", "flight", false);

    deployableCapControl = std::make_unique<IntControl>("Deployable Cap", "deployable_cap", 2, 1, INT_MAX, 1, false, false);

    primarySkillCooldownControl = std::make_unique<FloatControl>("Primary Skill Cooldown", "primary_skill_cooldown", 0.0f, 0.0f, 60.0f, 0.1f, false, false, true);
    secondarySkillCooldownControl = std::make_unique<FloatControl>("Secondary Skill Cooldown", "secondary_skill_cooldown", 0.0f, 0.0f, 60.0f, 0.1f, false, false, true);
    utilitySkillCooldownControl = std::make_unique<FloatControl>("Utility Skill Cooldown", "utility_skill_cooldown", 0.0f, 0.0f, 60.0f, 0.1f, false, false, true);
    specialSkillCooldownControl = std::make_unique<FloatControl>("Special Skill Cooldown", "special_skill_cooldown", 0.0f, 0.0f, 60.0f, 0.1f, false, false, true);

    moneyControl = std::make_unique<IntControl>("Money", "player_money", 0, 0, INT_MAX, 100, false, false);
    voidCoinsControl = std::make_unique<IntControl>("Void Coins", "player_void_coins", 0, 0, INT_MAX, 1, false, false);
    lunarCoinsControl = std::make_unique<IntControl>("Lunar Coins", "player_lunar_coins", 0, 0, INT_MAX, 1, false, false);
    levelControl = std::make_unique<FloatControl>("Level", "player_level", 1.0f, 1.0f, FLT_MAX, 1.0f, false, false, true);

    healthControl = std::make_unique<FloatControl>("Max Health", "player_health", 100.0f, 1.0f, FLT_MAX, 10.0f, false, false, true);
    armorControl = std::make_unique<FloatControl>("Armor", "player_armor", 0.0f, 0.0f, FLT_MAX, 5.0f, false, false, true);

    // Set up change callbacks to track user input
    moneyControl->SetOnChange([this](int newValue) {
        if (localUser_cached && localUser_cached->cachedMaster_backing) {
            localUser_cached->cachedMaster_backing->_money = newValue;
        }
    });

    voidCoinsControl->SetOnChange([this](int newValue) {
        if (localUser_cached && localUser_cached->cachedMaster_backing) {
            localUser_cached->cachedMaster_backing->_voidCoins = newValue;
        }
    });

    lunarCoinsControl->SetOnChange([this](int newValue) {
        static int lastUserValue = 0;
        if (lastUserValue == newValue) {
            return;
        }

        lastUserValue = newValue;

        if (localUser_cached && localUser_cached->currentNetworkUser_backing) {
            uint32_t currentCoins = localUser_cached->currentNetworkUser_backing->netLunarCoins;
            if (newValue > currentCoins) {
                uint32_t coinsToAdd = newValue - currentCoins;
                G::gameFunctions->AwardLunarCoins(localUser_cached->currentNetworkUser_backing, coinsToAdd);
            } else if (newValue < currentCoins) {
                uint32_t coinsToRemove = currentCoins - newValue;
                G::gameFunctions->DeductLunarCoins(localUser_cached->currentNetworkUser_backing, coinsToRemove);
            }
        }
    });

    levelControl->SetOnChange([this](float newValue) {
        static float lastUserValue = 1.0f;
        if (lastUserValue == newValue) {
            return;
        }

        lastUserValue = newValue;

        TeamManager* teamManager = G::gameFunctions->GetTeamManagerInstance();
        if (teamManager) {
            uint32_t level = static_cast<uint32_t>(newValue);
            G::logger.LogInfo("Setting team level to %u", level);

            G::gameFunctions->SetTeamLevel(TeamIndex_Value::Player, level);
        } else {
            G::logger.LogWarning("TeamManager instance not available in level control callback");
        }
    });



    healthControl->SetOnChange([this](float newValue) {
        if (localUser_cached && localUser_cached->cachedBody_backing) {
            localUser_cached->cachedBody_backing->maxHealth_backing = newValue;
        }
    });

    armorControl->SetOnChange([this](float newValue) {
        if (localUser_cached && localUser_cached->cachedBody_backing) {
            localUser_cached->cachedBody_backing->baseArmor = newValue;
        }
    });

    baseMoveSpeedControl->SetOnChange([this](float newValue) {
        if (localUser_cached && localUser_cached->cachedBody_backing) {
            localUser_cached->cachedBody_backing->baseMoveSpeed = newValue;
        }
    });

    baseDamageControl->SetOnChange([this](float newValue) {
        if (localUser_cached && localUser_cached->cachedBody_backing) {
            localUser_cached->cachedBody_backing->baseDamage = newValue;
        }
    });

    baseAttackSpeedControl->SetOnChange([this](float newValue) {
        if (localUser_cached && localUser_cached->cachedBody_backing) {
            localUser_cached->cachedBody_backing->baseAttackSpeed = newValue;
        }
    });

    baseCritControl->SetOnChange([this](float newValue) {
        if (localUser_cached && localUser_cached->cachedBody_backing) {
            localUser_cached->cachedBody_backing->baseCrit = newValue;
        }
    });

    baseJumpCountControl->SetOnChange([this](int newValue) {
        if (localUser_cached && localUser_cached->cachedBody_backing) {
            localUser_cached->cachedBody_backing->baseJumpCount = newValue;
        }
    });

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
    huntressTargetingModeOverrideControl->Update();
    huntressTargetingModeControl->Update();
    huntressWallPenetrationControl->Update();
    blockPhysicsEffectsControl->Update();
    blockPullsControl->Update();
    flightControl->Update();

    primarySkillCooldownControl->Update();
    secondarySkillCooldownControl->Update();
    utilitySkillCooldownControl->Update();
    specialSkillCooldownControl->Update();

    moneyControl->Update();
    voidCoinsControl->Update();
    lunarCoinsControl->Update();
    levelControl->Update();

    healthControl->Update();
    armorControl->Update();

    for (const auto& [index, control] : itemControls) {
        control->Update();
    }
}

void PlayerModule::DrawUI() {
    godModeControl->Draw();
    baseMoveSpeedControl->Draw();
    baseDamageControl->Draw();
    baseAttackSpeedControl->Draw();
    baseCritControl->Draw();
    levelControl->Draw();
    healthControl->Draw();
    armorControl->Draw();
    baseJumpCountControl->Draw();
    teleportToCursorControl->Draw();
    flightControl->Draw();
    deployableCapControl->Draw();

    if (ImGui::CollapsingHeader("Skill Cooldowns")) {
        primarySkillCooldownControl->Draw();
        secondarySkillCooldownControl->Draw();
        utilitySkillCooldownControl->Draw();
        specialSkillCooldownControl->Draw();
    }

    if (ImGui::CollapsingHeader("Resources")) {
        moneyControl->Draw();
        voidCoinsControl->Draw();
        lunarCoinsControl->Draw();
    }

    if (ImGui::CollapsingHeader("Huntress Settings")) {
        huntressRangeControl->Draw();
        huntressFOVControl->Draw();
        huntressWallPenetrationControl->Draw();
        huntressEnemyOnlyTargetingControl->Draw();
        huntressTargetingModeOverrideControl->Draw();
        if (huntressTargetingModeOverrideControl->IsEnabled()) {
            ImGui::Indent();
            huntressTargetingModeControl->Draw();
            ImGui::Unindent();
        }
    }

    if (ImGui::CollapsingHeader("Physics Settings")) {
        blockPhysicsEffectsControl->Draw();
        blockPullsControl->Draw();
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

    // Base Move Speed
    {
        float currentMoveSpeed = localUser_ptr->cachedBody_backing->baseMoveSpeed;
        if (baseMoveSpeedControl->IsEnabled() && currentMoveSpeed < baseMoveSpeedControl->GetFrozenValue()) {
            localUser_ptr->cachedBody_backing->baseMoveSpeed = baseMoveSpeedControl->GetFrozenValue();
        }
        baseMoveSpeedControl->SetValue(localUser_ptr->cachedBody_backing->baseMoveSpeed);
    }

    // Base Damage
    {
        float currentDamage = localUser_ptr->cachedBody_backing->baseDamage;
        if (baseDamageControl->IsEnabled() && currentDamage < baseDamageControl->GetFrozenValue()) {
            localUser_ptr->cachedBody_backing->baseDamage = baseDamageControl->GetFrozenValue();
        }
        baseDamageControl->SetValue(localUser_ptr->cachedBody_backing->baseDamage);
    }

    // Base Attack Speed
    {
        float currentAttackSpeed = localUser_ptr->cachedBody_backing->baseAttackSpeed;
        if (baseAttackSpeedControl->IsEnabled() && currentAttackSpeed < baseAttackSpeedControl->GetFrozenValue()) {
            localUser_ptr->cachedBody_backing->baseAttackSpeed = baseAttackSpeedControl->GetFrozenValue();
        }
        baseAttackSpeedControl->SetValue(localUser_ptr->cachedBody_backing->baseAttackSpeed);
    }

    // Base Crit
    {
        float currentCrit = localUser_ptr->cachedBody_backing->baseCrit;
        if (baseCritControl->IsEnabled() && currentCrit < baseCritControl->GetFrozenValue()) {
            localUser_ptr->cachedBody_backing->baseCrit = baseCritControl->GetFrozenValue();
        }
        baseCritControl->SetValue(localUser_ptr->cachedBody_backing->baseCrit);
    }

    // Base Jump Count
    {
        int currentJumpCount = localUser_ptr->cachedBody_backing->baseJumpCount;
        if (baseJumpCountControl->IsEnabled() && currentJumpCount < baseJumpCountControl->GetFrozenValue()) {
            localUser_ptr->cachedBody_backing->baseJumpCount = baseJumpCountControl->GetFrozenValue();
        }
        baseJumpCountControl->SetValue(localUser_ptr->cachedBody_backing->baseJumpCount);
    }

    // Money
    {
        uint32_t currentMoney = localUser_ptr->cachedMaster_backing->_money;
        if (moneyControl->IsEnabled() && currentMoney < moneyControl->GetFrozenValue()) {
            localUser_ptr->cachedMaster_backing->_money = moneyControl->GetFrozenValue();
        }
        moneyControl->SetValue(localUser_ptr->cachedMaster_backing->_money);
    }

    // Void Coins
    {
        uint32_t currentCoins = localUser_ptr->cachedMaster_backing->_voidCoins;
        if (voidCoinsControl->IsEnabled() && currentCoins < voidCoinsControl->GetFrozenValue()) {
            localUser_ptr->cachedMaster_backing->_voidCoins = voidCoinsControl->GetFrozenValue();
        }
        voidCoinsControl->SetValue(localUser_ptr->cachedMaster_backing->_voidCoins);
    }

    // Lunar Coins
    {
        if (localUser_ptr->currentNetworkUser_backing) {
            uint32_t currentLunarCoins = localUser_ptr->currentNetworkUser_backing->netLunarCoins;
            if (lunarCoinsControl->IsEnabled() && currentLunarCoins < lunarCoinsControl->GetFrozenValue()) {
                uint32_t targetCoins = lunarCoinsControl->GetFrozenValue();
                uint32_t coinsToAdd = targetCoins - currentLunarCoins;
                G::gameFunctions->AwardLunarCoins(localUser_ptr->currentNetworkUser_backing, coinsToAdd);
            }
            lunarCoinsControl->SetValue(currentLunarCoins);
        }
    }

    // Health
    {
        float currentHealth = localUser_ptr->cachedBody_backing->maxHealth_backing;
        if (healthControl->IsEnabled() && currentHealth < healthControl->GetFrozenValue()) {
            localUser_ptr->cachedBody_backing->maxHealth_backing = healthControl->GetFrozenValue();
        }
        healthControl->SetValue(localUser_ptr->cachedBody_backing->maxHealth_backing);
    }

    // Level
    {
        float currentLevel = localUser_ptr->cachedBody_backing->level_backing;
        if (levelControl->IsEnabled() && currentLevel < levelControl->GetFrozenValue()) {
            G::gameFunctions->SetTeamLevel(TeamIndex_Value::Player, static_cast<uint32_t>(levelControl->GetFrozenValue()));
            currentLevel = levelControl->GetFrozenValue();
        }
        levelControl->SetValue(currentLevel);
    }



    // Armor
    {
        float currentArmor = localUser_ptr->cachedBody_backing->baseArmor;
        if (armorControl->IsEnabled() && currentArmor < armorControl->GetFrozenValue()) {
            localUser_ptr->cachedBody_backing->baseArmor = armorControl->GetFrozenValue();
        }
        armorControl->SetValue(localUser_ptr->cachedBody_backing->baseArmor);
    }

    // Flight Implementation
    if (localUser_ptr->cachedBody_backing->characterMotor_backing) {
        CharacterMotor* motor = localUser_ptr->cachedBody_backing->characterMotor_backing;

        if (flightControl->IsEnabled() && !isProvidingFlight) {
            motor->_flightParameters.channeledFlightGranterCount++;
            motor->_gravityParameters.channeledAntiGravityGranterCount++;

            // Manually trigger the same logic as the property setters
            // CheckShouldUseFlight: return channeledFlightGranterCount > 0
            motor->isFlying_backing = (motor->_flightParameters.channeledFlightGranterCount > 0);

            // CheckShouldUseGravity: return environmentalAntiGravityGranterCount <= 0 && (antiGravityNeutralizerCount > 0 || channeledAntiGravityGranterCount <= 0)
            motor->useGravity_backing = (motor->_gravityParameters.environmentalAntiGravityGranterCount <= 0 &&
                                       (motor->_gravityParameters.antiGravityNeutralizerCount > 0 ||
                                        motor->_gravityParameters.channeledAntiGravityGranterCount <= 0));

            isProvidingFlight = true;
        } else if (!flightControl->IsEnabled() && isProvidingFlight) {
            motor->_flightParameters.channeledFlightGranterCount--;
            motor->_gravityParameters.channeledAntiGravityGranterCount--;

            // Manually trigger the same logic as the property setters
            // CheckShouldUseFlight: return channeledFlightGranterCount > 0
            motor->isFlying_backing = (motor->_flightParameters.channeledFlightGranterCount > 0);

            // CheckShouldUseGravity: return environmentalAntiGravityGranterCount <= 0 && (antiGravityNeutralizerCount > 0 || channeledAntiGravityGranterCount <= 0)
            motor->useGravity_backing = (motor->_gravityParameters.environmentalAntiGravityGranterCount <= 0 &&
                                       (motor->_gravityParameters.antiGravityNeutralizerCount > 0 ||
                                        motor->_gravityParameters.channeledAntiGravityGranterCount <= 0));

            isProvidingFlight = false;
        }
    }

    HuntressTracker* currentTracker = GetCurrentLocalTracker();
    if (currentTracker) {
        if (huntressRangeControl->IsEnabled()) {
            currentTracker->maxTrackingDistance = huntressRangeControl->GetValue();
        }

        if (huntressFOVControl->IsEnabled()) {
            currentTracker->maxTrackingAngle = huntressFOVControl->GetValue();
        }
    }

    SkillLocator* skillLocator = localUser_ptr->cachedBody_backing->skillLocator_backing;
    if (skillLocator) {
        if (primarySkillCooldownControl->IsEnabled() && skillLocator->primary) {
            skillLocator->primary->finalRechargeInterval = primarySkillCooldownControl->GetValue();
        }

        if (secondarySkillCooldownControl->IsEnabled() && skillLocator->secondary) {
            skillLocator->secondary->finalRechargeInterval = secondarySkillCooldownControl->GetValue();
        }

        if (utilitySkillCooldownControl->IsEnabled() && skillLocator->utility) {
            skillLocator->utility->finalRechargeInterval = utilitySkillCooldownControl->GetValue();
        }

        if (specialSkillCooldownControl->IsEnabled() && skillLocator->special) {
            skillLocator->special->finalRechargeInterval = specialSkillCooldownControl->GetValue();
        }
    }

    if (!localUser_ptr->cachedBody_backing->inventory_backing) {
        return;
    }


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
    if (!inventory_ptr || !inventory_ptr->itemStacks) {
        return;
    }

    // Check if this is the local player's inventory
    if (!localUser_cached || !localUser_cached->cachedBody_backing ||
        inventory != localUser_cached->cachedBody_backing->inventory_backing) {
        return;
    }

    std::unique_lock<std::shared_mutex> lock(G::itemsMutex);
    int* arrayData = (int*)(inventory_ptr->itemStacks + 8); // Adjusted offset of array with header
    for (int i = 0; i < itemStacks.size(); i++) {
        itemStacks[i] = arrayData[i];
        if (itemControls.count(i) > 0) {
            const auto& control = itemControls[i];

            // Update the UI control value
            if (control->GetValue() != arrayData[i]) {
                control->SetValue(arrayData[i]);
            }

            // If item is protected and below frozen value, queue it to be given
            if (control->IsEnabled() && arrayData[i] < control->GetFrozenValue()) {
                std::unique_lock<std::mutex> queueLock(queuedGiveItemsMutex);
                queuedGiveItems.push(std::make_tuple(i, control->GetFrozenValue()));
            }
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

        auto control = std::make_unique<IntControl>(item.displayName, "item_" + std::to_string(index),
                                                   currentCount, 0, INT_MAX, 1, false, false);

        control->SetOnChange([this, index](int newValue) {
            if (index < itemStacks.size()) {
                itemStacks[index] = newValue;
            }
            std::unique_lock<std::mutex> lock(queuedGiveItemsMutex);
            queuedGiveItems.push(std::make_tuple(index, newValue));
        });

        itemControls[index] = std::move(control);
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

    // Only cache player team trackers
    if (tracker->teamComponent &&
        tracker->teamComponent->_teamIndex == TeamIndex_Value::Player) {

        std::lock_guard<std::mutex> lock(trackerCacheMutex);
        playerHuntressTrackers[tracker->characterBody] = tracker;
    }
}

HuntressTracker* PlayerModule::GetCurrentLocalTracker() {
    if (!localUser_cached || !localUser_cached->cachedBody_backing) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(trackerCacheMutex);
    auto it = playerHuntressTrackers.find(localUser_cached->cachedBody_backing);
    return (it != playerHuntressTrackers.end()) ? it->second : nullptr;
}

void PlayerModule::OnCharacterBodyDestroyed(void* characterBody) {
    std::lock_guard<std::mutex> lock(trackerCacheMutex);
    CharacterBody* body = (CharacterBody*)characterBody;
    playerHuntressTrackers.erase(body);

    // Reset flight state when local player's body is destroyed
    if (localUser_cached && body == localUser_cached->cachedBody_backing) {
        isProvidingFlight = false;
    }
}

void PlayerModule::ConvertPlayerMoneyToExperienceUpdate() {
    if (localUser_cached && localUser_cached->cachedMaster_backing) {
        uint32_t currentMoney = localUser_cached->cachedMaster_backing->_money;

        // If this is the first run of the conversion, disable money freeze
        if (!isMoneyConversionActive && moneyControl->IsEnabled()) {
            isMoneyConversionActive = true;
            moneyControl->SetEnabled(false);
        }
    }
}

void PlayerModule::OnStageAdvance(void* stage) {
    // Re-enable money freeze when player advances to the next stage
    if (isMoneyConversionActive) {
        moneyControl->SetEnabled(true);
        if (localUser_cached && localUser_cached->cachedMaster_backing) {
            localUser_cached->cachedMaster_backing->_money = moneyControl->GetFrozenValue();
        }
        isMoneyConversionActive = false;
    }
}