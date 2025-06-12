#include "WorldModule.h"
#include "globals/globals.h"
#include "hooks/hooks.h"
#include "imgui.h"

WorldModule::WorldModule() : ModuleBase(),
    instantTeleporterControl(nullptr),
    instantHoldoutZoneControl(nullptr),
    openExpiredTimedChestsControl(nullptr),
    forceBluePortalControl(nullptr),
    forceGoldenPortalControl(nullptr),
    forceCelestialPortalControl(nullptr),
    forceAllPortalsControl(nullptr) {
    Initialize();
}

WorldModule::~WorldModule() {
    delete instantTeleporterControl;
    delete instantHoldoutZoneControl;
    delete openExpiredTimedChestsControl;
    delete forceBluePortalControl;
    delete forceGoldenPortalControl;
    delete forceCelestialPortalControl;
    delete forceAllPortalsControl;
}

void WorldModule::Initialize() {
    instantTeleporterControl = new ToggleControl("Instant Teleporter", "instantTeleporter", false);
    instantHoldoutZoneControl = new ToggleControl("Instant Holdout Zone", "instantHoldoutZone", false);
    openExpiredTimedChestsControl = new ToggleControl("Open Expired Timed Chests", "openExpiredTimedChests", false);
    forceBluePortalControl = new ToggleControl("Force Blue Portal (Bazaar)", "forceBluePortal", false);
    forceGoldenPortalControl = new ToggleControl("Force Golden Portal (Gilded Coast)", "forceGoldenPortal", false);
    forceCelestialPortalControl = new ToggleControl("Force Celestial Portal (Moon)", "forceCelestialPortal", false);
    forceAllPortalsControl = new ToggleControl("Force All Additional Portals", "forceAllPortals", false);
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