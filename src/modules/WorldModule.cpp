#include "WorldModule.h"
#include "globals/globals.h"
#include "hooks/hooks.h"
#include "imgui.h"

WorldModule::WorldModule() : ModuleBase(),
    instantTeleporterControl(nullptr) {
    Initialize();
}

WorldModule::~WorldModule() {
    delete instantTeleporterControl;
}

void WorldModule::Initialize() {
    instantTeleporterControl = new ToggleControl("Instant Teleporter", "instantTeleporter", false);
}

void WorldModule::Update() {
    instantTeleporterControl->Update();
}

void WorldModule::DrawUI() {
    instantTeleporterControl->Draw();
}

void WorldModule::OnTeleporterInteractionFixedUpdate(void* teleporter) {
    TeleporterInteraction* teleporter_ptr = (TeleporterInteraction*)teleporter;
    if (instantTeleporterControl->IsEnabled() && teleporter_ptr && teleporter_ptr->holdoutZoneController_backing && teleporter_ptr->holdoutZoneController_backing->_charge < 1.0f) {
        teleporter_ptr->holdoutZoneController_backing->_charge = 1.0f;
    }
}