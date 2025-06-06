#pragma once
#include "ModuleBase.h"
#include "menu/InputControls.h"
#include "game/GameStructs.h"

class WorldModule : public ModuleBase {
private:
    ToggleControl* instantTeleporterControl;
    ToggleControl* instantHoldoutZoneControl;

public:
    WorldModule();
    ~WorldModule() override;

    void Initialize() override;
    void Update() override;
    void DrawUI() override;

    void OnTeleporterInteractionFixedUpdate(void* teleporter);
    void OnHoldoutZoneControllerUpdate(void* holdoutZoneController);
};