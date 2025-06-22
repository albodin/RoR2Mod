#pragma once
#include "ModuleBase.h"
#include "menu/InputControls.h"
#include "game/GameStructs.h"
#include "utils/ModStructs.h"
#include <mutex>
#include <queue>
#include <shared_mutex>

class EnemySpawningModule : public ModuleBase {
private:
    ComboControl* enemySelectControl;
    ComboControl* teamSelectControl;
    IntControl* spawnCountControl;
    ToggleControl* difficultyMatchingControl;
    ComboControl* eliteSelectControl;
    ButtonControl* spawnButtonControl;

    std::mutex queuedSpawnsMutex;
    std::queue<std::tuple<int, int, int, bool, int>> queuedSpawns; // masterIndex, count, teamIndex, matchDifficulty, eliteIndex

    std::vector<RoR2Enemy> enemies;
    std::vector<std::string> enemyNames;
    std::vector<int> enemyMasterIndices;

public:
    EnemySpawningModule();
    ~EnemySpawningModule();

    void Initialize() override {}
    void Update() override;
    void DrawUI() override;

    void OnLocalUserUpdate(void* localUser);
    void InitializeEnemies();
    void SpawnEnemy(int masterIndex, int count = 1, int eliteIndex = 0);
    void PrepareEnemyLists();
};