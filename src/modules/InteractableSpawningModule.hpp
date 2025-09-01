#pragma once
#include "ModuleBase.hpp"
#include "game/GameStructs.hpp"
#include "menu/InputControls.hpp"
#include <memory>
#include <string>
#include <vector>

class InteractableSpawningModule : public ModuleBase {
  private:
    std::unique_ptr<ComboControl> interactableSelectControl;
    std::unique_ptr<ButtonControl> spawnButtonControl;
    std::unique_ptr<ButtonControl> spawnAtCrosshairButtonControl;

    std::vector<std::string> interactableNames;
    std::vector<std::string> interactablePaths;

  public:
    InteractableSpawningModule();
    ~InteractableSpawningModule();

    void Initialize() override;
    void Update() override;
    void DrawUI() override;

  private:
    void SetupInteractables();
    void SpawnSelectedInteractable(bool playerPosition);
};
