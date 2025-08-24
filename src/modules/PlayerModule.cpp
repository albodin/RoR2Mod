#include "PlayerModule.h"
#include "config/ConfigManager.h"
#include "globals/globals.h"
#include "hooks/hooks.h"
#include "imgui.h"
#include "menu/ItemsUI.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <set>

void BodyTracker::Initialize(const std::vector<std::pair<std::string, GameObject*>>& prefabsWithNames) {
    Clear();

    const std::vector<std::string> blacklist = {
        "LunarRain_DistanceTest" // Crashes the game when switched to
    };

    std::map<std::string, int> nameCount;
    std::map<std::string, int> nameIndex;
    std::vector<std::pair<std::string, GameObject*>> tempBodies;

    G::logger.LogInfo("BodyTracker::Initialize: Processing %d body prefabs", prefabsWithNames.size());

    // First pass: count duplicates
    for (const auto& [tokenOrName, prefab] : prefabsWithNames) {
        if (!prefab || tokenOrName.empty())
            continue;

        nameCount[tokenOrName]++;
    }

    // Second pass: add bodies with unique names
    for (const auto& [tokenOrName, prefab] : prefabsWithNames) {
        if (!prefab || tokenOrName.empty())
            continue;

        std::string internalName = G::gameFunctions->GetUnityObjectName(prefab);

        bool isBlacklisted = false;
        for (const auto& blacklistedName : blacklist) {
            if (internalName == blacklistedName) {
                G::logger.LogInfo("Skipping blacklisted body: %s", internalName.c_str());
                isBlacklisted = true;
                break;
            }
        }

        if (isBlacklisted)
            continue;

        std::string finalName = tokenOrName;

        if (nameCount[tokenOrName] > 1) {
            finalName = tokenOrName + " (" + internalName + ")";

            // If this combination is still not unique, add a number
            auto existingIt = std::find_if(tempBodies.begin(), tempBodies.end(), [&finalName](const auto& pair) { return pair.first == finalName; });
            if (existingIt != tempBodies.end()) {
                int index = 2;
                std::string numberedName;
                do {
                    numberedName = finalName + " " + std::to_string(index++);
                    existingIt = std::find_if(tempBodies.begin(), tempBodies.end(), [&numberedName](const auto& pair) { return pair.first == numberedName; });
                } while (existingIt != tempBodies.end());
                finalName = numberedName;
            }
        }

        tempBodies.push_back({finalName, prefab});
    }

    // Sort the bodies alphabetically by display name
    std::sort(tempBodies.begin(), tempBodies.end(), [](const auto& a, const auto& b) {
        std::string aLower = a.first;
        std::string bLower = b.first;
        std::transform(aLower.begin(), aLower.end(), aLower.begin(), ::tolower);
        std::transform(bLower.begin(), bLower.end(), bLower.begin(), ::tolower);
        return aLower < bLower;
    });

    for (const auto& [name, prefab] : tempBodies) {
        names.push_back(name);
        prefabs.push_back(prefab);
    }

    G::logger.LogInfo("BodyTracker initialized with %d bodies (sorted alphabetically)", names.size());
}

int BodyTracker::UpdateBody(GameObject* newPrefab) {
    cachedPrefab = newPrefab;
    cachedIndex = -1;
    cachedName = "";

    for (size_t i = 0; i < prefabs.size(); i++) {
        if (prefabs[i] == newPrefab) {
            cachedIndex = static_cast<int>(i); // i will always be within int range here
            cachedName = names[i];
            break;
        }
    }

    return cachedIndex;
}

void BodyTracker::Clear() {
    names.clear();
    prefabs.clear();
    cachedPrefab = nullptr;
    cachedIndex = -1;
    cachedName.clear();
}

GameObject* BodyTracker::GetPrefabByName(const std::string& name) const {
    for (size_t i = 0; i < names.size(); i++) {
        if (names[i] == name) {
            return prefabs[i];
        }
    }
    return nullptr;
}

PlayerModule::PlayerModule() : ModuleBase(), localUser_cached(nullptr), isProvidingFlight(false), isMoneyConversionActive(false) { Initialize(); }

