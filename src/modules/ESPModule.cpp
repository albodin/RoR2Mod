#include "ESPModule.h"
#include "globals/globals.h"
#include "hooks/hooks.h"
#include "utils/RenderUtils.h"
#include "fonts/FontManager.h"
#include <imgui.h>
#include <cmath>
#include <algorithm>

ESPModule::ESPModule() : ModuleBase() {
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

    RenderUtils::RenderText(screenPos, teleporterESPControl->GetColorU32(), teleporterESPControl->GetOutlineColorU32(), teleporterESPControl->IsOutlineEnabled(), true, "Teleporter (%.1fm)", distance);
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

        float distance = playerWorldPos.Distance(GetCameraPosition());

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

        float distance = enemyWorldPos.Distance(GetCameraPosition());

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
                              true, true, entity->displayName.c_str());
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

InteractableCategory ESPModule::DetermineInteractableCategory(PurchaseInteraction* pi, const std::string& displayName) {
    // Check for shrines first
    if (pi->isShrine || pi->isGoldShrine) {
        return InteractableCategory::Shrine;
    }

    // Check display name for categorization
    if (displayName.find("Shrine") != std::string::npos) {
        return InteractableCategory::Shrine;
    }

    // Drones
    if (displayName.find("Drone") != std::string::npos ||
        displayName.find("Turret") != std::string::npos ||
        displayName.find("TC-280") != std::string::npos) {
        return InteractableCategory::Drone;
    }

    // Shops and Printers
    if (displayName.find("3D Printer") != std::string::npos ||
        displayName.find("Multishop Terminal") != std::string::npos ||
        displayName.find("Cauldron") != std::string::npos ||
        displayName.find("Scrapper") != std::string::npos ||
        displayName.find("Cleansing Pool") != std::string::npos) {
        return InteractableCategory::Shop;
    }

    // Special interactables
    if (displayName.find("Newt Altar") != std::string::npos ||
        displayName.find("Pillar") != std::string::npos ||
        displayName.find("Frog") != std::string::npos ||
        displayName.find("Portal") != std::string::npos ||
        displayName.find("Lunar Seer") != std::string::npos ||
        displayName.find("Teleporter") != std::string::npos ||
        displayName.find("Halcyon") != std::string::npos ||
        displayName.find("Obelisk") != std::string::npos ||
        displayName.find("Deep Void Signal") != std::string::npos ||
        displayName.find("Cell Vent") != std::string::npos ||
        displayName == "") { // Empty name items (rare)
        return InteractableCategory::Special;
    }

    // Everything else is a chest (including barrels, lockboxes, etc.)
    return InteractableCategory::Chest;
}


