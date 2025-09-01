#pragma once

#include "ModuleBase.hpp"
#include "menu/InputControls.hpp"
#include <memory>

class EnemyModule : public ModuleBase {
  private:
    std::unique_ptr<IntControl> monsterLevelControl;
    std::unique_ptr<IntControl> lunarLevelControl;
    std::unique_ptr<IntControl> voidLevelControl;

  public:
    EnemyModule();
    ~EnemyModule() override;

    void Initialize() override;
    void Update() override;
    void DrawUI() override;

    void OnLocalUserUpdate(void* localUser);

    IntControl* GetMonsterLevelControl() { return monsterLevelControl.get(); }
    IntControl* GetLunarLevelControl() { return lunarLevelControl.get(); }
    IntControl* GetVoidLevelControl() { return voidLevelControl.get(); }
};
