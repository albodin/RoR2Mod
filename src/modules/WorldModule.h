#pragma once
#include "ModuleBase.h"
#include "menu/InputControls.h"
#include "game/GameStructs.h"

class WorldModule : public ModuleBase {
private:
    ToggleControl* instantTeleporterControl;

public:
    WorldModule();
    ~WorldModule() override;

    void Initialize() override;
    void Update() override;
    void DrawUI() override;

    void OnTeleporterInteractionFixedUpdate(void* teleporter);
};