void ESPModule::OnPurchaseInteractionSpawned(void* purchaseInteraction) {
    if (!purchaseInteraction) return;

    PurchaseInteraction* pi = (PurchaseInteraction*)purchaseInteraction;

    // Don't skip shrines anymore - we'll categorize everything

    // Get the GameObject from the PurchaseInteraction (it's a MonoBehaviour, so we can get the gameObject)
    // For now, we'll use the PurchaseInteraction pointer as the GameObject identifier
    void* gameObject = purchaseInteraction; // This will need proper implementation

    // Get position from transform
    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(purchaseInteraction);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    // Get display name from the token using Language_GetString
    std::string displayName = G::gameFunctions->Language_GetString((MonoString*)pi->displayNameToken);

    // Determine category
    InteractableCategory category = DetermineInteractableCategory(pi, displayName);

    // Create interactable tracking info
    TrackedInteractable* trackedInteractable = new TrackedInteractable();
    trackedInteractable->gameObject = gameObject;
    trackedInteractable->purchaseInteraction = purchaseInteraction;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->category = category;

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

    // Get position from transform
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
    trackedInteractable->category = InteractableCategory::Barrel; // Categorize as barrel

    // Add to tracked interactables
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

    // Reset teleporter position
    teleporterPosition = Vector3{0, 0, 0};
    G::logger.LogInfo("Stage started - ESP data cleared");
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
        }

        if (!categoryControl || !categoryControl->IsMasterEnabled()) continue;

        float distance = interactable->position.Distance(GetCameraPosition());

        // Get the control
        ChestESPSubControl* control = categoryControl->GetSubControl();

        if (!control->IsEnabled()) continue;

        // Check availability dynamically
        bool isAvailable = true;
        if (interactable->category == InteractableCategory::Barrel && interactable->gameObject) {
            BarrelInteraction* barrel = (BarrelInteraction*)interactable->gameObject;
            isAvailable = !barrel->opened;
        } else if (interactable->purchaseInteraction) {
            PurchaseInteraction* pi = (PurchaseInteraction*)interactable->purchaseInteraction;
            isAvailable = pi->available;
        }

        // Skip unavailable if not showing them
        if (!isAvailable && !control->ShouldShowUnavailable()) continue;

        if (distance > control->GetMaxDistance()) continue;

        // Convert world position to screen
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

        // Add distance if enabled
        if (control->ShouldShowDistance()) {
            nameText += " (" + std::to_string((int)distance) + "m)";
        }

        // Add unavailable suffix if needed
        if (!isAvailable) {
            nameText += " (Unavailable)";
        }

        ImVec2 textPos = RenderUtils::RenderText(ImVec2(screenPos.x, screenPos.y - yOffset),
                                                 control->GetNameColorU32(),
                                                 control->GetNameShadowColorU32(),
                                                 control->IsNameShadowEnabled(),
                                                 true,  // Center text
                                                 nameText.c_str());
        yOffset += fontSize + 2;
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
            // Get cost info from PurchaseInteraction
            PurchaseInteraction* pi = (PurchaseInteraction*)interactable->purchaseInteraction;
            if (pi->cost > 0) {
                switch(pi->costType) {
                    case CostTypeIndex_Value::None:
                        rewardText = "Free";
                        break;
                    case CostTypeIndex_Value::Money:
                        rewardText = "$" + std::to_string(pi->cost);
                        break;
                    case CostTypeIndex_Value::PercentHealth:
                        rewardText = std::to_string(pi->cost) + "% Health";
                        break;
                    case CostTypeIndex_Value::LunarCoin:
                        rewardText = std::to_string(pi->cost) + " Lunar " + (pi->cost == 1 ? "Coin" : "Coins");
                        break;
                    case CostTypeIndex_Value::WhiteItem:
                        rewardText = std::to_string(pi->cost) + " White " + (pi->cost == 1 ? "Item" : "Items");
                        break;
                    case CostTypeIndex_Value::GreenItem:
                        rewardText = std::to_string(pi->cost) + " Green " + (pi->cost == 1 ? "Item" : "Items");
                        break;
                    case CostTypeIndex_Value::RedItem:
                        rewardText = std::to_string(pi->cost) + " Red " + (pi->cost == 1 ? "Item" : "Items");
                        break;
                    case CostTypeIndex_Value::Equipment:
                        rewardText = std::to_string(pi->cost) + " Equipment";
                        break;
                    case CostTypeIndex_Value::VolatileBattery:
                        rewardText = std::to_string(pi->cost) + " Volatile " + (pi->cost == 1 ? "Battery" : "Batteries");
                        break;
                    case CostTypeIndex_Value::LunarItemOrEquipment:
                        rewardText = std::to_string(pi->cost) + " Lunar Item/Equipment";
                        break;
                    case CostTypeIndex_Value::BossItem:
                        rewardText = std::to_string(pi->cost) + " Boss " + (pi->cost == 1 ? "Item" : "Items");
                        break;
                    case CostTypeIndex_Value::ArtifactShellKillerItem:
                        rewardText = std::to_string(pi->cost) + " Artifact Shell Killer " + (pi->cost == 1 ? "Item" : "Items");
                        break;
                    case CostTypeIndex_Value::TreasureCacheItem:
                        rewardText = std::to_string(pi->cost) + " Treasure Cache " + (pi->cost == 1 ? "Item" : "Items");
                        break;
                    case CostTypeIndex_Value::TreasureCacheVoidItem:
                        rewardText = std::to_string(pi->cost) + " Treasure Cache Void " + (pi->cost == 1 ? "Item" : "Items");
                        break;
                    case CostTypeIndex_Value::VoidCoin:
                        rewardText = std::to_string(pi->cost) + " Void " + (pi->cost == 1 ? "Coin" : "Coins");
                        break;
                    case CostTypeIndex_Value::SoulCost:
                        rewardText = std::to_string(pi->cost) + " Soul Cost";
                        break;
                    default:
                        rewardText = "Cost: " + std::to_string(pi->cost);
                        break;
                }
            }
        }

        if (!rewardText.empty()) {
            ImVec2 textPos = RenderUtils::RenderText(ImVec2(screenPos.x, screenPos.y - yOffset),
                                                     control->GetCostColorU32(),
                                                     control->GetCostShadowColorU32(),
                                                     control->IsCostShadowEnabled(),
                                                     true,  // Center text
                                                     rewardText.c_str());
            yOffset += fontSize + 2;
        }
    }

}