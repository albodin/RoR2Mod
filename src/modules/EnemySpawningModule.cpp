#include "EnemySpawningModule.h"
#include "globals/globals.h"
#include "imgui.h"
#include "menu/InputControls.h"
#include "config/ConfigManager.h"

EnemySpawningModule::EnemySpawningModule() : ModuleBase() {
    // Team names for dropdown
    std::vector<std::string> teamNames = {"Neutral", "Player", "Monster", "Lunar", "Void"};

    // Create UI controls
    enemySelectControl = new ComboControl("Enemy", "enemySpawn_selectedEnemy", {"Loading..."}, 0);
    teamSelectControl = new ComboControl("Team", "enemySpawn_team", teamNames, 2); // Default to Monster
    spawnCountControl = new IntControl("Spawn Count", "enemySpawn_count", 1, 1, 100, 1, false, false);
    spawnCountControl->SetShowCheckbox(false);
    difficultyMatchingControl = new ToggleControl("Match Difficulty", "enemySpawn_matchDifficulty", false, ImGuiKey_None);
    eliteSelectControl = new ComboControl("Elite Type", "enemySpawn_eliteType", {"Loading..."}, 0);
    spawnButtonControl = new ButtonControl("Spawn", "enemySpawn_button", "Spawn at Crosshair");

    // Set up callbacks

    spawnButtonControl->SetOnClick([this]() {
        int enemyIdx = enemySelectControl->GetSelectedIndex();
        int eliteDropdownIdx = eliteSelectControl->GetSelectedIndex();
        if (enemyIdx >= 0 && enemyIdx < enemyMasterIndices.size()) {
            // Convert dropdown index to actual buff index
            int eliteBuffIndex = 0; // Default to None
            if (eliteDropdownIdx > 0 && eliteDropdownIdx < G::eliteNames.size()) {
                std::string eliteName = G::eliteNames[eliteDropdownIdx];
                auto it = G::eliteBuffIndices.find(eliteName);
                if (it != G::eliteBuffIndices.end()) {
                    eliteBuffIndex = it->second;
                }
            }
            SpawnEnemy(enemyMasterIndices[enemyIdx], spawnCountControl->GetValue(), eliteBuffIndex);
        }
    });
}

EnemySpawningModule::~EnemySpawningModule() {
    delete enemySelectControl;
    delete teamSelectControl;
    delete spawnCountControl;
    delete difficultyMatchingControl;
    delete eliteSelectControl;
    delete spawnButtonControl;
}


void EnemySpawningModule::Update() {
    // Controls handle their own hotkey updates
    enemySelectControl->Update();
    teamSelectControl->Update();
    spawnCountControl->Update();
    difficultyMatchingControl->Update();
    eliteSelectControl->Update();
    spawnButtonControl->Update();
}

void EnemySpawningModule::DrawUI() {
    if (ImGui::CollapsingHeader("Enemy Spawning")) {
        // Draw controls
        enemySelectControl->Draw();
        teamSelectControl->Draw();
        spawnCountControl->Draw();
        difficultyMatchingControl->Draw();
        eliteSelectControl->Draw();
        spawnButtonControl->Draw();

        // Show selected enemy info
        int selectedIdx = enemySelectControl->GetSelectedIndex();
        if (selectedIdx >= 0 && selectedIdx < enemies.size()) {
            const auto& enemy = enemies[selectedIdx];
            ImGui::Separator();
            ImGui::Text("=== Enemy Details ===");
            ImGui::Text("Name: %s", enemy.displayName.c_str());
            ImGui::Text("Master Index: %d", enemy.masterIndex);
            ImGui::Text("Internal Name: %s", enemy.masterName.c_str());
        }
    }
}

void EnemySpawningModule::OnLocalUserUpdate(void* localUser) {
    LocalUser* localUser_ptr = (LocalUser*)localUser;
    if (!localUser_ptr->_cameraRigController) {
        return;
    }

    // Process queued spawns
    std::unique_lock<std::mutex> lock(queuedSpawnsMutex);
    for (; !queuedSpawns.empty(); queuedSpawns.pop()) {
        auto spawn = queuedSpawns.front();
        int masterIndex = std::get<0>(spawn);
        int count = std::get<1>(spawn);
        int teamIndex = std::get<2>(spawn);
        bool matchDifficulty = std::get<3>(spawn);
        int eliteBuffIndex = std::get<4>(spawn);
        Vector3 spawnPosition = localUser_ptr->_cameraRigController->crosshairWorldPosition_backing;

        // Find elite name from buff index
        std::string eliteType = "None";
        for (const auto& [name, index] : G::eliteBuffIndices) {
            if (index == eliteBuffIndex) {
                eliteType = name;
                break;
            }
        }
        G::logger.LogInfo("Spawning %d %s enemies with difficulty matching %s (masterIndex: %d, team: %d)", count, eliteType.c_str(), matchDifficulty ? "enabled" : "disabled", masterIndex, teamIndex);

        for (int i = 0; i < count; i++) {
            bool success = G::gameFunctions->SpawnEnemyAtPosition(masterIndex, spawnPosition, teamIndex, matchDifficulty, eliteBuffIndex);
            if (!success) {
                G::logger.LogError("Failed to spawn enemy %d of %d (masterIndex: %d)", i + 1, count, masterIndex);
            }
        }
    }
}

void EnemySpawningModule::InitializeEnemies() {
    std::shared_lock<std::shared_mutex> lock(G::enemiesMutex);

    enemies = G::enemies;
    PrepareEnemyLists();

    // Initialize elite types if available
    if (!G::eliteNames.empty()) {
        eliteSelectControl->SetItems(G::eliteNames);
    }

    G::logger.LogInfo("Enemy spawning module initialized with %zu enemies", enemies.size());
}

void EnemySpawningModule::PrepareEnemyLists() {
    enemyNames.clear();
    enemyMasterIndices.clear();

    for (const auto& enemy : enemies) {
        enemyNames.push_back(enemy.displayName);
        enemyMasterIndices.push_back(enemy.masterIndex);
    }

    // Update enemy selection dropdown with all enemies
    enemySelectControl->SetItems(enemyNames);

    // Reset selection to first item
    if (!enemies.empty()) {
        enemySelectControl->SetSelectedIndex(0);
    }
}



void EnemySpawningModule::SpawnEnemy(int masterIndex, int count, int eliteIndex) {
    std::unique_lock<std::mutex> lock(queuedSpawnsMutex);
    int teamIndex = teamSelectControl->GetSelectedIndex();
    queuedSpawns.push(std::make_tuple(masterIndex, count, teamIndex, difficultyMatchingControl->IsEnabled(), eliteIndex));
}