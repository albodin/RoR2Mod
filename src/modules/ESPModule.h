#pragma once
#include "ModuleBase.h"
#include "menu/InputControls.h"
#include "game/GameStructs.h"
#include <vector>
#include <unordered_map>

class ESPModule : public ModuleBase {
private:
    ESPControl* teleporterESPControl;
    ESPControl* playerVisibleESPControl;
    ESPControl* enemyVisibleESPControl;

    Vector3 teleporterPosition;
    Vector3 playerPosition;
    Camera* mainCamera;

    void UpdateVisibleEntities();
    bool IsEntityInRange(const Vector3& position, float maxDistance);
    bool WorldToScreen(const Vector3& worldPos, ImVec2& screenPos);
    void RenderBoxESP(const Vector3& position, float height, float width, const char* label, ImU32 color, bool showLabel);

public:
    ESPModule();
    ~ESPModule() override;

    void Initialize() override;
    void Update() override;
    void DrawUI() override;
    void OnFrameRender();

    void OnGameUpdate();
    void OnTeleporterAwake(void* teleporter);

    void RenderTeleporterESP();
    void RenderPlayerESP();
    void RenderEnemyESP();
};