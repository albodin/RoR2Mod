#pragma once
#include "ModuleBase.h"
#include "core/MonoTypes.h"
#include "game/GameStructs.h"
#include "menu/InputControls.h"
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

struct TrackedEntity {
    CharacterBody* body;
    std::string displayName;
};

enum class InteractableCategory { Chest, Shop, Drone, Shrine, Special, Barrel, ItemPickup, Portal, CommandCube, Unknown };

struct TrackedInteractable {
    void* gameObject;
    void* purchaseInteraction;
    std::string displayName;
    std::string itemName;   // Name of the item in chest/shop
    std::string nameToken;  // Store the language-independent token
    std::string costString; // Store the localized cost string
    Vector3 position;
    InteractableCategory category;
    bool consumed;       // For item pickups
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

// Hierarchical ESP ordering system
enum class ESPMainCategory { Players = 0, Enemies, Teleporter, Chests, Shops, Drones, Shrines, Specials, Barrels, ItemPickups, Portals, COUNT };

enum class ESPSubCategory {
    Visible = 0,
    NonVisible,
    Single, // For categories that don't have visible/non-visible distinction
    COUNT
};

struct ESPCategoryInfo {
    ESPMainCategory mainCategory;
    ESPSubCategory subCategory;
    std::string displayName;

    ESPCategoryInfo(ESPMainCategory main, ESPSubCategory sub, const std::string& name) : mainCategory(main), subCategory(sub), displayName(name) {}
};

// Unified render item for hierarchical rendering
struct ESPHierarchicalRenderItem {
    ESPMainCategory mainCategory;
    ESPSubCategory subCategory;
    float distance;

    // Union to hold different ESP item types
    union {
        TrackedEntity* entity;
        TrackedInteractable* interactable;
        void* teleporterData; // For teleporter ESP
    };

    ImVec2 screenPos;
    bool isVisible;
    bool onScreen;
    bool isAvailable; // For interactables

    // Constructor for entities
    ESPHierarchicalRenderItem(ESPMainCategory main, ESPSubCategory sub, TrackedEntity* ent, float dist, const ImVec2& pos, bool visible, bool onScr)
        : mainCategory(main), subCategory(sub), distance(dist), entity(ent), screenPos(pos), isVisible(visible), onScreen(onScr), isAvailable(true) {}

    // Constructor for interactables
    ESPHierarchicalRenderItem(ESPMainCategory main, ESPSubCategory sub, TrackedInteractable* inter, float dist, const ImVec2& pos, bool visible, bool onScr,
                              bool available)
        : mainCategory(main), subCategory(sub), distance(dist), interactable(inter), screenPos(pos), isVisible(visible), onScreen(onScr),
          isAvailable(available) {}

    // Constructor for teleporter
    ESPHierarchicalRenderItem(ESPMainCategory main, ESPSubCategory sub, void* tele, float dist, const ImVec2& pos, bool visible, bool onScr)
        : mainCategory(main), subCategory(sub), distance(dist), teleporterData(tele), screenPos(pos), isVisible(visible), onScreen(onScr), isAvailable(true) {}
};

class ChestESPControl;

// Manager for ESP rendering order hierarchy
class ESPRenderOrderManager {
  private:
    std::vector<ESPMainCategory> m_mainCategoryOrder;
    std::map<ESPMainCategory, std::vector<ESPSubCategory>> m_subCategoryOrders;

  public:
    ESPRenderOrderManager();

    // Get/Set main category order
    const std::vector<ESPMainCategory>& GetMainOrder() const { return m_mainCategoryOrder; }
    void SetMainOrder(const std::vector<ESPMainCategory>& order) { m_mainCategoryOrder = order; }

    // Get/Set sub-category order for a main category
    const std::vector<ESPSubCategory>& GetSubOrder(ESPMainCategory mainCat) const;
    void SetSubOrder(ESPMainCategory mainCat, const std::vector<ESPSubCategory>& order);

    // Get all categories in hierarchical render order (reverse of priority for rendering)
    std::vector<ESPCategoryInfo> GetRenderOrder() const;

    // Move categories up/down in priority
    void MoveCategoryUp(ESPMainCategory category);
    void MoveCategoryDown(ESPMainCategory category);
    void MoveSubCategoryUp(ESPMainCategory mainCat, ESPSubCategory subCat);
    void MoveSubCategoryDown(ESPMainCategory mainCat, ESPSubCategory subCat);

    // Reset to default order
    void ResetToDefault();

    // Configuration validation
    bool ValidateConfiguration() const;
    void EnsureValidConfiguration();

    // Get display name for categories
    static std::string GetCategoryDisplayName(ESPMainCategory category);
    static std::string GetSubCategoryDisplayName(ESPSubCategory subCategory);

