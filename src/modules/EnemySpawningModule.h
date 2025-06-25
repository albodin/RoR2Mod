#pragma once
#include "ModuleBase.h"
#include "menu/InputControls.h"
#include "game/GameStructs.h"
#include "utils/ModStructs.h"
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <memory>

class EnemySpawningModule : public ModuleBase {
private:
    std::unique_ptr<ComboControl> enemySelectControl;
    std::unique_ptr<ComboControl> teamSelectControl;
    std::unique_ptr<IntControl> spawnCountControl;
    std::unique_ptr<ToggleControl> difficultyMatchingControl;
    std::unique_ptr<ComboControl> eliteSelectControl;
    std::unique_ptr<ButtonControl> spawnButtonControl;

    std::mutex queuedSpawnsMutex;
    std::queue<std::tuple<int, int, int, bool, int, std::vector<std::pair<int, int>>>> queuedSpawns; // masterIndex, count, teamIndex, matchDifficulty, eliteIndex, items

    std::vector<RoR2Enemy> enemies;
    std::vector<std::string> enemyNames;
    std::vector<int> enemyMasterIndices;

    // Item management (similar to PlayerModule)
    std::shared_mutex itemsMutex;
    std::vector<RoR2Item> items;
    std::map<int, std::unique_ptr<IntControl>> itemControls;

public:
    EnemySpawningModule();
    ~EnemySpawningModule();

    void Initialize() override {}
    void Update() override;
    void DrawUI() override;

    void OnLocalUserUpdate(void* localUser);
    void InitializeEnemies();
    void InitializeItems();
    void InitializeAllItemControls();
    void SpawnEnemy(int masterIndex, int count = 1, int eliteIndex = 0);
    void PrepareEnemyLists();
    void DrawItemInputs(ItemTier_Value tier);
    void SortItemsByName();
};