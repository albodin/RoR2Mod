#include "ESPModule.h"
#include "globals/globals.h"
#include "hooks/hooks.h"
#include "utils/RenderUtils.h"
#include "fonts/FontManager.h"
#include <imgui.h>
#include <cmath>
#include <algorithm>
#include <cstdio>

ESPModule::ESPModule() : ModuleBase() {
    m_costFormatsInitialized = false;
    Initialize();
}

ESPModule::~ESPModule() {
    delete teleporterESPControl;
    delete playerESPControl;
    delete enemyESPControl;
    delete chestESPControl;
    delete shopESPControl;
    delete droneESPControl;
    delete shrineESPControl;
    delete specialESPControl;
    delete barrelESPControl;

    std::lock_guard<std::mutex> lock(interactablesMutex);
    for (auto interactable : trackedInteractables) {
        delete interactable;
    }
    trackedInteractables.clear();
}

void ESPModule::Initialize() {
    teleporterESPControl = new ESPControl("Teleporter ESP", "teleporter_esp", false,
                                    250.0f, 1000.0f, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow

    playerESPControl = new EntityESPControl("Players", "player_esp");
    enemyESPControl = new EntityESPControl("Enemies", "enemy_esp");
    chestESPControl = new ChestESPControl("Chests", "chest_esp");
    shopESPControl = new ChestESPControl("Shops & Printers", "shop_esp");
    droneESPControl = new ChestESPControl("Drones", "drone_esp");
    shrineESPControl = new ChestESPControl("Shrines", "shrine_esp");
    specialESPControl = new ChestESPControl("Special", "special_esp");
    barrelESPControl = new ChestESPControl("Barrels", "barrel_esp");
    itemPickupESPControl = new ChestESPControl("Item Pickups", "item_pickup_esp");
    portalESPControl = new ChestESPControl("Portals", "portal_esp");
}

void ESPModule::Update() {
    teleporterESPControl->Update();
    playerESPControl->Update();
    enemyESPControl->Update();
    chestESPControl->Update();
    shopESPControl->Update();
    droneESPControl->Update();
    shrineESPControl->Update();
    specialESPControl->Update();
    barrelESPControl->Update();

    // Clean up consumed item pickups
    {
        std::lock_guard<std::mutex> lock(interactablesMutex);
        trackedInteractables.erase(
            std::remove_if(trackedInteractables.begin(), trackedInteractables.end(),
                [](TrackedInteractable* tracked) {
                    if (tracked->category == InteractableCategory::ItemPickup && tracked->gameObject) {
                        // Check if the pickup has been consumed or recycled
                        GenericPickupController* gpc = static_cast<GenericPickupController*>(tracked->gameObject);
                        if (gpc->Recycled || gpc->consumed) {
                            delete tracked;
                            return true;
                        }
                    }
                    return false;
                }),
            trackedInteractables.end()
        );
    }
}

void ESPModule::DrawUI() {
    teleporterESPControl->Draw();
    playerESPControl->Draw();
    enemyESPControl->Draw();

    if (ImGui::CollapsingHeader("Interactables")) {
        chestESPControl->Draw();
        shopESPControl->Draw();
        droneESPControl->Draw();
        shrineESPControl->Draw();
        specialESPControl->Draw();
        barrelESPControl->Draw();
        itemPickupESPControl->Draw();
        portalESPControl->Draw();
    }
}

void ESPModule::OnFrameRender() {
    if (teleporterESPControl->IsEnabled()) {
        RenderTeleporterESP();
    }

    if (enemyESPControl->IsMasterEnabled()) {
        RenderEnemyESP();
    }

    if (playerESPControl->IsMasterEnabled()) {
        RenderPlayerESP();
    }

    RenderInteractablesESP();
}

void ESPModule::OnGameUpdate() {
    mainCamera = Hooks::Camera_get_main();
}

void ESPModule::OnTeleporterAwake(void* teleporter) {
    TeleporterInteraction* teleporter_ptr = (TeleporterInteraction*)teleporter;
    if (teleporter_ptr && teleporter_ptr->teleporterPositionIndicator) {
        Hooks::Transform_get_position_Injected(teleporter_ptr->teleporterPositionIndicator->targetTransform, &teleporterPosition);
        teleporterDisplayName = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("TELEPORTER_NAME"));
    }
}

void ESPModule::RenderTeleporterESP() {
    if (!mainCamera || !teleporterPosition || !G::localPlayer->GetPlayerPosition()) {
        return;
    }

    ImVec2 screenPos;
    if (!RenderUtils::WorldToScreen(mainCamera, teleporterPosition, screenPos)) {
        return;
    }

    float distance = teleporterPosition.Distance(G::localPlayer->GetPlayerPosition());
    if (distance > teleporterESPControl->GetDistance()) {
        return;
    }

    // Use the localized teleporter name that was set during OnTeleporterAwake
    std::string teleporterText = teleporterDisplayName.empty() ? "Teleporter" : teleporterDisplayName;
    teleporterText += " (";
    teleporterText += std::to_string((int)distance);
    teleporterText += "m)";

    RenderUtils::RenderText(screenPos, teleporterESPControl->GetColorU32(), teleporterESPControl->GetOutlineColorU32(), teleporterESPControl->IsOutlineEnabled(), true, "%s", teleporterText.c_str());
}

void ESPModule::OnCharacterBodySpawned(void* characterBody) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    CharacterBody* body = (CharacterBody*)characterBody;

    if (!body) return;

    TrackedEntity* newEntity = new TrackedEntity();
    newEntity->displayName = G::gameFunctions->Language_GetString((MonoString*)body->baseNameToken);
    newEntity->body = body;

    // For players, try to get the actual username
    if (body->teamComponent_backing && body->teamComponent_backing->_teamIndex == TeamIndex_Value::Player) {
        if (body->_master && body->_master->playerCharacterMasterController_backing) {
            PlayerCharacterMasterController* pcmc = body->_master->playerCharacterMasterController_backing;
            if (pcmc->resolvedNetworkUserInstance && pcmc->resolvedNetworkUserInstance->userName) {
                std::string playerName = G::g_monoRuntime->StringToUtf8((MonoString*)pcmc->resolvedNetworkUserInstance->userName);
                if (!playerName.empty()) {
                    newEntity->displayName = playerName;
                }
            }
        }
    }

    // Categorize by team
    switch (body->teamComponent_backing->_teamIndex) {
        case TeamIndex_Value::Monster:
        case TeamIndex_Value::Lunar:
        case TeamIndex_Value::Void:
            trackedEnemies.push_back(newEntity);
            break;
        case TeamIndex_Value::Player:
            trackedPlayers.push_back(newEntity);
            break;

        default:
            break;
    }
}

