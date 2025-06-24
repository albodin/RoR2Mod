#pragma once
#include "ModuleBase.h"
#include "menu/InputControls.h"
#include "game/GameStructs.h"
#include "core/MonoTypes.h"
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
    std::string nameToken; // Store the language-independent token
    std::string costString; // Store the localized cost string
    Vector3 position;
    InteractableCategory category;
    bool consumed; // For item pickups
    int32_t pickupIndex; // Store the pickup index
};

// Helper structures for depth-based rendering
struct ESPEntityRenderItem {
    TrackedEntity* entity;
    float distance;
    bool isVisible;
    bool onScreen;
    ImVec2 screenPos;

    ESPEntityRenderItem(TrackedEntity* e, float d, bool visible, bool onScr, const ImVec2& pos)
        : entity(e), distance(d), isVisible(visible), onScreen(onScr), screenPos(pos) {}
};

struct ESPInteractableRenderItem {
    TrackedInteractable* interactable;
    float distance;
    bool isVisible;
    bool onScreen;
    bool isAvailable;
    ImVec2 screenPos;

    ESPInteractableRenderItem(TrackedInteractable* inter, float d, bool visible, bool onScr, bool available, const ImVec2& pos)
        : interactable(inter), distance(d), isVisible(visible), onScreen(onScr), isAvailable(available), screenPos(pos) {}
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
    std::string teleporterDisplayName;
    Vector3 playerPosition;
    Camera* mainCamera;

    std::vector<TrackedEntity*> trackedEnemies;
    std::vector<TrackedEntity*> trackedPlayers;
    std::vector<TrackedInteractable*> trackedInteractables;
    std::mutex entitiesMutex;
    std::mutex interactablesMutex;

    // Cached cost format strings
    std::string m_moneyFormat;
    std::string m_percentHealthFormat;
    std::string m_itemFormat;
    std::string m_lunarFormat;
    std::string m_equipmentFormat;
    std::string m_volatileBatteryFormat;
    std::string m_artifactKeyFormat;
    std::string m_rustedKeyFormat;
    std::string m_encrustedKeyFormat;
    std::string m_lunarCoinName;
    std::string m_voidCoinName;
    bool m_costFormatsInitialized;

    // Pickup name cache
    std::unordered_map<int32_t, std::string> m_pickupIdToNameCache;
    bool m_pickupCacheInitialized;

    void RenderEntityESP(TrackedEntity* entity, ImVec2 screenPos, float distance, EntityESPSubControl* control, bool isVisible, bool onScreen);
    void UpdateTimedChestDisplayName(TrackedInteractable* interactable, void* timedChestController);
    void UpdatePressurePlateDisplayName(TrackedInteractable* interactable, void* pressurePlateController);
    void RenderInteractableESP(TrackedInteractable* interactable, ImVec2 screenPos, float distance, ChestESPSubControl* control, bool isVisible, bool onScreen, bool isAvailable);
    Vector3 GetCameraPosition();
    bool IsVisible(const Vector3& position);
    InteractableCategory DetermineInteractableCategory(PurchaseInteraction* pi, MonoString* nameToken);
    void InitializeCostFormats();
    std::string GetPickupName(int32_t pickupIndex);
    std::string GetCostString(CostTypeIndex_Value costType, int cost);

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

    void CachePickupName(int32_t pickupIndex, const std::string& name);

    void RenderTeleporterESP();
    void RenderPlayerESP();
    void RenderEnemyESP();
    void RenderInteractablesESP();
};