PlayerModule::~PlayerModule() { itemControls.clear(); }

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
    baseMoveSpeedControl->SetGameValueFunctions(
        [this]() { return localUser_cached && localUser_cached->cachedBody_backing ? localUser_cached->cachedBody_backing->baseMoveSpeed : 0.0f; },
        [this](float value) {
            if (localUser_cached && localUser_cached->cachedBody_backing)
                localUser_cached->cachedBody_backing->baseMoveSpeed = value;
        });

    baseDamageControl = std::make_unique<FloatControl>("Base Damage", "baseDamage", 12.0f, 0.0f, FLT_MAX, 1.0f, false, false, true);
    baseDamageControl->SetGameValueFunctions(
        [this]() { return localUser_cached && localUser_cached->cachedBody_backing ? localUser_cached->cachedBody_backing->baseDamage : 0.0f; },
        [this](float value) {
            if (localUser_cached && localUser_cached->cachedBody_backing)
                localUser_cached->cachedBody_backing->baseDamage = value;
        });

    baseAttackSpeedControl = std::make_unique<FloatControl>("Base Attack Speed", "baseAttackSpeed", 1.0f, 0.0f, FLT_MAX, 0.1f, false, false, true);
    baseAttackSpeedControl->SetGameValueFunctions(
        [this]() { return localUser_cached && localUser_cached->cachedBody_backing ? localUser_cached->cachedBody_backing->baseAttackSpeed : 0.0f; },
        [this](float value) {
            if (localUser_cached && localUser_cached->cachedBody_backing)
                localUser_cached->cachedBody_backing->baseAttackSpeed = value;
        });

    baseCritControl = std::make_unique<FloatControl>("Base Crit", "baseCrit", 1.0f, 0.0f, FLT_MAX, 1.0f, false, false, true);
    baseCritControl->SetGameValueFunctions(
        [this]() { return localUser_cached && localUser_cached->cachedBody_backing ? localUser_cached->cachedBody_backing->baseCrit : 0.0f; },
        [this](float value) {
            if (localUser_cached && localUser_cached->cachedBody_backing)
                localUser_cached->cachedBody_backing->baseCrit = value;
        });

    baseJumpCountControl = std::make_unique<IntControl>("Base Jump Count", "baseJumpCount", 1, 0, INT_MAX, 1, false, false, true);
    baseJumpCountControl->SetGameValueFunctions(
        [this]() { return localUser_cached && localUser_cached->cachedBody_backing ? localUser_cached->cachedBody_backing->baseJumpCount : 0; },
        [this](int value) {
            if (localUser_cached && localUser_cached->cachedBody_backing)
                localUser_cached->cachedBody_backing->baseJumpCount = value;
        });

    teleportToCursorControl = std::make_unique<ToggleButtonControl>("Teleport to Cursor", "teleportToCursor", "Teleport", false);
    teleportToCursorControl->SetOnAction([this]() {
        if (localUser_cached && localUser_cached->cachedBody_backing && localUser_cached->_cameraRigController) {
            G::gameFunctions->TeleportHelper_TeleportBody(localUser_cached->cachedBody_backing,
                                                          localUser_cached->_cameraRigController->crosshairWorldPosition_backing);
        }
    });

    huntressRangeControl = std::make_unique<FloatControl>("Huntress Range", "huntressRange", 20.0f, 0.0f, 1000.0f, 5.0f, false, false, true);
    huntressRangeControl->SetGameValueFunctions(
        [this]() {
            HuntressTracker* tracker = GetCurrentLocalTracker();
            return tracker ? tracker->maxTrackingDistance : 20.0f;
        },
        [this](float value) {
            HuntressTracker* tracker = GetCurrentLocalTracker();
            if (tracker)
                tracker->maxTrackingDistance = value;
        });

    huntressFOVControl = std::make_unique<FloatControl>("Huntress FOV", "huntressFOV", 20.0f, 0.0f, 180.0f, 5.0f, false, false, true);
    huntressFOVControl->SetGameValueFunctions(
        [this]() {
            HuntressTracker* tracker = GetCurrentLocalTracker();
            return tracker ? tracker->maxTrackingAngle : 20.0f;
        },
        [this](float value) {
            HuntressTracker* tracker = GetCurrentLocalTracker();
            if (tracker)
                tracker->maxTrackingAngle = value;
        });
    huntressWallPenetrationControl = std::make_unique<ToggleControl>("Huntress Wall Penetration", "huntressWallPenetration", false);
    huntressEnemyOnlyTargetingControl = std::make_unique<ToggleControl>("Huntress Ignore Breakables", "huntressEnemyOnlyTargeting", false);

    huntressTargetingModeOverrideControl = std::make_unique<ToggleControl>("Override Huntress Targeting Mode", "huntressTargetingModeOverride", false);
    huntressTargetingModeControl = std::make_unique<ComboControl>(
        "Targeting Mode", "huntressTargetingMode", std::vector<std::string>{"None", "Distance", "Angle", "Distance + Angle"}, std::vector<int>{0, 1, 2, 3}, 3);

    blockPhysicsEffectsControl = std::make_unique<ToggleControl>("Block Enemy Forces", "blockPhysicsEffects", false);
    blockPullsControl = std::make_unique<ToggleControl>("Block Displacements", "blockPulls", false);

    flightControl = std::make_unique<ToggleControl>("Flight", "flight", false);

    deployableCapControl = std::make_unique<IntControl>("Deployable Cap", "deployable_cap", 2, 1, INT_MAX, 1, false, false, true);

    primarySkillCooldownControl =
        std::make_unique<FloatControl>("Primary Skill Cooldown", "primary_skill_cooldown", 0.0f, 0.0f, 60.0f, 0.1f, false, false, true);
    primarySkillCooldownControl->SetGameValueFunctions(
        [this]() {
            if (localUser_cached && localUser_cached->cachedBody_backing && localUser_cached->cachedBody_backing->skillLocator_backing &&
                localUser_cached->cachedBody_backing->skillLocator_backing->primary) {
                return localUser_cached->cachedBody_backing->skillLocator_backing->primary->finalRechargeInterval;
            }
            return 0.0f;
        },
        [this](float value) {
            if (localUser_cached && localUser_cached->cachedBody_backing && localUser_cached->cachedBody_backing->skillLocator_backing &&
                localUser_cached->cachedBody_backing->skillLocator_backing->primary) {
                localUser_cached->cachedBody_backing->skillLocator_backing->primary->finalRechargeInterval = value;
            }
        });

    secondarySkillCooldownControl =
        std::make_unique<FloatControl>("Secondary Skill Cooldown", "secondary_skill_cooldown", 0.0f, 0.0f, 60.0f, 0.1f, false, false, true);
    secondarySkillCooldownControl->SetGameValueFunctions(
        [this]() {
            if (localUser_cached && localUser_cached->cachedBody_backing && localUser_cached->cachedBody_backing->skillLocator_backing &&
                localUser_cached->cachedBody_backing->skillLocator_backing->secondary) {
                return localUser_cached->cachedBody_backing->skillLocator_backing->secondary->finalRechargeInterval;
            }
            return 0.0f;
        },
        [this](float value) {
            if (localUser_cached && localUser_cached->cachedBody_backing && localUser_cached->cachedBody_backing->skillLocator_backing &&
                localUser_cached->cachedBody_backing->skillLocator_backing->secondary) {
                localUser_cached->cachedBody_backing->skillLocator_backing->secondary->finalRechargeInterval = value;
            }
        });

    utilitySkillCooldownControl =
        std::make_unique<FloatControl>("Utility Skill Cooldown", "utility_skill_cooldown", 0.0f, 0.0f, 60.0f, 0.1f, false, false, true);
    utilitySkillCooldownControl->SetGameValueFunctions(
        [this]() {
            if (localUser_cached && localUser_cached->cachedBody_backing && localUser_cached->cachedBody_backing->skillLocator_backing &&
                localUser_cached->cachedBody_backing->skillLocator_backing->utility) {
                return localUser_cached->cachedBody_backing->skillLocator_backing->utility->finalRechargeInterval;
            }
            return 0.0f;
        },
        [this](float value) {
            if (localUser_cached && localUser_cached->cachedBody_backing && localUser_cached->cachedBody_backing->skillLocator_backing &&
                localUser_cached->cachedBody_backing->skillLocator_backing->utility) {
                localUser_cached->cachedBody_backing->skillLocator_backing->utility->finalRechargeInterval = value;
            }
        });

    specialSkillCooldownControl =
        std::make_unique<FloatControl>("Special Skill Cooldown", "special_skill_cooldown", 0.0f, 0.0f, 60.0f, 0.1f, false, false, true);
    specialSkillCooldownControl->SetGameValueFunctions(
        [this]() {
            if (localUser_cached && localUser_cached->cachedBody_backing && localUser_cached->cachedBody_backing->skillLocator_backing &&
                localUser_cached->cachedBody_backing->skillLocator_backing->special) {
                return localUser_cached->cachedBody_backing->skillLocator_backing->special->finalRechargeInterval;
            }
            return 0.0f;
        },
        [this](float value) {
            if (localUser_cached && localUser_cached->cachedBody_backing && localUser_cached->cachedBody_backing->skillLocator_backing &&
                localUser_cached->cachedBody_backing->skillLocator_backing->special) {
                localUser_cached->cachedBody_backing->skillLocator_backing->special->finalRechargeInterval = value;
            }
        });

    moneyControl = std::make_unique<IntControl>("Money", "player_money", 0, 0, INT_MAX, 100, false, false, true);
    moneyControl->SetGameValueFunctions(
        [this]() { return localUser_cached && localUser_cached->cachedMaster_backing ? static_cast<int>(localUser_cached->cachedMaster_backing->_money) : 0; },
        [this](int value) {
            if (localUser_cached && localUser_cached->cachedMaster_backing)
                localUser_cached->cachedMaster_backing->_money = static_cast<uint32_t>(value);
        });

    voidCoinsControl = std::make_unique<IntControl>("Void Coins", "player_void_coins", 0, 0, INT_MAX, 1, false, false, true);
    voidCoinsControl->SetGameValueFunctions(
        [this]() {
            return localUser_cached && localUser_cached->cachedMaster_backing ? static_cast<int>(localUser_cached->cachedMaster_backing->_voidCoins) : 0;
        },
        [this](int value) {
            if (localUser_cached && localUser_cached->cachedMaster_backing)
                localUser_cached->cachedMaster_backing->_voidCoins = static_cast<uint32_t>(value);
        });

    lunarCoinsControl = std::make_unique<IntControl>("Lunar Coins", "player_lunar_coins", 0, 0, INT_MAX, 1, false, false, true);
    lunarCoinsControl->SetGameValueFunctions(
        [this]() {
            return localUser_cached && localUser_cached->currentNetworkUser_backing
                       ? static_cast<int>(localUser_cached->currentNetworkUser_backing->netLunarCoins)
                       : 0;
        },
        [this](int value) {
            if (localUser_cached && localUser_cached->currentNetworkUser_backing) {
                uint32_t currentCoins = localUser_cached->currentNetworkUser_backing->netLunarCoins;
                if (value > currentCoins) {
                    G::gameFunctions->AwardLunarCoins(localUser_cached->currentNetworkUser_backing, value - currentCoins);
                } else if (value < currentCoins) {
                    G::gameFunctions->DeductLunarCoins(localUser_cached->currentNetworkUser_backing, currentCoins - value);
                }
            }
        });

    levelControl = std::make_unique<FloatControl>("Level", "player_level", 1.0f, 1.0f, FLT_MAX, 1.0f, false, false, true);
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
    levelControl->SetGameValueFunctions(
        [this]() { return localUser_cached && localUser_cached->cachedBody_backing ? localUser_cached->cachedBody_backing->level_backing : 1.0f; },
        [this](float value) {
            if (localUser_cached)
                G::gameFunctions->SetTeamLevel(TeamIndex_Value::Player, static_cast<uint32_t>(value));
        });

    healthControl = std::make_unique<FloatControl>("Max Health", "player_health", 100.0f, 1.0f, FLT_MAX, 10.0f, false, false, true);
    healthControl->SetGameValueFunctions(
        [this]() { return localUser_cached && localUser_cached->cachedBody_backing ? localUser_cached->cachedBody_backing->maxHealth_backing : 100.0f; },
        [this](float value) {
            if (localUser_cached && localUser_cached->cachedBody_backing)
                localUser_cached->cachedBody_backing->maxHealth_backing = value;
        });

    armorControl = std::make_unique<FloatControl>("Armor", "player_armor", 0.0f, 0.0f, FLT_MAX, 5.0f, false, false, true);
    armorControl->SetGameValueFunctions(
        [this]() { return localUser_cached && localUser_cached->cachedBody_backing ? localUser_cached->cachedBody_backing->baseArmor : 0.0f; },
        [this](float value) {
            if (localUser_cached && localUser_cached->cachedBody_backing)
                localUser_cached->cachedBody_backing->baseArmor = value;
        });

    lockCharacterModelControl = std::make_unique<ToggleControl>("Lock Character Model", "lockCharacterModel", false);
    std::vector<std::string> characterNames = {"Loading..."};
    selectedCharacterIndexControl = std::make_unique<ComboControl>("Character", "selectedCharacter", characterNames, 0);
    selectedCharacterIndexControl->SetOnChange([this](int index) {
        if (!IsInGame()) {
            return;
        }

        std::string selectedBody = selectedCharacterIndexControl->GetSelectedItem();
        G::logger.LogInfo("Selected body: '%s', current body: '%s'", selectedBody.c_str(), bodyTracker.GetCurrentBodyName().c_str());
        if (!selectedBody.empty() && selectedBody != bodyTracker.GetCurrentBodyName()) {
            ApplyModelChange(selectedBody);
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

    lockCharacterModelControl->Update();
    selectedCharacterIndexControl->Update();

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

    UpdateCurrentBodyTracking();
    if (ImGui::CollapsingHeader("Character Model")) {
        lockCharacterModelControl->Draw();
        ImGui::Text("Search:");
        ImGui::SameLine();
        if (ImGui::InputText("##BodySearch", bodySearchFilter, sizeof(bodySearchFilter))) {
            if (strlen(bodySearchFilter) > 0) {
                RefreshFilteredBodyList();
                // Keep current body selected (always first in filtered list)
                if (!lockCharacterModelControl->IsEnabled()) {
                    selectedCharacterIndexControl->SetSelectedIndex(0);
                }
            } else {
                if (selectedCharacterIndexControl && !bodyTracker.GetBodyNames().empty()) {
                    selectedCharacterIndexControl->SetItems(bodyTracker.GetBodyNames());
                    // Restore selection to actual body index in full list
                    if (!lockCharacterModelControl->IsEnabled()) {
                        int currentIndex = bodyTracker.GetCurrentBodyIndex();
                        if (currentIndex != -1) {
                            selectedCharacterIndexControl->SetSelectedIndex(currentIndex);
                        }
                    }
                }
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear")) {
            bodySearchFilter[0] = '\0';
            if (selectedCharacterIndexControl && !bodyTracker.GetBodyNames().empty()) {
                selectedCharacterIndexControl->SetItems(bodyTracker.GetBodyNames());
                // Restore selection to actual body index in full list
                if (!lockCharacterModelControl->IsEnabled()) {
                    int currentIndex = bodyTracker.GetCurrentBodyIndex();
                    if (currentIndex != -1) {
                        selectedCharacterIndexControl->SetSelectedIndex(currentIndex);
                    }
                }
            }
        }

        selectedCharacterIndexControl->Draw();
    }

    ItemsUI::DrawItemsSection(items, itemControls, &itemStacks);
}

void PlayerModule::OnLocalUserUpdate(void* localUser) {
    LocalUser* localUser_ptr = static_cast<LocalUser*>(localUser);
    if (!localUser_ptr->cachedMaster_backing || !localUser_ptr->cachedBody_backing || !localUser_ptr->cachedBody_backing->healthComponent_backing) {
        return;
    }
    localUser_cached = localUser_ptr;

    Hooks::Transform_get_position_Injected(localUser_ptr->cachedBody_backing->transform, &playerPosition);
    if (localUser_cached->_cameraRigController) {
        crosshairPosition = localUser_cached->_cameraRigController->crosshairWorldPosition_backing;
    }

    localUser_ptr->cachedMaster_backing->godMode = godModeControl->IsEnabled();
    localUser_ptr->cachedBody_backing->healthComponent_backing->godMode_backing = godModeControl->IsEnabled();

    baseMoveSpeedControl->UpdateFreezeLogic();
    baseDamageControl->UpdateFreezeLogic();
    baseAttackSpeedControl->UpdateFreezeLogic();
    baseCritControl->UpdateFreezeLogic();
    baseJumpCountControl->UpdateFreezeLogic();

    moneyControl->UpdateFreezeLogic();
    voidCoinsControl->UpdateFreezeLogic();
    lunarCoinsControl->UpdateFreezeLogic();

    healthControl->UpdateFreezeLogic();
    levelControl->UpdateFreezeLogic();
    armorControl->UpdateFreezeLogic();

    // Flight Implementation
    if (localUser_ptr->cachedBody_backing->characterMotor_backing) {
        CharacterMotor* motor = localUser_ptr->cachedBody_backing->characterMotor_backing;

        if (flightControl->IsEnabled() && !isProvidingFlight) {
            motor->_flightParameters.channeledFlightGranterCount++;
            motor->_gravityParameters.channeledAntiGravityGranterCount++;

            // Manually trigger the same logic as the property setters
            // CheckShouldUseFlight: return channeledFlightGranterCount > 0
            motor->isFlying_backing = (motor->_flightParameters.channeledFlightGranterCount > 0);

            // CheckShouldUseGravity: return environmentalAntiGravityGranterCount <= 0 && (antiGravityNeutralizerCount > 0 || channeledAntiGravityGranterCount
            // <= 0)
            motor->useGravity_backing =
                (motor->_gravityParameters.environmentalAntiGravityGranterCount <= 0 &&
                 (motor->_gravityParameters.antiGravityNeutralizerCount > 0 || motor->_gravityParameters.channeledAntiGravityGranterCount <= 0));

            isProvidingFlight = true;
        } else if (!flightControl->IsEnabled() && isProvidingFlight) {
            motor->_flightParameters.channeledFlightGranterCount--;
            motor->_gravityParameters.channeledAntiGravityGranterCount--;

            // Manually trigger the same logic as the property setters
            // CheckShouldUseFlight: return channeledFlightGranterCount > 0
            motor->isFlying_backing = (motor->_flightParameters.channeledFlightGranterCount > 0);

            // CheckShouldUseGravity: return environmentalAntiGravityGranterCount <= 0 && (antiGravityNeutralizerCount > 0 || channeledAntiGravityGranterCount
            // <= 0)
            motor->useGravity_backing =
                (motor->_gravityParameters.environmentalAntiGravityGranterCount <= 0 &&
                 (motor->_gravityParameters.antiGravityNeutralizerCount > 0 || motor->_gravityParameters.channeledAntiGravityGranterCount <= 0));

            isProvidingFlight = false;
        }
    }

    huntressRangeControl->UpdateFreezeLogic();
    huntressFOVControl->UpdateFreezeLogic();

    primarySkillCooldownControl->UpdateFreezeLogic();
    secondarySkillCooldownControl->UpdateFreezeLogic();
    utilitySkillCooldownControl->UpdateFreezeLogic();
    specialSkillCooldownControl->UpdateFreezeLogic();

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
    Inventory* inventory_ptr = static_cast<Inventory*>(inventory);
    if (!inventory_ptr || !inventory_ptr->itemStacks) {
        return;
    }

    // Check if this is the local player's inventory
    if (!localUser_cached || !localUser_cached->cachedBody_backing || inventory != localUser_cached->cachedBody_backing->inventory_backing) {
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

            // Handle freeze logic based on freeze mode
            if (control->IsEnabled()) {
                IntControl* intControl = static_cast<IntControl*>(control.get());
                if (intControl->GetFreezeMode() == FreezeMode::HardLock) {
                    // HardLock: Force exact value
                    if (arrayData[i] != control->GetFrozenValue()) {
                        std::unique_lock<std::mutex> queueLock(queuedGiveItemsMutex);
                        queuedGiveItems.push(std::make_tuple(i, control->GetFrozenValue()));
                    }
                } else {
                    // MinimumValue: Only restore if below frozen value
                    if (arrayData[i] < control->GetFrozenValue()) {
                        std::unique_lock<std::mutex> queueLock(queuedGiveItemsMutex);
                        queuedGiveItems.push(std::make_tuple(i, control->GetFrozenValue()));
                    }
                }
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

        auto control = std::make_unique<IntControl>(item.displayName, "item_" + std::to_string(index), currentCount, 0, INT_MAX, 1, false, false, true);

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

std::string PlayerModule::GetCurrentBodyName() { return bodyTracker.GetCurrentBodyName(); }

void PlayerModule::UpdateCurrentBodyTracking() {
    if (!IsInGame() || !localUser_cached || !localUser_cached->cachedMaster_backing || !localUser_cached->cachedMaster_backing->bodyPrefab) {
        return;
    }

    GameObject* currentPrefab = localUser_cached->cachedMaster_backing->bodyPrefab;
    if (bodyTracker.HasBodyChanged(currentPrefab)) {
        int newIndex = bodyTracker.UpdateBody(currentPrefab);
        if (!lockCharacterModelControl->IsEnabled() && selectedCharacterIndexControl && newIndex != -1) {
            if (strlen(bodySearchFilter) > 0) {
                RefreshFilteredBodyList();
                // Current body is always first in filtered list when it exists
                selectedCharacterIndexControl->SetSelectedIndex(0);
            } else {
                selectedCharacterIndexControl->SetSelectedIndex(newIndex);
            }
        }
    }
}

void PlayerModule::RefreshFilteredBodyList() {
    if (bodyTracker.GetBodyNames().empty()) {
        return;
    }

    std::vector<std::string> filteredNames;
    std::string searchLower = bodySearchFilter;
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

    std::string currentName = bodyTracker.GetCurrentBodyName();
    if (!currentName.empty()) {
        filteredNames.push_back(currentName);
    }

    for (const auto& name : bodyTracker.GetBodyNames()) {
        if (name == currentName)
            continue;

        std::string nameLower = name;
        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
        if (nameLower.find(searchLower) != std::string::npos) {
            filteredNames.push_back(name);
        }
    }

    selectedCharacterIndexControl->SetItems(filteredNames);
}

void PlayerModule::OnHuntressTrackerStart(void* huntressTracker) {
    if (!huntressTracker)
        return;
    HuntressTracker* tracker = static_cast<HuntressTracker*>(huntressTracker);

    // Only cache player team trackers
    if (tracker->teamComponent && tracker->teamComponent->_teamIndex == TeamIndex_Value::Player) {

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
    CharacterBody* body = static_cast<CharacterBody*>(characterBody);
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

void PlayerModule::ApplyModelChange(const std::string& bodyName) {
    CharacterMaster* master = localUser_cached ? localUser_cached->cachedMaster_backing : nullptr;
    if (!master) {
        G::logger.LogError("ApplyModelChange: No master available");
        return;
    }

    GameObject* newPrefab = bodyTracker.GetPrefabByName(bodyName);
    if (!newPrefab) {
        G::logger.LogError("ApplyModelChange: Could not find prefab for body '%s'", bodyName.c_str());
        return;
    }
    G::logger.LogInfo("Transforming to %s", bodyName.c_str());
    G::gameFunctions->TransformCharacterBody(master, newPrefab);
}

bool PlayerModule::IsInGame() {
    return localUser_cached && localUser_cached->cachedMaster_backing && localUser_cached->cachedMaster_backing->resolvedBodyInstance;
}

GameObject* PlayerModule::GetSelectedBodyPrefab() {
    if (!selectedCharacterIndexControl || bodyTracker.GetBodyNames().empty()) {
        return nullptr;
    }

    std::string selectedName = selectedCharacterIndexControl->GetSelectedItem();
    return bodyTracker.GetPrefabByName(selectedName);
}

void PlayerModule::SetBodyPrefabsWithNames(const std::vector<std::pair<std::string, GameObject*>>& prefabsWithNames) {
    bodyTracker.Initialize(prefabsWithNames);

    if (selectedCharacterIndexControl && !bodyTracker.GetBodyNames().empty()) {
        selectedCharacterIndexControl->SetItems(bodyTracker.GetBodyNames());
        G::logger.LogInfo("SetBodyPrefabsWithNames: Updated dropdown with %zu bodies", bodyTracker.GetBodyNames().size());
    }
}