void ESPModule::OnCharacterBodyDestroyed(void* characterBody) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    CharacterBody* body = (CharacterBody*)characterBody;

    trackedEnemies.erase(
        std::remove_if(trackedEnemies.begin(), trackedEnemies.end(),
                      [body](TrackedEntity* enemy) {
                          return enemy->body == body;
                      }),
        trackedEnemies.end()
    );

    trackedPlayers.erase(
        std::remove_if(trackedPlayers.begin(), trackedPlayers.end(),
                      [body](TrackedEntity* player) {
                          return player->body == body;
                      }),
        trackedPlayers.end()
    );
}

void ESPModule::RenderPlayerESP() {
    if (!mainCamera || !G::localPlayer->GetPlayerPosition()) {
        return;
    }

    std::lock_guard<std::mutex> lock(entitiesMutex);
    Vector3 localPlayerPos = G::localPlayer->GetPlayerPosition();

    // Clean up invalid player pointers
    trackedPlayers.erase(
        std::remove_if(trackedPlayers.begin(), trackedPlayers.end(),
            [](TrackedEntity* entity) {
                return !entity->body || !entity->body->healthComponent_backing ||
                       entity->body->healthComponent_backing->health <= 0;
            }),
        trackedPlayers.end()
    );

    for (TrackedEntity* entity : trackedPlayers) {
        if (!entity->body || !entity->body->transform || !entity->body->healthComponent_backing) continue;

        if (entity->body == G::localPlayer->GetLocalPlayerBody()) continue;

        Vector3 playerWorldPos;
        Hooks::Transform_get_position_Injected(entity->body->transform, &playerWorldPos);

        float distance = playerWorldPos.Distance(G::localPlayer->GetPlayerPosition());

        bool isVisible = IsVisible(playerWorldPos);

        EntityESPSubControl* control = isVisible ?
            playerESPControl->GetVisibleControl() :
            playerESPControl->GetNonVisibleControl();

        if (!control->IsEnabled() || distance > control->GetMaxDistance()) {
            continue;
        }

        ImVec2 screenPos;
        bool onScreen = RenderUtils::WorldToScreen(mainCamera, playerWorldPos, screenPos);

        // Skip if entity is behind camera (off-screen and at default position)
        if (!onScreen && screenPos.x == 0.0f && screenPos.y == 0.0f) {
            continue;
        }

        RenderEntityESP(entity, screenPos, distance, control, isVisible, onScreen);
    }
}

void ESPModule::RenderEnemyESP() {
    if (!mainCamera || !G::localPlayer->GetPlayerPosition()) {
        return;
    }

    std::lock_guard<std::mutex> lock(entitiesMutex);
    Vector3 playerPos = G::localPlayer->GetPlayerPosition();

    // Clean up invalid enemy pointers
    trackedEnemies.erase(
        std::remove_if(trackedEnemies.begin(), trackedEnemies.end(),
            [](TrackedEntity* entity) {
                return !entity->body || !entity->body->healthComponent_backing ||
                       entity->body->healthComponent_backing->health <= 0;
            }),
        trackedEnemies.end()
    );

    for (TrackedEntity* entity : trackedEnemies) {
        if (!entity->body || !entity->body->transform || !entity->body->healthComponent_backing) continue;

        Vector3 enemyWorldPos;
        Hooks::Transform_get_position_Injected(entity->body->transform, &enemyWorldPos);

        float distance = enemyWorldPos.Distance(G::localPlayer->GetPlayerPosition());

        bool isVisible = IsVisible(enemyWorldPos);

        EntityESPSubControl* control = isVisible ?
            enemyESPControl->GetVisibleControl() :
            enemyESPControl->GetNonVisibleControl();

        if (!control->IsEnabled() || distance > control->GetMaxDistance()) {
            continue;
        }

        ImVec2 screenPos;
        bool onScreen = RenderUtils::WorldToScreen(mainCamera, enemyWorldPos, screenPos);

        // Skip if entity is behind camera (off-screen and at default position)
        if (!onScreen && screenPos.x == 0.0f && screenPos.y == 0.0f) {
            continue;
        }

        RenderEntityESP(entity, screenPos, distance, control, isVisible, onScreen);
    }
}

