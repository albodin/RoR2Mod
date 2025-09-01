#pragma once
#include "ModuleBase.hpp"
#include "game/GameStructs.hpp"
#include "menu/InputControls.hpp"
#include <memory>

class WorldModule : public ModuleBase {
  private:
    std::unique_ptr<ToggleControl> instantTeleporterControl;
    std::unique_ptr<ToggleControl> instantHoldoutZoneControl;
    std::unique_ptr<ToggleControl> openExpiredTimedChestsControl;
    std::unique_ptr<ToggleControl> forceBluePortalControl;
    std::unique_ptr<ToggleControl> forceGoldenPortalControl;
    std::unique_ptr<ToggleControl> forceCelestialPortalControl;
    std::unique_ptr<ToggleControl> forceAllPortalsControl;

    std::unique_ptr<IntControl> stageClearCountControl;
    std::unique_ptr<FloatControl> fixedTimeControl;

  public:
    WorldModule();
    ~WorldModule() override;

    void Initialize() override;
    void Update() override;
    void DrawUI() override;

    ToggleControl* GetOpenExpiredTimedChestsControl() const { return openExpiredTimedChestsControl.get(); }
    ToggleControl* GetForceAllPortalsControl() const { return forceAllPortalsControl.get(); }

    void OnLocalUserUpdate(void* localUser);
    void OnTeleporterInteractionAwake(void* teleporter);
    void OnTeleporterInteractionFixedUpdate(void* teleporter);
    void OnHoldoutZoneControllerUpdate(void* holdoutZoneController);
};
