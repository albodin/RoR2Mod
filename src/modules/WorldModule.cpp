#include "WorldModule.h"
#include "globals/globals.h"
#include "hooks/hooks.h"
#include "imgui.h"

WorldModule::WorldModule() : ModuleBase(),
    instantTeleporterControl(nullptr),
    instantHoldoutZoneControl(nullptr),
    openExpiredTimedChestsControl(nullptr) {
    Initialize();
}

WorldModule::~WorldModule() {
    delete instantTeleporterControl;
    delete instantHoldoutZoneControl;
    delete openExpiredTimedChestsControl;
}

void WorldModule::Initialize() {
    instantTeleporterControl = new ToggleControl("Instant Teleporter", "instantTeleporter", false);
    instantHoldoutZoneControl = new ToggleControl("Instant Holdout Zone", "instantHoldoutZone", false);
    openExpiredTimedChestsControl = new ToggleControl("Open Expired Timed Chests", "openExpiredTimedChests", false);
}

void WorldModule::Update() {
    instantTeleporterControl->Update();
    instantHoldoutZoneControl->Update();
    openExpiredTimedChestsControl->Update();
}

void WorldModule::DrawUI() {
    instantTeleporterControl->Draw();
    instantHoldoutZoneControl->Draw();
    openExpiredTimedChestsControl->Draw();
}

void WorldModule::OnTeleporterInteractionFixedUpdate(void* teleporter) {
    TeleporterInteraction* teleporter_ptr = (TeleporterInteraction*)teleporter;
    if (instantTeleporterControl->IsEnabled() && teleporter_ptr && teleporter_ptr->holdoutZoneController_backing && teleporter_ptr->holdoutZoneController_backing->_charge < 1.0f) {
        teleporter_ptr->holdoutZoneController_backing->_charge = 1.0f;
    }
}

void WorldModule::OnHoldoutZoneControllerUpdate(void* holdoutZoneController) {
    if (!instantHoldoutZoneControl->IsEnabled()) return;

    HoldoutZoneController* controller = (HoldoutZoneController*)holdoutZoneController;
    if (!controller) return;

    if (controller->_charge < 1.0f && controller->_charge > 0.0f) {
        controller->_charge = 1.0f;
    }
}