    // Config persistence
    std::string SerializeToString() const;
    void DeserializeFromString(const std::string& data);
};

class ESPModule : public ModuleBase {
  private:
    // Configuration control for render order persistence
    class RenderOrderConfigControl : public InputControl {
      private:
        ESPRenderOrderManager* m_manager;

      public:
        RenderOrderConfigControl(ESPRenderOrderManager* manager) : InputControl("ESP Render Order", "esp_render_order", false), m_manager(manager) {
            SetSaveEnabledState(false); // Don't save enabled state, only the order
        }

        void Draw() override {}   // No UI needed, handled by ESPModule::DrawRenderOrderUI()
        void Update() override {} // No updates needed

        json Serialize() const override {
            json data = InputControl::Serialize();
            if (m_manager) {
                data["renderOrder"] = m_manager->SerializeToString();
            }
            return data;
        }

        void Deserialize(const json& data) override {
            InputControl::Deserialize(data);
            if (m_manager && data.contains("renderOrder")) {
                m_manager->DeserializeFromString(data["renderOrder"]);
            }
        }
    };

    std::unique_ptr<RenderOrderConfigControl> m_renderOrderConfigControl;
    std::unique_ptr<ESPControl> teleporterESPControl;
    std::unique_ptr<EntityESPControl> playerESPControl;
    std::unique_ptr<EntityESPControl> enemyESPControl;
    std::unique_ptr<ChestESPControl> chestESPControl;
    std::unique_ptr<ChestESPControl> shopESPControl;
    std::unique_ptr<ChestESPControl> droneESPControl;
    std::unique_ptr<ChestESPControl> shrineESPControl;
    std::unique_ptr<ChestESPControl> specialESPControl;
    std::unique_ptr<ChestESPControl> barrelESPControl;
    std::unique_ptr<ChestESPControl> itemPickupESPControl;
    std::unique_ptr<ChestESPControl> portalESPControl;

    Vector3 teleporterPosition;
    std::string teleporterDisplayName;
    Vector3 playerPosition;
    Camera* mainCamera;

    std::vector<std::unique_ptr<TrackedEntity>> trackedEnemies;
    std::vector<std::unique_ptr<TrackedEntity>> trackedPlayers;
    std::vector<std::unique_ptr<TrackedInteractable>> trackedInteractables;
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
    std::string m_soulCostFormat;
    bool m_costFormatsInitialized;

    // Pickup name cache
    std::unordered_map<int32_t, std::string> m_pickupIdToNameCache;
    bool m_pickupCacheInitialized;

    // ESP rendering order management
    ESPRenderOrderManager m_renderOrderManager;

    // Category mapping lookup table
    struct CategoryMapping {
        ESPMainCategory mainCategory;
        ChestESPControl* control;
    };
    CategoryMapping m_categoryMappings[static_cast<int>(InteractableCategory::Unknown) + 1];

    // Main category to control lookup table
    ChestESPControl* m_mainCategoryControls[static_cast<int>(ESPMainCategory::COUNT)];

    void RenderEntityESP(TrackedEntity* entity, ImVec2 screenPos, float distance, EntityESPSubControl* control, bool isVisible, bool onScreen);
    void UpdateTimedChestDisplayName(TrackedInteractable* interactable, void* timedChestController);
    void UpdatePressurePlateDisplayName(TrackedInteractable* interactable, void* pressurePlateController);
    void RenderInteractableESP(TrackedInteractable* interactable, ImVec2 screenPos, float distance, ChestESPSubControl* control, bool isVisible, bool onScreen,
                               bool isAvailable);
    Vector3 GetCameraPosition();
    bool IsVisible(const Vector3& position);
    InteractableCategory DetermineInteractableCategory(PurchaseInteraction* pi, MonoString* nameToken);
    void InitializeCostFormats();
    void InitializeCategoryMappings();
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
    void OnStageAdvance(void* stage);
    void OnChestBehaviorSpawned(void* chestBehavior);
    void OnShopTerminalBehaviorSpawned(void* shopTerminalBehavior);
    void OnPressurePlateControllerSpawned(void* pressurePlateController);

    void CachePickupName(int32_t pickupIndex, const std::string& name);

    // Hierarchical rendering order management
    ESPRenderOrderManager& GetRenderOrderManager() { return m_renderOrderManager; }
    void DrawRenderOrderUI();

    // New hierarchical rendering methods
    void OnFrameRenderHierarchical();
    void CollectAllESPItems(std::vector<ESPHierarchicalRenderItem>& items);
    void RenderHierarchicalItem(const ESPHierarchicalRenderItem& item);
};
