#include "ESPModule.h"
#include "globals/globals.h"
#include "hooks/hooks.h"
#include "utils/RenderUtils.h"
#include <imgui.h>
#include <cmath>

ESPModule::ESPModule() : ModuleBase() {
    Initialize();
}

ESPModule::~ESPModule() {
    delete teleporterESPControl;
    delete playerVisibleESPControl;
    delete enemyVisibleESPControl;
}

void ESPModule::Initialize() {
    teleporterESPControl = new ESPControl("Teleporter ESP", "teleporter_esp", false,
                                    250.0f, 1000.0f, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow

    playerVisibleESPControl = new ESPControl("Player ESP", "player_esp", false,
                                    250.0f, 1000.0f, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green

    enemyVisibleESPControl = new ESPControl("Enemy ESP", "enemy_esp", false,
                                   250.0f, 1000.0f, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
}

void ESPModule::Update() {
    teleporterESPControl->Update();
    playerVisibleESPControl->Update();
    enemyVisibleESPControl->Update();
}

void ESPModule::DrawUI() {
    teleporterESPControl->Draw();
    playerVisibleESPControl->Draw();
    enemyVisibleESPControl->Draw();
}

void ESPModule::OnFrameRender() {
    if (teleporterESPControl->IsEnabled()) {
        RenderTeleporterESP();
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

void ESPModule::RenderPlayerESP() {
}

void ESPModule::RenderEnemyESP() {
}