void ESPModule::RenderEntityESP(TrackedEntity* entity, ImVec2 screenPos, float distance, EntityESPSubControl* control, bool isVisible, bool onScreen) {
    if (!entity || !control) return;

    // If off-screen, only draw traceline and return
    if (!onScreen) {
        if (control->ShouldShowTraceline()) {
            ImVec2 screenCenter(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y);
            RenderUtils::RenderLine(screenCenter, screenPos, control->GetTracelineColorU32(), 1.0f);
        }
        return;
    }

    float lineHeight = FontManager::ESPFontSize;
    static float boxBorderThickness = 2.0f;

    ImVec2 screenMin(FLT_MAX, FLT_MAX);
    ImVec2 screenMax(-FLT_MAX, -FLT_MAX);
    bool boundsFound = false;

    if (entity->body->hurtBoxGroup_backing && entity->body->hurtBoxGroup_backing->hurtBoxes) {
        Vector3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        uint32_t len = static_cast<uint32_t>(((MonoArray_Internal*)entity->body->hurtBoxGroup_backing->hurtBoxes)->max_length);
        HurtBox** data = mono_array_addr<HurtBox*>((MonoArray_Internal*)entity->body->hurtBoxGroup_backing->hurtBoxes);

        for (uint32_t i = 0; i < len; ++i) {
            HurtBox* hurtBox = data[i];
            if (hurtBox) {
                Transform* hurtBoxTransform = (Transform*)Hooks::Component_get_transform(hurtBox);
                if (hurtBoxTransform) {
                    Vector3 hurtBoxPos;
                    Hooks::Transform_get_position_Injected(hurtBoxTransform, &hurtBoxPos);

                    if (!boundsFound) {
                        minBounds = hurtBoxPos;
                        maxBounds = hurtBoxPos;
                        boundsFound = true;
                    } else {
                        minBounds.x = std::min(minBounds.x, hurtBoxPos.x);
                        minBounds.y = std::min(minBounds.y, hurtBoxPos.y);
                        minBounds.z = std::min(minBounds.z, hurtBoxPos.z);
                        maxBounds.x = std::max(maxBounds.x, hurtBoxPos.x);
                        maxBounds.y = std::max(maxBounds.y, hurtBoxPos.y);
                        maxBounds.z = std::max(maxBounds.z, hurtBoxPos.z);
                    }
                }
            }
        }

        if (boundsFound) {
            Vector3 corners[8] = {
                Vector3(minBounds.x, minBounds.y, minBounds.z),
                Vector3(maxBounds.x, minBounds.y, minBounds.z),
                Vector3(minBounds.x, maxBounds.y, minBounds.z),
                Vector3(maxBounds.x, maxBounds.y, minBounds.z),
                Vector3(minBounds.x, minBounds.y, maxBounds.z),
                Vector3(maxBounds.x, minBounds.y, maxBounds.z),
                Vector3(minBounds.x, maxBounds.y, maxBounds.z),
                Vector3(maxBounds.x, maxBounds.y, maxBounds.z)
            };

            int visibleCorners = 0;

            for (int i = 0; i < 8; i++) {
                ImVec2 cornerScreen;
                if (RenderUtils::WorldToScreen(mainCamera, corners[i], cornerScreen)) {
                    if (visibleCorners == 0) {
                        screenMin = cornerScreen;
                        screenMax = cornerScreen;
                    } else {
                        screenMin.x = std::min(screenMin.x, cornerScreen.x);
                        screenMin.y = std::min(screenMin.y, cornerScreen.y);
                        screenMax.x = std::max(screenMax.x, cornerScreen.x);
                        screenMax.y = std::max(screenMax.y, cornerScreen.y);
                    }
                    visibleCorners++;
                }
            }

            if (visibleCorners == 0) {
                boundsFound = false;
            }
        }
    }

    // Fallback bounds if we couldn't calculate from hurtboxes or bounds are too small
    bool useFallbackBounds = false;
    if (!boundsFound) {
        useFallbackBounds = true;
    } else {
        // Check if bounds are too small (like wisps) - if box is smaller than 5x5 pixels, use fallback
        ImVec2 currentBoxSize(screenMax.x - screenMin.x, screenMax.y - screenMin.y);
        if (currentBoxSize.x < 5.0f || currentBoxSize.y < 5.0f) {
            useFallbackBounds = true;
        }
    }

    if (useFallbackBounds) {
        // Scale fallback bounds based on distance - smaller at far distances
        float distanceScale = std::max(0.3f, std::min(1.0f, 50.0f / distance)); // Scale from 1.0 at 50m to 0.3 at far distances
        float halfWidth = 30.0f * distanceScale;
        float halfHeight = 40.0f * distanceScale;
        screenMin = ImVec2(screenPos.x - halfWidth, screenPos.y - halfHeight);
        screenMax = ImVec2(screenPos.x + halfWidth, screenPos.y + halfHeight);
    }

    ImVec2 boxSize(screenMax.x - screenMin.x, screenMax.y - screenMin.y);
    ImVec2 boxBottomCenter(screenMin.x + boxSize.x / 2, screenMax.y);
    ImVec2 textPos = boxBottomCenter;
    textPos.y += 5; // Small gap below the box

    // Render traceline first so it appears under everything else
    if (control->ShouldShowTraceline()) {
        ImVec2 screenCenter(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y);
        RenderUtils::RenderLine(screenCenter, boxBottomCenter, control->GetTracelineColorU32(), 1.0f);
    }

    if (control->ShouldShowBox()) {
        RenderUtils::RenderBox(screenMin, boxSize, control->GetBoxColorU32(), boxBorderThickness);
    }

    if (control->ShouldShowName() && !entity->displayName.empty()) {
        RenderUtils::RenderText(textPos, control->GetNameColorU32(), IM_COL32(0, 0, 0, 255),
                              true, true, "%s", entity->displayName.c_str());
        textPos.y += lineHeight;
    }

    if (entity->body->healthComponent_backing) {
        bool showHealth = control->ShouldShowHealth();
        bool showMaxHealth = control->ShouldShowMaxHealth();

        if (showHealth && showMaxHealth) {
            std::string healthPart = "HP: " + std::to_string((int)entity->body->healthComponent_backing->health);
            std::string maxHealthPart = "/" + std::to_string((int)entity->body->maxHealth_backing);

            ImFont* font = FontManager::GetESPFont();
            float scale = FontManager::ESPFontSize / font->FontSize;
            ImVec2 healthPartSize = ImGui::CalcTextSize(healthPart.c_str());
            ImVec2 maxHealthPartSize = ImGui::CalcTextSize(maxHealthPart.c_str());
            healthPartSize.x *= scale;
            maxHealthPartSize.x *= scale;
            float totalWidth = healthPartSize.x + maxHealthPartSize.x;

            // Render health part (left side, adjusted for centering)
            ImVec2 healthPartPos = ImVec2(textPos.x - totalWidth / 2, textPos.y);
            RenderUtils::RenderText(healthPartPos, control->GetHealthColorU32(), IM_COL32(0, 0, 0, 255),
                                  true, false, healthPart.c_str());

            // Render max health part (right side)
            ImVec2 maxHealthPartPos = ImVec2(healthPartPos.x + healthPartSize.x, textPos.y);
            RenderUtils::RenderText(maxHealthPartPos, control->GetMaxHealthColorU32(), IM_COL32(0, 0, 0, 255),
                                  true, false, maxHealthPart.c_str());

            textPos.y += lineHeight;
        } else if (showHealth) {
            std::string healthText = "HP: " + std::to_string((int)entity->body->healthComponent_backing->health);
            RenderUtils::RenderText(textPos, control->GetHealthColorU32(), IM_COL32(0, 0, 0, 255),
                                  true, true, healthText.c_str());
            textPos.y += lineHeight;
        } else if (showMaxHealth) {
            std::string maxHealthText = "Max HP: " + std::to_string((int)entity->body->maxHealth_backing);
            RenderUtils::RenderText(textPos, control->GetMaxHealthColorU32(), IM_COL32(0, 0, 0, 255),
                                  true, true, maxHealthText.c_str());
            textPos.y += lineHeight;
        }
    }

    if (control->ShouldShowDistance()) {
        std::string distanceText = std::to_string((int)distance) + "m";
        RenderUtils::RenderText(textPos, control->GetDistanceColorU32(), IM_COL32(0, 0, 0, 255),
                              true, true, distanceText.c_str());
        textPos.y += lineHeight;
    }


    if (control->ShouldShowHealthbar() && entity->body->healthComponent_backing) {
        ImVec2 healthbarPos(screenMin.x - 8, screenMin.y - boxBorderThickness / 2);
        ImVec2 healthbarSize(8, boxSize.y + boxBorderThickness);
        float health = entity->body->healthComponent_backing->health;
        float maxHealth = entity->body->maxHealth_backing;

        ImU32 healthColor = health > maxHealth * 0.5f ? IM_COL32(0, 255, 0, 255) :
                           health > maxHealth * 0.25f ? IM_COL32(255, 255, 0, 255) :
                           IM_COL32(255, 0, 0, 255);

        RenderUtils::RenderHealthbar(healthbarPos, healthbarSize, health, maxHealth,
                                   healthColor, IM_COL32(50, 50, 50, 180));
    }
}

