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

enum class InteractableCategory {
    Chest,
    Shop,
    Drone,
    Shrine,
    Special,
    Barrel,
    ItemPickup,
    Portal,
    Unknown
};

struct TrackedInteractable {
    void* gameObject;
    void* purchaseInteraction;
    std::string displayName;
    std::string itemName; // Name of the item in chest/shop
    Vector3 position;
    InteractableCategory category;
    bool consumed; // For item pickups
    int32_t pickupIndex; // Store the pickup index
};

class ChestESPControl;

class ESPModule : public ModuleBase {
private:
    ESPControl* teleporterESPControl;
    EntityESPControl* playerESPControl;
    EntityESPControl* enemyESPControl;
    ChestESPControl* chestESPControl;
    ChestESPControl* shopESPControl;
    ChestESPControl* droneESPControl;
    ChestESPControl* shrineESPControl;
    ChestESPControl* specialESPControl;
    ChestESPControl* barrelESPControl;
    ChestESPControl* itemPickupESPControl;
    ChestESPControl* portalESPControl;

    Vector3 teleporterPosition;
    Vector3 playerPosition;
    Camera* mainCamera;

    std::vector<TrackedEntity*> trackedEnemies;
    std::vector<TrackedEntity*> trackedPlayers;
    std::vector<TrackedInteractable*> trackedInteractables;
    std::mutex entitiesMutex;
    std::mutex interactablesMutex;

    void RenderEntityESP(TrackedEntity* entity, ImVec2 screenPos, float distance, EntityESPSubControl* control, bool isVisible, bool onScreen);
    void UpdateTimedChestDisplayName(TrackedInteractable* interactable, void* timedChestController);
    void UpdatePressurePlateDisplayName(TrackedInteractable* interactable, void* pressurePlateController);
    void RenderInteractableESP(TrackedInteractable* interactable, ImVec2 screenPos, float distance, ChestESPSubControl* control, bool isVisible, bool onScreen, bool isAvailable);
    Vector3 GetCameraPosition();
    bool IsVisible(const Vector3& position);
    InteractableCategory DetermineInteractableCategory(PurchaseInteraction* pi, const std::string& displayName);

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
    void OnPurchaseInteractionSpawned(void* purchaseInteraction);
    void OnPurchaseInteractionDestroyed(void* purchaseInteraction);
    void OnBarrelInteractionSpawned(void* barrelInteraction);
    void OnGenericPickupControllerSpawned(void* genericPickupController);
    void OnTimedChestControllerSpawned(void* timedChestController);
    void OnTimedChestControllerDespawned(void* timedChestController);
    void OnGenericInteractionSpawned(void* genericInteraction);
    void OnPickupPickerControllerSpawned(void* pickupPickerController);
    void OnScrapperControllerSpawned(void* scrapperController);
    void OnStageStart(void* stage);
    void OnChestBehaviorSpawned(void* chestBehavior);
    void OnShopTerminalBehaviorSpawned(void* shopTerminalBehavior);
    void OnPressurePlateControllerSpawned(void* pressurePlateController);

    void RenderTeleporterESP();
    void RenderPlayerESP();
    void RenderEnemyESP();
    void RenderInteractablesESP();
};