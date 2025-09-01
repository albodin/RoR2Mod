#include "EnemyModule.hpp"
#include "game/GameStructs.hpp"
#include "globals/globals.hpp"
#include <climits>

EnemyModule::EnemyModule() {
    monsterLevelControl = std::make_unique<IntControl>("Monster Level", "enemy_monster_level", 1, 1, INT_MAX, 1, false, false, true);
    monsterLevelControl->SetGameValueFunctions([]() { return G::gameFunctions->GetTeamLevel(TeamIndex_Value::Monster); },
                                               [](int value) { G::gameFunctions->SetTeamLevel(TeamIndex_Value::Monster, static_cast<uint32_t>(value)); });

    lunarLevelControl = std::make_unique<IntControl>("Lunar Level", "enemy_lunar_level", 1, 1, INT_MAX, 1, false, false, true);
    lunarLevelControl->SetGameValueFunctions([]() { return G::gameFunctions->GetTeamLevel(TeamIndex_Value::Lunar); },
                                             [](int value) { G::gameFunctions->SetTeamLevel(TeamIndex_Value::Lunar, static_cast<uint32_t>(value)); });

    voidLevelControl = std::make_unique<IntControl>("Void Level", "enemy_void_level", 1, 1, INT_MAX, 1, false, false, true);
    voidLevelControl->SetGameValueFunctions([]() { return G::gameFunctions->GetTeamLevel(TeamIndex_Value::Void); },
                                            [](int value) { G::gameFunctions->SetTeamLevel(TeamIndex_Value::Void, static_cast<uint32_t>(value)); });
}

EnemyModule::~EnemyModule() {}

void EnemyModule::Initialize() {}

void EnemyModule::Update() {
    monsterLevelControl->Update();
    lunarLevelControl->Update();
    voidLevelControl->Update();
}

void EnemyModule::DrawUI() {
    ImGui::Separator();
    ImGui::Text("Enemy Level Control");

    monsterLevelControl->Draw();
    lunarLevelControl->Draw();
    voidLevelControl->Draw();
}

void EnemyModule::OnLocalUserUpdate(void* localUser) {
    if (G::gameFunctions->GetTeamManagerInstance()) {
        monsterLevelControl->UpdateFreezeLogic();
        lunarLevelControl->UpdateFreezeLogic();
        voidLevelControl->UpdateFreezeLogic();
    }
}