Vector3 ESPModule::GetCameraPosition() {
    Camera* camera = Hooks::Camera_get_main();
    if (!camera || !Hooks::Component_get_transform) {
        return Vector3{0, 0, 0};
    }

    void* transform = Hooks::Component_get_transform((void*)camera);
    if (!transform) {
        return Vector3{0, 0, 0};
    }

    Vector3 position;
    Hooks::Transform_get_position_Injected(transform, &position);
    return position;
}

bool ESPModule::IsVisible(const Vector3& position) {
    if (!Hooks::Physics_get_defaultPhysicsScene_Injected || !Hooks::PhysicsScene_Internal_Raycast_Injected) {
        return true;
    }

    Vector3 cameraPos = GetCameraPosition();
    Vector3 direction = position - cameraPos;
    float distance = direction.Length();

    if (distance < 0.1f) {
        return true;
    }

    direction.Normalize();

    PhysicsScene_Value defaultScene = {};
    Hooks::Physics_get_defaultPhysicsScene_Injected(&defaultScene);

    Ray_Value ray;
    ray.m_Origin = cameraPos;
    ray.m_Direction = direction;

    RaycastHit_Value hitInfo = {};

    int32_t layerMask = 0;
    if (G::worldLayer >= 0) layerMask |= (1 << G::worldLayer);
    if (G::ignoreRaycastLayer >= 0) layerMask |= (1 << G::ignoreRaycastLayer);

    bool hitSomething = Hooks::PhysicsScene_Internal_Raycast_Injected(
        &defaultScene,
        &ray,
        distance - 1.0f,
        &hitInfo,
        layerMask,
        QueryTriggerInteraction_Value::UseGlobal
    );

    return !hitSomething;
}

InteractableCategory ESPModule::DetermineInteractableCategory(PurchaseInteraction* pi, MonoString* nameToken) {
    // Check for shrines first
    if (pi->isShrine || pi->isGoldShrine) {
        return InteractableCategory::Shrine;
    }

    std::string token = "";
    if (nameToken) {
        token = G::g_monoRuntime->StringToUtf8(nameToken);
    }

    if (token.find("SHRINE_CLEANSE_") != std::string::npos ||  // Cleansing Pool
        token.find("DUPLICATOR_") != std::string::npos ||      // 3D Printer variants
        token.find("MULTISHOP_TERMINAL_") != std::string::npos ||
        token.find("BAZAAR_CAULDRON_") != std::string::npos ||
        token.find("SCRAPPER_") != std::string::npos) {
        return InteractableCategory::Shop;
    }

    if (token.find("SHRINE_") != std::string::npos) {
        return InteractableCategory::Shrine;
    }

    if (token.find("DRONE_") != std::string::npos ||
        token.find("TURRET1_") != std::string::npos) {
        return InteractableCategory::Drone;
    }

    if (token.find("PORTAL_") != std::string::npos &&
        token.find("PORTAL_DIALER") == std::string::npos) {
        return InteractableCategory::Portal;
    }

    // Chests - CHEST covers all variants (LUNAR_CHEST_, TIMEDCHEST_, etc.)
    if (token.find("CHEST") != std::string::npos ||
        token.find("LOCKBOX") != std::string::npos ||
        token.find("SCAVBACKPACK_") != std::string::npos) {
        return InteractableCategory::Chest;
    }

    if (token.find("BARREL") != std::string::npos) {
        return InteractableCategory::Barrel;
    }

    // Special interactables
    if (token.find("NEWT_STATUE_") != std::string::npos ||       // Newt Altar
        token.find("MOON_BATTERY_") != std::string::npos ||      // Pillars
        token.find("FROG_") != std::string::npos ||
        token.find("BAZAAR_SEER_") != std::string::npos ||       // Lunar Seer
        token.find("TELEPORTER_") != std::string::npos ||
        token.find("GOLDTOTEM_") != std::string::npos ||         // Halcyon Beacon
        token.find("MSOBELISK_") != std::string::npos ||         // Obelisk
        token.find("DEEPVOIDBATTERY_") != std::string::npos ||   // Deep Void Signal
        token.find("NULL_WARD_") != std::string::npos ||         // Cell Vent
        token.find("RADIOTOWER_") != std::string::npos ||        // Radio Scanner
        token.find("FAN_") != std::string::npos ||
        token.find("LOCKEDTREEBOT_") != std::string::npos ||     // Broken Robot
        token.find("PORTAL_DIALER_") != std::string::npos ||     // Compound Generator
        token.find("ARTIFACT_TRIAL_") != std::string::npos ||    // Artifact Reliquary
        token.find("LUNAR_TERMINAL_") != std::string::npos ||    // Lunar Buds
        token.find("LUNAR_REROLL_") != std::string::npos ||      // Lunar Shop Refresher
        token.find("LOCKEDMAGE_") != std::string::npos ||        // Survivor Suspended In Time
        token.find("ZIPLINE_") != std::string::npos ||           // Quantum Tunnel
        token.find("VENDING_MACHINE_") != std::string::npos ||
        token.find("GEODE_") != std::string::npos) {             // Aurelionite Geode
        return InteractableCategory::Special;
    }

    // Everything else is unknown
    return InteractableCategory::Unknown;
}


