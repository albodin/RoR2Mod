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
}

void ESPModule::Initialize() {
    teleporterESPControl = new ESPControl("Teleporter ESP", "teleporter_esp", false,
                                    250.0f, 1000.0f, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow

    playerESPControl = new EntityESPControl("Players", "player_esp");
    enemyESPControl = new EntityESPControl("Enemies", "enemy_esp");
}

void ESPModule::Update() {
    teleporterESPControl->Update();
    playerESPControl->Update();
    enemyESPControl->Update();
}

void ESPModule::DrawUI() {
    teleporterESPControl->Draw();
    playerESPControl->Draw();
    enemyESPControl->Draw();
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
        if (!RenderUtils::WorldToScreen(mainCamera, playerWorldPos, screenPos)) {
            continue;
        }

        RenderEntityESP(entity, screenPos, distance, control, isVisible);
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
        if (!RenderUtils::WorldToScreen(mainCamera, enemyWorldPos, screenPos)) {
            continue;
        }

        RenderEntityESP(entity, screenPos, distance, control, isVisible);
    }
}

void ESPModule::RenderEntityESP(TrackedEntity* entity, ImVec2 screenPos, float distance, EntityESPSubControl* control, bool isVisible) {
    if (!entity || !control) return;

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

    // Fallback bounds if we couldn't calculate from hurtboxes
    if (!boundsFound) {
        screenMin = ImVec2(screenPos.x - 30.0f, screenPos.y - 40.0f);
        screenMax = ImVec2(screenPos.x + 30.0f, screenPos.y + 40.0f);
    }

    ImVec2 boxSize(screenMax.x - screenMin.x, screenMax.y - screenMin.y);
    ImVec2 boxBottomCenter(screenMin.x + boxSize.x / 2, screenMax.y);
    ImVec2 textPos = boxBottomCenter;
    textPos.y += 5; // Small gap below the box

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