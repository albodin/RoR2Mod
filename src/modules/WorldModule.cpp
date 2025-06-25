#include "WorldModule.h"
#include "globals/globals.h"
#include "hooks/hooks.h"
#include "imgui.h"

WorldModule::WorldModule() : ModuleBase() {
    Initialize();
}

WorldModule::~WorldModule() {
}

void WorldModule::Initialize() {
    instantTeleporterControl = std::make_unique<ToggleControl>("Instant Teleporter", "instantTeleporter", false);
    instantHoldoutZoneControl = std::make_unique<ToggleControl>("Instant Holdout Zone", "instantHoldoutZone", false);
    openExpiredTimedChestsControl = std::make_unique<ToggleControl>("Open Expired Timed Chests", "openExpiredTimedChests", false);
    forceBluePortalControl = std::make_unique<ToggleControl>("Force Blue Portal (Bazaar)", "forceBluePortal", false);
    forceGoldenPortalControl = std::make_unique<ToggleControl>("Force Golden Portal (Gilded Coast)", "forceGoldenPortal", false);
    forceCelestialPortalControl = std::make_unique<ToggleControl>("Force Celestial Portal (Moon)", "forceCelestialPortal", false);
    forceAllPortalsControl = std::make_unique<ToggleControl>("Force All Additional Portals", "forceAllPortals", false);
}

void WorldModule::Update() {
    instantTeleporterControl->Update();
    instantHoldoutZoneControl->Update();
    openExpiredTimedChestsControl->Update();
    forceBluePortalControl->Update();
    forceGoldenPortalControl->Update();
    forceCelestialPortalControl->Update();
    forceAllPortalsControl->Update();
}

void WorldModule::DrawUI() {
    instantTeleporterControl->Draw();
    instantHoldoutZoneControl->Draw();
    openExpiredTimedChestsControl->Draw();

    ImGui::Separator();
    ImGui::Text("Portal Forcing");
    forceBluePortalControl->Draw();
    forceGoldenPortalControl->Draw();
    forceCelestialPortalControl->Draw();
    forceAllPortalsControl->Draw();
}

void WorldModule::OnTeleporterInteractionAwake(void* teleporter) {
    // TODO: Delete or find a use for this function
}

void WorldModule::OnTeleporterInteractionFixedUpdate(void* teleporter) {
    TeleporterInteraction* teleporter_ptr = (TeleporterInteraction*)teleporter;
    if (!teleporter_ptr) return;

    // Instant teleporter logic
    if (instantTeleporterControl->IsEnabled() && teleporter_ptr->holdoutZoneController_backing && teleporter_ptr->holdoutZoneController_backing->_charge < 1.0f) {
        teleporter_ptr->holdoutZoneController_backing->_charge = 1.0f;
    }

    // Portal forcing logic
    if (forceBluePortalControl->IsEnabled()) {
        teleporter_ptr->_shouldAttemptToSpawnShopPortal = true;
    }

    if (forceGoldenPortalControl->IsEnabled()) {
        teleporter_ptr->_shouldAttemptToSpawnGoldshoresPortal = true;
    }

    if (forceCelestialPortalControl->IsEnabled()) {
        teleporter_ptr->_shouldAttemptToSpawnMSPortal = true;
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