void ESPModule::OnPurchaseInteractionSpawned(void* purchaseInteraction) {
    if (!purchaseInteraction) return;

    PurchaseInteraction* pi = (PurchaseInteraction*)purchaseInteraction;

    void* gameObject = purchaseInteraction;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(purchaseInteraction);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    std::string displayName = G::gameFunctions->Language_GetString((MonoString*)pi->displayNameToken);

    // Skip interactables with empty or whitespace-only names
    if (displayName.empty() || displayName.find_first_not_of(" \t\r\n") == std::string::npos) {
        return;
    }

    // Determine category using language-independent token
    InteractableCategory category = DetermineInteractableCategory(pi, (MonoString*)pi->displayNameToken);

    // Log error if unknown interactable found
    if (category == InteractableCategory::Unknown) {
        std::string token = "";
        if (pi->displayNameToken) {
            token = G::g_monoRuntime->StringToUtf8((MonoString*)pi->displayNameToken);
        }
        G::logger.LogError("Unknown PurchaseInteraction detected: token=\"" + token + "\" display=\"" + displayName + "\" cost=" + std::to_string(pi->cost) + " isShrine=" + std::to_string(pi->isShrine) + " - Please report this to the developers!");
        displayName += " (UNKNOWN)";
    }

    // Create interactable tracking info
    TrackedInteractable* trackedInteractable = new TrackedInteractable();
    trackedInteractable->gameObject = gameObject;
    trackedInteractable->purchaseInteraction = purchaseInteraction;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = "";
    if (pi->displayNameToken) {
        trackedInteractable->nameToken = G::g_monoRuntime->StringToUtf8((MonoString*)pi->displayNameToken);
    }
    trackedInteractable->category = category;
    trackedInteractable->consumed = false;
    trackedInteractable->pickupIndex = -1;
    trackedInteractable->itemName = "";

    // Get the localized cost string
    trackedInteractable->costString = "";
    if (pi->cost > 0) {
        trackedInteractable->costString = GetCostString(pi->costType, pi->cost);
    }

    // Add to tracked interactables
    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(trackedInteractable);

    const char* categoryName = "";
    switch(category) {
        case InteractableCategory::Chest: categoryName = "Chest"; break;
        case InteractableCategory::Shop: categoryName = "Shop"; break;
        case InteractableCategory::Drone: categoryName = "Drone"; break;
        case InteractableCategory::Shrine: categoryName = "Shrine"; break;
        case InteractableCategory::Special: categoryName = "Special"; break;
        case InteractableCategory::Barrel: categoryName = "Barrel"; break;
        case InteractableCategory::ItemPickup: categoryName = "ItemPickup"; break;
        case InteractableCategory::Portal: categoryName = "Portal"; break;
        case InteractableCategory::Unknown: categoryName = "Unknown"; break;
    }
}

void ESPModule::OnPurchaseInteractionDestroyed(void* purchaseInteraction) {
    if (!purchaseInteraction) return;

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.erase(
        std::remove_if(trackedInteractables.begin(), trackedInteractables.end(),
                      [purchaseInteraction](TrackedInteractable* tracked) {
                          if (tracked->gameObject == purchaseInteraction) {
                              delete tracked;
                              return true;
                          }
                          return false;
                      }),
        trackedInteractables.end()
    );
}

void ESPModule::OnBarrelInteractionSpawned(void* barrelInteraction) {
    if (!barrelInteraction) return;

    BarrelInteraction* barrel = (BarrelInteraction*)barrelInteraction;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(barrelInteraction);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    std::string displayName = G::gameFunctions->Language_GetString((MonoString*)barrel->displayNameToken);
    // Create interactable tracking info for barrels
    TrackedInteractable* trackedInteractable = new TrackedInteractable();
    trackedInteractable->gameObject = barrelInteraction;
    trackedInteractable->purchaseInteraction = nullptr; // Barrels don't use PurchaseInteraction
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = "";
    if (barrel->displayNameToken) {
        trackedInteractable->nameToken = G::g_monoRuntime->StringToUtf8((MonoString*)barrel->displayNameToken);
    }
    trackedInteractable->category = InteractableCategory::Barrel;
    trackedInteractable->consumed = false;
    trackedInteractable->costString = "";

    // Add to tracked interactables
    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(trackedInteractable);
}

void ESPModule::OnGenericInteractionSpawned(void* genericInteraction) {
    if (!genericInteraction) return;

    GenericInteraction* gi = (GenericInteraction*)genericInteraction;

    // Use the component pointer as the identifier
    void* gameObject = genericInteraction;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(genericInteraction);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    // Get display name - GenericInteraction uses contextToken instead of displayNameToken
    std::string displayName = "Generic Interaction"; // Fallback only
    if (gi->contextToken) {
        displayName = G::gameFunctions->Language_GetString((MonoString*)gi->contextToken);
    }

    // Skip interactables with empty or whitespace-only names
    if (displayName.empty() || displayName.find_first_not_of(" \t\r\n") == std::string::npos) {
        return;
    }

    // Convert token to string for filtering (language-independent)
    std::string token = "";
    if (gi->contextToken) {
        token = G::g_monoRuntime->StringToUtf8((MonoString*)gi->contextToken);
    }

    // Filter out unwanted interactions using tokens
    if (token == "SURVIVOR_POD_OPEN_PANEL_CONTEXT" ||      // "Open Panel"
        token == "PORTAL_DIALER_CONTEXT" ||                // "Cycle Compound"
        token == "LUNAR_TELEPORTER_SHIFT") {               // "Shift Destination"
        return;
    }

    InteractableCategory category = InteractableCategory::Special;

    if (token.find("PORTAL_") != std::string::npos &&
        token.find("PORTAL_DIALER") == std::string::npos) {
        category = InteractableCategory::Portal;
    }

    // Create tracking info
    TrackedInteractable* trackedInteractable = new TrackedInteractable();
    trackedInteractable->gameObject = gameObject;
    trackedInteractable->purchaseInteraction = nullptr;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = token;
    trackedInteractable->category = category;
    trackedInteractable->consumed = false;
    trackedInteractable->costString = "";

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(trackedInteractable);
}

