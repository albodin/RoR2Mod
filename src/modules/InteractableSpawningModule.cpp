#include "InteractableSpawningModule.h"
#include "game/InteractablePaths.h"
#include "globals/globals.h"
#include "imgui/imgui.h"
#include <algorithm>

InteractableSpawningModule::InteractableSpawningModule() {
    interactableSelectControl = std::make_unique<ComboControl>("Interactable", "interactableSpawn_selected", std::vector<std::string>{"Loading..."}, 0, true);

    spawnButtonControl = std::make_unique<ButtonControl>("Spawn", "interactableSpawn_button", "Spawn on Player");
    spawnAtCrosshairButtonControl = std::make_unique<ButtonControl>("Spawn at Crosshair", "interactableSpawn_crosshair", "Spawn at Crosshair");

    spawnButtonControl->SetOnClick([this]() { SpawnSelectedInteractable(true); });
    spawnAtCrosshairButtonControl->SetOnClick([this]() { SpawnSelectedInteractable(false); });
}

InteractableSpawningModule::~InteractableSpawningModule() {}

void InteractableSpawningModule::Initialize() {
    LOG_INFO("Initializing InteractableSpawningModule...");
    SetupInteractables();
    LOG_INFO("InteractableSpawningModule initialized with %zu interactables", interactableNames.size());
}

void InteractableSpawningModule::Update() {
    interactableSelectControl->Update();
    spawnButtonControl->Update();
    spawnAtCrosshairButtonControl->Update();
}

void InteractableSpawningModule::DrawUI() {
    if (interactableNames.empty()) {
        ImGui::Text("No interactables loaded");
        return;
    }

    interactableSelectControl->Draw();

    if (!G::csHelper || !G::csHelper->IsLoaded()) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "C# Helper not loaded");
    } else {
        spawnButtonControl->Draw();
        spawnAtCrosshairButtonControl->Draw();
    }
}

void InteractableSpawningModule::SetupInteractables() {
    interactableNames.clear();
    interactablePaths.clear();

    for (const auto& [name, path] : InteractablePaths::INTERACTABLES) {
        interactableNames.push_back(name);
        interactablePaths.push_back(path);
    }

    std::vector<std::pair<std::string, std::string>> sortedPairs;
    for (size_t i = 0; i < interactableNames.size(); ++i) {
        sortedPairs.push_back({interactableNames[i], interactablePaths[i]});
    }
    std::sort(sortedPairs.begin(), sortedPairs.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    interactableNames.clear();
    interactablePaths.clear();
    for (const auto& [name, path] : sortedPairs) {
        interactableNames.push_back(name);
        interactablePaths.push_back(path);
    }

    interactableSelectControl->SetItems(interactableNames);

    LOG_INFO("Loaded %zu interactables with addressable paths", interactableNames.size());
}

void InteractableSpawningModule::SpawnSelectedInteractable(bool playerPosition) {
    if (!G::csHelper || !G::csHelper->IsLoaded()) {
        LOG_ERROR("C# Helper not available for spawning");
        return;
    }

    int selectedIndex = interactableSelectControl->GetSelectedIndex();
    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(interactablePaths.size())) {
        LOG_ERROR("Invalid interactable selection index: %d", selectedIndex);
        return;
    }

    std::string selectedPath = interactablePaths[selectedIndex];
    std::string selectedName = interactableNames[selectedIndex];

    Vector3 spawnPos;
    const char* locationDesc;

    if (playerPosition) {
        spawnPos = G::localPlayer->GetPlayerPosition();
        locationDesc = "near player";
        if (spawnPos.x == 0.0f && spawnPos.y == 0.0f && spawnPos.z == 0.0f) {
            LOG_ERROR("Could not get player position");
            return;
        }
    } else {
        spawnPos = G::localPlayer->GetCrosshairPosition();
        locationDesc = "at crosshair";
        if (spawnPos.x == 0.0f && spawnPos.y == 0.0f && spawnPos.z == 0.0f) {
            LOG_ERROR("Could not get crosshair position");
            return;
        }
    }

    LOG_INFO("Spawning %s %s (%.2f, %.2f, %.2f)", selectedName.c_str(), locationDesc, spawnPos.x, spawnPos.y, spawnPos.z);
    G::csHelper->SpawnInteractable(selectedPath, spawnPos.x, spawnPos.y, spawnPos.z);
}
