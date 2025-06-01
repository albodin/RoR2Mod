#pragma once
#include "ModuleBase.h"
#include "menu/InputControls.h"
#include "game/GameStructs.h"
#include <vector>
#include <unordered_map>
#include <mutex>

struct TrackedEntity {
    CharacterBody* body;
    std::string displayName;
};

class ESPModule : public ModuleBase {
private:
    ESPControl* teleporterESPControl;
    EntityESPControl* playerESPControl;
    EntityESPControl* enemyESPControl;

    Vector3 teleporterPosition;
    Vector3 playerPosition;
    Camera* mainCamera;

    std::vector<TrackedEntity*> trackedEnemies;
    std::vector<TrackedEntity*> trackedPlayers;
    std::mutex entitiesMutex;

    void RenderEntityESP(TrackedEntity* entity, ImVec2 screenPos, float distance, EntityESPSubControl* control, bool isVisible, bool onScreen);
    Vector3 GetCameraPosition();
    bool IsVisible(const Vector3& position);

public:
    ESPModule();
    ~ESPModule() override;

    void Initialize() override;
    void Update() override;
    void DrawUI() override;
    void OnFrameRender();

    void OnGameUpdate();
    void OnTeleporterAwake(void* teleporter);
    void OnCharacterBodySpawned(void* characterBody);
    void OnCharacterBodyDestroyed(void* characterBody);

    void RenderTeleporterESP();
    void RenderPlayerESP();
    void RenderEnemyESP();
};