void ESPModule::OnGenericPickupControllerSpawned(void* genericPickupController) {
    if (!genericPickupController) return;

    GenericPickupController* gpc = (GenericPickupController*)genericPickupController;

    // Use the component pointer as the identifier
    void* gameObject = genericPickupController;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(genericPickupController);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    // Don't track pickups with invalid pickup index
    if (gpc->pickupIndex <= 0) {
        return;
    }

    std::string displayName = "Item Pickup"; // Fallback only

    PickupDef* pickupDef = G::gameFunctions->GetPickupDef(gpc->pickupIndex);
    if (pickupDef && pickupDef->nameToken) {
        displayName = G::gameFunctions->Language_GetString((MonoString*)pickupDef->nameToken);
    } else {
        // If not found, show the pickup index for debugging
        displayName = "Item Pickup [" + std::to_string(gpc->pickupIndex) + "]";
    }

    // Create tracking info - categorize as item pickup
    TrackedInteractable* trackedInteractable = new TrackedInteractable();
    trackedInteractable->gameObject = gameObject;
    trackedInteractable->purchaseInteraction = nullptr;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = ""; // Item pickups don't have name tokens
    trackedInteractable->category = InteractableCategory::ItemPickup;
    trackedInteractable->consumed = false;
    trackedInteractable->costString = "";

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(trackedInteractable);
}

void ESPModule::OnTimedChestControllerSpawned(void* timedChestController) {
    if (!timedChestController) return;

    TimedChestController* tcc = (TimedChestController*)timedChestController;

    void* gameObject = timedChestController;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(timedChestController);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    std::string displayName = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("TIMEDCHEST_NAME"));
    void* purchaseInteraction = nullptr;

    // Create tracking info
    TrackedInteractable* trackedInteractable = new TrackedInteractable();
    trackedInteractable->gameObject = gameObject;
    trackedInteractable->purchaseInteraction = purchaseInteraction;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = "TIMEDCHEST_NAME";
    trackedInteractable->category = InteractableCategory::Chest;
    trackedInteractable->consumed = false;
    trackedInteractable->costString = "";

    {
        std::lock_guard<std::mutex> lock(interactablesMutex);
        trackedInteractables.push_back(trackedInteractable);
    }
}

void ESPModule::OnTimedChestControllerDespawned(void* timedChestController) {
    if (!timedChestController) return;

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.erase(
        std::remove_if(trackedInteractables.begin(), trackedInteractables.end(),
                      [timedChestController](TrackedInteractable* tracked) {
                          if (tracked->gameObject == timedChestController) {
                              delete tracked;
                              return true;
                          }
                          return false;
                      }),
        trackedInteractables.end()
    );
}

void ESPModule::OnPickupPickerControllerSpawned(void* pickupPickerController) {
    // TODO: Implement command selectors but don't show when inside a chest
    return;
}

void ESPModule::OnScrapperControllerSpawned(void* scrapperController) {
    if (!scrapperController) return;

    void* gameObject = scrapperController;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(scrapperController);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    std::string displayName = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("SCRAPPER_NAME"));

    // Create tracking info
    TrackedInteractable* trackedInteractable = new TrackedInteractable();
    trackedInteractable->gameObject = gameObject;
    trackedInteractable->purchaseInteraction = nullptr;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = "SCRAPPER_NAME";
    trackedInteractable->category = InteractableCategory::Shop;
    trackedInteractable->consumed = false;
    trackedInteractable->costString = "";

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(trackedInteractable);
}

void ESPModule::OnStageStart(void* stage) {
    // New stage detected - clear all tracked interactables and entities
    {
        std::lock_guard<std::mutex> lock(interactablesMutex);
        size_t interactableCount = trackedInteractables.size();
        for (auto interactable : trackedInteractables) {
            delete interactable;
        }
        trackedInteractables.clear();
    }

    {
        std::lock_guard<std::mutex> lock(entitiesMutex);
        for (auto entity : trackedEnemies) {
            delete entity;
        }
        trackedEnemies.clear();

        for (auto entity : trackedPlayers) {
            delete entity;
        }
        trackedPlayers.clear();
    }

    // Reset teleporter position and display name
    teleporterPosition = Vector3{0, 0, 0};
    teleporterDisplayName = "";
}

void ESPModule::RenderInteractablesESP() {
    if (!mainCamera) return;

    std::lock_guard<std::mutex> lock(interactablesMutex);

    for (auto interactable : trackedInteractables) {
        if (!interactable || !interactable->gameObject) continue;

        // Determine which control to use based on category
        ChestESPControl* categoryControl = nullptr;
        switch(interactable->category) {
            case InteractableCategory::Chest: categoryControl = chestESPControl; break;
            case InteractableCategory::Shop: categoryControl = shopESPControl; break;
            case InteractableCategory::Drone: categoryControl = droneESPControl; break;
            case InteractableCategory::Shrine: categoryControl = shrineESPControl; break;
            case InteractableCategory::Special: categoryControl = specialESPControl; break;
            case InteractableCategory::Barrel: categoryControl = barrelESPControl; break;
            case InteractableCategory::ItemPickup: categoryControl = itemPickupESPControl; break;
            case InteractableCategory::Portal: categoryControl = portalESPControl; break;
            case InteractableCategory::Unknown: categoryControl = specialESPControl; break; // Use special control for unknown
        }

        if (!categoryControl || !categoryControl->IsMasterEnabled()) continue;

        float distance = interactable->position.Distance(G::localPlayer->GetPlayerPosition());

        ChestESPSubControl* control = categoryControl->GetSubControl();

        if (!control->IsEnabled()) continue;

        // Check availability dynamically
        bool isAvailable = true;
        if (interactable->category == InteractableCategory::Barrel && interactable->gameObject) {
            BarrelInteraction* barrel = (BarrelInteraction*)interactable->gameObject;
            isAvailable = !barrel->opened;
        } else if (interactable->category == InteractableCategory::Chest &&
                   interactable->nameToken.find("TIMEDCHEST_") != std::string::npos &&
                   interactable->gameObject) {
            TimedChestController* tcc = (TimedChestController*)interactable->gameObject;
            isAvailable = !tcc->purchased;
            UpdateTimedChestDisplayName(interactable, interactable->gameObject);
        } else if (interactable->category == InteractableCategory::Special &&
                   interactable->nameToken.find("PRESSURE_PLATE_") != std::string::npos &&
                   interactable->gameObject) {
            UpdatePressurePlateDisplayName(interactable, interactable->gameObject);
            isAvailable = true; // Pressure plates are always "available" to interact with
        } else if (interactable->purchaseInteraction) {
            PurchaseInteraction* pi = (PurchaseInteraction*)interactable->purchaseInteraction;
            isAvailable = pi->available;

            // Update cost string if it has changed
            if (pi->cost > 0) {
                std::string newCostString = GetCostString(pi->costType, pi->cost);
                if (newCostString != interactable->costString) {
                    interactable->costString = newCostString;
                }
            }
        }

        if (!isAvailable && !control->ShouldShowUnavailable()) continue;
        if (distance > control->GetMaxDistance()) continue;

        Vector3 screenPos3D;
        Hooks::Camera_WorldToScreenPoint_Injected(mainCamera, &interactable->position,
                                                MonoOrStereoscopicEye::Mono, &screenPos3D);

        if (screenPos3D.z <= 0) continue;

        ImVec2 screenPos(screenPos3D.x, ImGui::GetIO().DisplaySize.y - screenPos3D.y);

        // Check if interactable is visible
        bool isVisible = IsVisible(interactable->position);
        bool onScreen = screenPos.x >= 0 && screenPos.x <= ImGui::GetIO().DisplaySize.x &&
                       screenPos.y >= 0 && screenPos.y <= ImGui::GetIO().DisplaySize.y;

        RenderInteractableESP(interactable, screenPos, distance, control, isVisible, onScreen, isAvailable);
    }
}

