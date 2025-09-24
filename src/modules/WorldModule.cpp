#include "WorldModule.hpp"
#include "globals/globals.hpp"
#include "hooks/hooks.hpp"
#include "imgui.h"

WorldModule::WorldModule() : ModuleBase() { Initialize(); }

WorldModule::~WorldModule() {}

void WorldModule::Initialize() {
    instantTeleporterControl = std::make_unique<ToggleControl>("Instant Teleporter", "instantTeleporter", false);
    instantHoldoutZoneControl = std::make_unique<ToggleControl>("Instant Holdout Zone", "instantHoldoutZone", false);
    openExpiredTimedChestsControl = std::make_unique<ToggleControl>("Open Expired Timed Chests", "openExpiredTimedChests", false);
    openLockedInteractablesControl = std::make_unique<ToggleControl>("Open Locked Interactables", "openLockedInteractables", false);
    forceBluePortalControl = std::make_unique<ToggleControl>("Force Blue Portal (Bazaar)", "forceBluePortal", false);
    forceGoldenPortalControl = std::make_unique<ToggleControl>("Force Golden Portal (Gilded Coast)", "forceGoldenPortal", false);
    forceCelestialPortalControl = std::make_unique<ToggleControl>("Force Celestial Portal (Moon)", "forceCelestialPortal", false);
    forceAllPortalsControl = std::make_unique<ToggleControl>("Force All Additional Portals", "forceAllPortals", false);

    stageClearCountControl = std::make_unique<IntControl>("Stages Cleared", "stage_clear_count", 0, 0, INT_MAX, 1, false, false, true);
    stageClearCountControl->SetGameValueFunctions([]() { return G::gameFunctions->GetStageClearCount(); },
                                                  [](int value) { G::gameFunctions->SetStageClearCount(value); });

    fixedTimeControl = std::make_unique<FloatControl>("Fixed Time", "fixed_time", 1.0f, 0.0f, FLT_MAX, 1.0f, false, false, true);
    fixedTimeControl->SetGameValueFunctions([]() { return G::gameFunctions->GetFixedTime(); }, [](float value) { G::gameFunctions->SetFixedTime(value); });

    stageClearCountControl->SetOnChange([](int newValue) { G::gameFunctions->SetStageClearCount(newValue); });

    fixedTimeControl->SetOnChange([](float newValue) { G::gameFunctions->SetFixedTime(newValue); });
}

void WorldModule::Update() {
    instantTeleporterControl->Update();
    instantHoldoutZoneControl->Update();
    openExpiredTimedChestsControl->Update();
    openLockedInteractablesControl->Update();
    forceBluePortalControl->Update();
    forceGoldenPortalControl->Update();
    forceCelestialPortalControl->Update();
    forceAllPortalsControl->Update();

    stageClearCountControl->Update();
    fixedTimeControl->Update();
}

void WorldModule::DrawUI() {
    instantTeleporterControl->Draw();
    instantHoldoutZoneControl->Draw();
    openExpiredTimedChestsControl->Draw();
    openLockedInteractablesControl->Draw();

    ImGui::Separator();
    ImGui::Text("Portal Forcing");
    forceBluePortalControl->Draw();
    forceGoldenPortalControl->Draw();
    forceCelestialPortalControl->Draw();
    forceAllPortalsControl->Draw();

    ImGui::Separator();
    ImGui::Text("Stage Control");
    stageClearCountControl->Draw();
    fixedTimeControl->Draw();
}

void WorldModule::OnLocalUserUpdate(void* localUser) {
    if (G::runInstance) {
        stageClearCountControl->UpdateFreezeLogic();
        fixedTimeControl->UpdateFreezeLogic();
    }
}

void WorldModule::OnTeleporterInteractionAwake(void* teleporter) {
    // TODO: Delete or find a use for this function
}

void WorldModule::OnTeleporterInteractionFixedUpdate(void* teleporter) {
    TeleporterInteraction* teleporter_ptr = static_cast<TeleporterInteraction*>(teleporter);
    if (!teleporter_ptr)
        return;

    // Instant teleporter logic
    if (instantTeleporterControl->IsEnabled() && teleporter_ptr->holdoutZoneController_backing &&
        teleporter_ptr->holdoutZoneController_backing->_charge < 1.0f) {
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
    if (!instantHoldoutZoneControl->IsEnabled())
        return;

    HoldoutZoneController* controller = static_cast<HoldoutZoneController*>(holdoutZoneController);
    if (!controller)
        return;

    if (controller->_charge < 1.0f && controller->_charge > 0.0f) {
        controller->_charge = 1.0f;
    }
}