void ESPModule::UpdateTimedChestDisplayName(TrackedInteractable* interactable, void* timedChestController) {
    if (!timedChestController) return;

    TimedChestController* tcc = (TimedChestController*)timedChestController;

    // Use the original display name that was already localized when the interactable was created
    std::string baseName = interactable->displayName;
    // Remove any previous status suffixes
    size_t bracketPos = baseName.find(" [");
    if (bracketPos != std::string::npos) {
        baseName = baseName.substr(0, bracketPos);
    }

    // Calculate time remaining
    if (tcc->lockTime > 0 && !tcc->purchased) {
        float currentTime = G::gameFunctions->GetRunStopwatch();
        float timeRemaining = tcc->lockTime - currentTime;

        // Format time as MM:SS (can be negative)
        bool isNegative = timeRemaining < 0;
        float absTime = isNegative ? -timeRemaining : timeRemaining;

        int minutes = static_cast<int>(absTime) / 60;
        int seconds = static_cast<int>(absTime) % 60;

        char timeStr[16];
        if (isNegative) {
            snprintf(timeStr, sizeof(timeStr), " [-%02d:%02d]", minutes, seconds);
        } else {
            snprintf(timeStr, sizeof(timeStr), " [%02d:%02d]", minutes, seconds);
        }
        baseName += timeStr;
    }

    // Update if purchased
    if (tcc->purchased) {
        baseName += " [OPENED]";
    }

    interactable->displayName = baseName;
}

void ESPModule::UpdatePressurePlateDisplayName(TrackedInteractable* interactable, void* pressurePlateController) {
    if (!pressurePlateController) return;

    PressurePlateController* ppc = (PressurePlateController*)pressurePlateController;

    // Use the original display name that was already localized when the interactable was created
    std::string baseName = interactable->displayName;
    // Remove any previous status suffixes
    size_t parenPos = baseName.find(" (");
    if (parenPos != std::string::npos) {
        baseName = baseName.substr(0, parenPos);
    }

    if (ppc->switchDown) {
        baseName += " (Active)";
    } else {
        baseName += " (Inactive)";
    }

    interactable->displayName = baseName;
}

void ESPModule::RenderInteractableESP(TrackedInteractable* interactable, ImVec2 screenPos, float distance,
                                     ChestESPSubControl* control, bool isVisible, bool onScreen, bool isAvailable) {
    if (!interactable || !control->IsEnabled()) return;

    // If off-screen, only draw traceline and return
    if (!onScreen) {
        if (control->ShouldShowTraceline()) {
            ImVec2 screenCenter(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y);
            RenderUtils::RenderLine(screenCenter, screenPos, control->GetTracelineColorU32(), 1.0f);
        }
        return;
    }

    float fontSize = ImGui::GetFont()->FontSize;

    if (control->ShouldShowTraceline()) {
        ImVec2 startPos(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y);
        RenderUtils::RenderLine(startPos, screenPos, control->GetTracelineColorU32(), 1.0f);
    }

    float yOffset = 0;

    // Draw interactable name with optional distance
    if (control->ShouldShowName()) {
        std::string nameText = interactable->displayName;

        if (control->ShouldShowDistance()) {
            nameText += " (" + std::to_string((int)distance) + "m)";
        }

        if (!isAvailable) {
            nameText += " (Unavailable)";
        }

        ImVec2 textPos = RenderUtils::RenderText(ImVec2(screenPos.x, screenPos.y - yOffset),
                                                 control->GetNameColorU32(),
                                                 control->GetNameShadowColorU32(),
                                                 control->IsNameShadowEnabled(),
                                                 true,  // Center text
                                                 "%s", nameText.c_str());
        yOffset += fontSize + 2;

        // Show item name for chests and shops if available
        if (!interactable->itemName.empty() &&
            (interactable->category == InteractableCategory::Chest ||
             interactable->category == InteractableCategory::Shop)) {
            std::string itemText = "[" + interactable->itemName + "]";
            RenderUtils::RenderText(ImVec2(screenPos.x, screenPos.y - yOffset),
                                   IM_COL32(255, 215, 0, 255), // Gold color for items
                                   control->GetNameShadowColorU32(),
                                   control->IsNameShadowEnabled(),
                                   true,  // Center text
                                   "%s", itemText.c_str());
            yOffset += fontSize + 2;
        }
    }

    // Draw cost/reward info
    if (control->ShouldShowCost()) {
        std::string rewardText;

        // Special handling for barrels - show gold and XP rewards
        if (interactable->category == InteractableCategory::Barrel) {
            // Try to cast back to BarrelInteraction to get rewards
            if (interactable->purchaseInteraction == nullptr && interactable->gameObject) {
                BarrelInteraction* barrel = (BarrelInteraction*)interactable->gameObject;
                if (barrel->goldReward > 0 || barrel->expReward > 0) {
                    rewardText = "$" + std::to_string(barrel->goldReward) + " + " + std::to_string(barrel->expReward) + " XP";
                }
            }
        } else if (interactable->purchaseInteraction) {
            // Use the cached cost string that was localized when the interactable was created
            if (!interactable->costString.empty()) {
                rewardText = interactable->costString;
            }
        }

        if (!rewardText.empty()) {
            ImVec2 textPos = RenderUtils::RenderText(ImVec2(screenPos.x, screenPos.y - yOffset),
                                                     control->GetCostColorU32(),
                                                     control->GetCostShadowColorU32(),
                                                     control->IsCostShadowEnabled(),
                                                     true,  // Center text
                                                     "%s", rewardText.c_str());
            yOffset += fontSize + 2;
        }
    }

}

void ESPModule::OnChestBehaviorSpawned(void* chestBehavior) {
    // TODO: See if we can use this for anything
    return;
}

void ESPModule::OnShopTerminalBehaviorSpawned(void* shopTerminalBehavior) {
    if (!shopTerminalBehavior) return;

    ShopTerminalBehavior* shop = (ShopTerminalBehavior*)shopTerminalBehavior;

    Vector3 shopPos = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(shopTerminalBehavior);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &shopPos);
        }
    }

    // Find the corresponding tracked interactable by position
    std::lock_guard<std::mutex> lock(interactablesMutex);
    for (auto& tracked : trackedInteractables) {
        // Check if positions match
        if (tracked->position == shopPos && tracked->category == InteractableCategory::Shop) {
            // Get the pickup index from the shop
            if (shop->pickupIndex != -1) {
                tracked->pickupIndex = shop->pickupIndex;
                // Get pickup name from pickup index using PickupCatalog
                PickupDef* pickupDef = G::gameFunctions->GetPickupDef(shop->pickupIndex);
                if (pickupDef && pickupDef->nameToken) {
                    tracked->itemName = G::gameFunctions->Language_GetString((MonoString*)pickupDef->nameToken);
                } else {
                    tracked->itemName = "Unknown [" + std::to_string(shop->pickupIndex) + "]";
                }
            }
            break;
        }
    }
}

void ESPModule::OnPressurePlateControllerSpawned(void* pressurePlateController) {
    if (!pressurePlateController) return;

    PressurePlateController* ppc = (PressurePlateController*)pressurePlateController;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(pressurePlateController);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    // Note: Pressure plates don't seem to have standard language tokens, they're dynamic
    std::string displayName = "Pressure Plate"; // TODO: Find proper localization

    if (ppc->switchDown) {
        displayName += " (Active)";
    } else {
        displayName += " (Inactive)";
    }

    // Create tracking info - categorize as special interactable
    TrackedInteractable* trackedInteractable = new TrackedInteractable();
    trackedInteractable->gameObject = pressurePlateController;
    trackedInteractable->purchaseInteraction = nullptr;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = "PRESSURE_PLATE_DYNAMIC"; // Mark as dynamic
    trackedInteractable->category = InteractableCategory::Special;
    trackedInteractable->consumed = false;
    trackedInteractable->pickupIndex = -1;
    trackedInteractable->itemName = "";
    trackedInteractable->costString = "";

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(trackedInteractable);
}

void ESPModule::InitializeCostFormats() {
    if (m_costFormatsInitialized) return;

    G::logger.LogInfo("Initializing cost formats...");

    m_moneyFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_MONEY_FORMAT"));
    m_percentHealthFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_PERCENTHEALTH_FORMAT"));
    m_itemFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_ITEM_FORMAT"));
    m_lunarFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_LUNAR_FORMAT"));
    m_equipmentFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_EQUIPMENT_FORMAT"));
    m_volatileBatteryFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_VOLATILEBATTERY_FORMAT"));
    m_artifactKeyFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_ARTIFACTSHELLKILLERITEM_FORMAT"));
    m_rustedKeyFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_TREASURECACHEITEM_FORMAT"));
    m_encrustedKeyFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_TREASURECACHEVOIDITEM_FORMAT"));
    m_lunarCoinName = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("PICKUP_LUNAR_COIN"));
    m_voidCoinName = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("PICKUP_VOID_COIN"));

    m_costFormatsInitialized = true;
    G::logger.LogInfo("Cost formats initialized successfully");
}

std::string ESPModule::GetCostString(CostTypeIndex_Value costType, int cost) {
    if (!m_costFormatsInitialized) {
        InitializeCostFormats();
    }

    std::string format;

    switch (costType) {
        case CostTypeIndex_Value::None:
            return std::to_string(cost);

        case CostTypeIndex_Value::Money:
            format = m_moneyFormat;
            break;

        case CostTypeIndex_Value::PercentHealth:
            format = m_percentHealthFormat;
            break;

        case CostTypeIndex_Value::LunarCoin:
            return std::to_string(cost) + " " + m_lunarCoinName;

        case CostTypeIndex_Value::WhiteItem:
        case CostTypeIndex_Value::GreenItem:
        case CostTypeIndex_Value::RedItem:
        case CostTypeIndex_Value::BossItem:
            format = m_itemFormat;
            break;

        case CostTypeIndex_Value::LunarItemOrEquipment:
            format = m_lunarFormat;
            break;

        case CostTypeIndex_Value::Equipment:
            format = m_equipmentFormat;
            break;

        case CostTypeIndex_Value::VolatileBattery:
            format = m_volatileBatteryFormat;
            break;

        case CostTypeIndex_Value::ArtifactShellKillerItem:
            format = m_artifactKeyFormat;
            break;

        case CostTypeIndex_Value::TreasureCacheItem:
            format = m_rustedKeyFormat;
            break;

        case CostTypeIndex_Value::TreasureCacheVoidItem:
            format = m_encrustedKeyFormat;
            break;

        case CostTypeIndex_Value::VoidCoin:
            return std::to_string(cost) + " " + m_voidCoinName;

        default:
            G::logger.LogWarning("Unknown cost type: %d", (int)costType);
            return std::to_string(cost) + " Unknown";
    }

    // Replace {0} with the cost value
    size_t pos = format.find("{0}");
    if (pos != std::string::npos) {
        format.replace(pos, 3, std::to_string(cost));
    }

    return format;
}