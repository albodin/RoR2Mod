#include "ESPModule.hpp"
#include "config/ConfigManager.hpp"
#include "fonts/FontManager.hpp"
#include "globals/globals.hpp"
#include "hooks/hooks.hpp"
#include "utils/RenderUtils.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <imgui.h>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

// ESPRenderOrderManager implementation
ESPRenderOrderManager::ESPRenderOrderManager() { ResetToDefault(); }

const std::vector<ESPSubCategory>& ESPRenderOrderManager::GetSubOrder(ESPMainCategory mainCat) const {
    auto it = m_subCategoryOrders.find(mainCat);
    if (it != m_subCategoryOrders.end()) {
        return it->second;
    }
    static std::vector<ESPSubCategory> empty;
    return empty;
}

void ESPRenderOrderManager::SetSubOrder(ESPMainCategory mainCat, const std::vector<ESPSubCategory>& order) { m_subCategoryOrders[mainCat] = order; }

std::vector<ESPCategoryInfo> ESPRenderOrderManager::GetRenderOrder() const {
    std::vector<ESPCategoryInfo> renderOrder;

    // Use main category order directly (lowest priority first for proper rendering)
    for (ESPMainCategory mainCat : m_mainCategoryOrder) {
        auto it = m_subCategoryOrders.find(mainCat);
        if (it != m_subCategoryOrders.end()) {
            for (ESPSubCategory subCat : it->second) {
                std::string displayName = GetCategoryDisplayName(mainCat);
                if (subCat != ESPSubCategory::Single) {
                    displayName += " (" + GetSubCategoryDisplayName(subCat) + ")";
                }
                renderOrder.emplace_back(mainCat, subCat, displayName);
            }
        }
    }

    return renderOrder;
}

void ESPRenderOrderManager::MoveCategoryUp(ESPMainCategory category) {
    auto it = std::find(m_mainCategoryOrder.begin(), m_mainCategoryOrder.end(), category);
    if (it != m_mainCategoryOrder.end() && it != m_mainCategoryOrder.begin()) {
        std::swap(*it, *(it - 1));
    }
}

void ESPRenderOrderManager::MoveCategoryDown(ESPMainCategory category) {
    auto it = std::find(m_mainCategoryOrder.begin(), m_mainCategoryOrder.end(), category);
    if (it != m_mainCategoryOrder.end() && it != m_mainCategoryOrder.end() - 1) {
        std::swap(*it, *(it + 1));
    }
}

void ESPRenderOrderManager::MoveSubCategoryUp(ESPMainCategory mainCat, ESPSubCategory subCat) {
    auto& subOrder = m_subCategoryOrders[mainCat];
    auto it = std::find(subOrder.begin(), subOrder.end(), subCat);
    if (it != subOrder.end() && it != subOrder.begin()) {
        std::swap(*it, *(it - 1));
    }
}

void ESPRenderOrderManager::MoveSubCategoryDown(ESPMainCategory mainCat, ESPSubCategory subCat) {
    auto& subOrder = m_subCategoryOrders[mainCat];
    auto it = std::find(subOrder.begin(), subOrder.end(), subCat);
    if (it != subOrder.end() && it != subOrder.end() - 1) {
        std::swap(*it, *(it + 1));
    }
}

void ESPRenderOrderManager::ResetToDefault() {
    // Default main category order (matches current ESP rendering order)
    m_mainCategoryOrder = {ESPMainCategory::Teleporter, ESPMainCategory::Enemies,     ESPMainCategory::Players, ESPMainCategory::Chests,
                           ESPMainCategory::Shops,      ESPMainCategory::Drones,      ESPMainCategory::Shrines, ESPMainCategory::Specials,
                           ESPMainCategory::Barrels,    ESPMainCategory::ItemPickups, ESPMainCategory::Portals};

    // Default sub-category orders
    m_subCategoryOrders.clear();

    // Entities have Visible/NonVisible sub-categories
    m_subCategoryOrders[ESPMainCategory::Players] = {ESPSubCategory::Visible, ESPSubCategory::NonVisible};
    m_subCategoryOrders[ESPMainCategory::Enemies] = {ESPSubCategory::Visible, ESPSubCategory::NonVisible};

    // All others use Single sub-category
    for (int i = 0; i < static_cast<int>(ESPMainCategory::COUNT); i++) {
        ESPMainCategory cat = static_cast<ESPMainCategory>(i);
        if (cat != ESPMainCategory::Players && cat != ESPMainCategory::Enemies) {
            m_subCategoryOrders[cat] = {ESPSubCategory::Single};
        }
    }
}

bool ESPRenderOrderManager::ValidateConfiguration() const {
    // Check if main category order contains all categories exactly once
    if (m_mainCategoryOrder.size() != static_cast<int>(ESPMainCategory::COUNT)) {
        return false;
    }

    // Check for duplicates and ensure all categories are present
    std::vector<bool> categoryPresent(static_cast<int>(ESPMainCategory::COUNT), false);
    for (ESPMainCategory cat : m_mainCategoryOrder) {
        int index = static_cast<int>(cat);
        if (index < 0 || index >= static_cast<int>(ESPMainCategory::COUNT)) {
            return false; // Invalid category
        }
        if (categoryPresent[index]) {
            return false; // Duplicate category
        }
        categoryPresent[index] = true;
    }

    // Check that all categories are present
    for (bool present : categoryPresent) {
        if (!present) {
            return false;
        }
    }

    // Validate sub-category orders
    for (const auto& pair : m_subCategoryOrders) {
        ESPMainCategory mainCat = pair.first;
        const std::vector<ESPSubCategory>& subOrder = pair.second;

        // Check if main category is valid
        if (static_cast<int>(mainCat) < 0 || static_cast<int>(mainCat) >= static_cast<int>(ESPMainCategory::COUNT)) {
            return false;
        }

        // Check if sub-categories are valid and no duplicates
        std::vector<bool> subCategoryPresent(static_cast<int>(ESPSubCategory::COUNT), false);
        for (ESPSubCategory subCat : subOrder) {
            int index = static_cast<int>(subCat);
            if (index < 0 || index >= static_cast<int>(ESPSubCategory::COUNT)) {
                return false; // Invalid sub-category
            }
            if (subCategoryPresent[index]) {
                return false; // Duplicate sub-category
            }
            subCategoryPresent[index] = true;
        }
    }

    return true;
}

void ESPRenderOrderManager::EnsureValidConfiguration() {
    if (!ValidateConfiguration()) {
        ResetToDefault();
    }
}

std::string ESPRenderOrderManager::GetCategoryDisplayName(ESPMainCategory category) {
    switch (category) {
    case ESPMainCategory::Players:
        return "Players";
    case ESPMainCategory::Enemies:
        return "Enemies";
    case ESPMainCategory::Teleporter:
        return "Teleporter";
    case ESPMainCategory::Chests:
        return "Chests";
    case ESPMainCategory::Shops:
        return "Shops";
    case ESPMainCategory::Drones:
        return "Drones";
    case ESPMainCategory::Shrines:
        return "Shrines";
    case ESPMainCategory::Specials:
        return "Specials";
    case ESPMainCategory::Barrels:
        return "Barrels";
    case ESPMainCategory::ItemPickups:
        return "Item Pickups";
    case ESPMainCategory::Portals:
        return "Portals";
    default:
        return "Unknown";
    }
}

std::string ESPRenderOrderManager::GetSubCategoryDisplayName(ESPSubCategory subCategory) {
    switch (subCategory) {
    case ESPSubCategory::Visible:
        return "Visible";
    case ESPSubCategory::NonVisible:
        return "Non-Visible";
    case ESPSubCategory::Single:
        return "";
    default:
        return "Unknown";
    }
}

std::string ESPRenderOrderManager::SerializeToString() const {
    std::stringstream ss;

    // Serialize main order
    ss << "main:";
    for (size_t i = 0; i < m_mainCategoryOrder.size(); i++) {
        if (i > 0)
            ss << ",";
        ss << static_cast<int>(m_mainCategoryOrder[i]);
    }
    ss << ";";

    // Serialize sub orders
    for (const auto& pair : m_subCategoryOrders) {
        ss << "sub" << static_cast<int>(pair.first) << ":";
        for (size_t i = 0; i < pair.second.size(); i++) {
            if (i > 0)
                ss << ",";
            ss << static_cast<int>(pair.second[i]);
        }
        ss << ";";
    }

    return ss.str();
}

void ESPRenderOrderManager::DeserializeFromString(const std::string& data) {
    if (data.empty()) {
        ResetToDefault();
        return;
    }

    try {
        // Simple parsing - split by semicolon, then process each part
        std::stringstream ss(data);
        std::string segment;
        bool hasValidMainOrder = false;

        while (std::getline(ss, segment, ';')) {
            if (segment.empty())
                continue;

            size_t colonPos = segment.find(':');
            if (colonPos == std::string::npos)
                continue;

            std::string key = segment.substr(0, colonPos);
            std::string values = segment.substr(colonPos + 1);

            if (key == "main") {
                // Parse main category order
                std::vector<ESPMainCategory> tempOrder;
                std::stringstream valueStream(values);
                std::string value;

                while (std::getline(valueStream, value, ',')) {
                    if (value.empty())
                        continue;

                    try {
                        int catInt = std::stoi(value);
                        if (catInt >= 0 && catInt < static_cast<int>(ESPMainCategory::COUNT)) {
                            tempOrder.push_back(static_cast<ESPMainCategory>(catInt));
                        }
                    } catch (const std::exception&) {
                        // Skip invalid number formats
                        continue;
                    }
                }

                // Only accept the main order if it contains all categories
                if (tempOrder.size() == static_cast<int>(ESPMainCategory::COUNT)) {
                    m_mainCategoryOrder = tempOrder;
                    hasValidMainOrder = true;
                }
            } else if (key.length() > 3 && key.substr(0, 3) == "sub") {
                // Parse sub-category order
                try {
                    std::string mainCatStr = key.substr(3);
                    int mainCatInt = std::stoi(mainCatStr);

                    if (mainCatInt >= 0 && mainCatInt < static_cast<int>(ESPMainCategory::COUNT)) {
                        ESPMainCategory mainCat = static_cast<ESPMainCategory>(mainCatInt);
                        std::vector<ESPSubCategory> subOrder;

                        std::stringstream valueStream(values);
                        std::string value;
                        while (std::getline(valueStream, value, ',')) {
                            if (value.empty())
                                continue;

                            try {
                                int subCatInt = std::stoi(value);
                                if (subCatInt >= 0 && subCatInt < static_cast<int>(ESPSubCategory::COUNT)) {
                                    subOrder.push_back(static_cast<ESPSubCategory>(subCatInt));
                                }
                            } catch (const std::exception&) {
                                // Skip invalid number formats
                                continue;
                            }
                        }

                        // Only set sub-order if it's not empty
                        if (!subOrder.empty()) {
                            m_subCategoryOrders[mainCat] = subOrder;
                        }
                    }
                } catch (const std::exception&) {
                    // Skip invalid main category numbers
                    continue;
                }
            }
        }

        // If we didn't get a valid main order, reset to default
        if (!hasValidMainOrder) {
            ResetToDefault();
        }
    } catch (const std::exception&) {
        // If any critical error occurs, reset to default
        ResetToDefault();
    }
}

ESPModule::ESPModule() : ModuleBase() {
    m_costFormatsInitialized = false;
    m_pickupCacheInitialized = false;
    Initialize();
}

ESPModule::~ESPModule() {}

void ESPModule::Initialize() {
    teleporterESPControl = std::make_unique<ESPControl>("Teleporter ESP", "teleporter_esp", false, 250.0f, 1000.0f, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow

    playerESPControl = std::make_unique<EntityESPControl>("Players", "player_esp");
    enemyESPControl = std::make_unique<EntityESPControl>("Enemies", "enemy_esp");
    chestESPControl = std::make_unique<ChestESPControl>("Chests", "chest_esp");
    shopESPControl = std::make_unique<ChestESPControl>("Shops & Printers", "shop_esp");
    droneESPControl = std::make_unique<ChestESPControl>("Drones", "drone_esp");
    shrineESPControl = std::make_unique<ChestESPControl>("Shrines", "shrine_esp");
    specialESPControl = std::make_unique<ChestESPControl>("Special", "special_esp");
    barrelESPControl = std::make_unique<ChestESPControl>("Barrels", "barrel_esp");
    itemPickupESPControl = std::make_unique<ChestESPControl>("Item Pickups", "item_pickup_esp");
    portalESPControl = std::make_unique<ChestESPControl>("Portals", "portal_esp");

    // Initialize render order configuration control for persistence
    m_renderOrderConfigControl = std::make_unique<RenderOrderConfigControl>(&m_renderOrderManager);
    ConfigManager::RegisterControl(m_renderOrderConfigControl.get());

    // Initialize category mappings
    InitializeCategoryMappings();

    // Ensure render order configuration is valid
    m_renderOrderManager.EnsureValidConfiguration();
}

void ESPModule::Update() {
    teleporterESPControl->Update();
    playerESPControl->Update();
    enemyESPControl->Update();
    chestESPControl->Update();
    shopESPControl->Update();
    droneESPControl->Update();
    shrineESPControl->Update();
    specialESPControl->Update();
    barrelESPControl->Update();

    // Clean up consumed item pickups and command cubes
    {
        std::lock_guard<std::mutex> lock(interactablesMutex);
        trackedInteractables.erase(std::remove_if(trackedInteractables.begin(), trackedInteractables.end(),
                                                  [](const std::unique_ptr<TrackedInteractable>& tracked) {
                                                      if (tracked->category == InteractableCategory::ItemPickup && tracked->gameObject) {
                                                          // Check if the pickup has been consumed or recycled
                                                          GenericPickupController* gpc = static_cast<GenericPickupController*>(tracked->gameObject);
                                                          if (gpc->Recycled || gpc->consumed) {
                                                              return true;
                                                          }
                                                      } else if (tracked->category == InteractableCategory::CommandCube && tracked->gameObject) {
                                                          PickupPickerController* ppc = static_cast<PickupPickerController*>(tracked->gameObject);
                                                          if (!ppc->available) {
                                                              return true;
                                                          }
                                                      }
                                                      return false;
                                                  }),
                                   trackedInteractables.end());
    }
}

void ESPModule::InitializeCategoryMappings() {
    // Initialize InteractableCategory to ESPMainCategory mapping
    m_categoryMappings[static_cast<int>(InteractableCategory::Chest)] = {ESPMainCategory::Chests, chestESPControl.get()};
    m_categoryMappings[static_cast<int>(InteractableCategory::Shop)] = {ESPMainCategory::Shops, shopESPControl.get()};
    m_categoryMappings[static_cast<int>(InteractableCategory::Drone)] = {ESPMainCategory::Drones, droneESPControl.get()};
    m_categoryMappings[static_cast<int>(InteractableCategory::Shrine)] = {ESPMainCategory::Shrines, shrineESPControl.get()};
    m_categoryMappings[static_cast<int>(InteractableCategory::Special)] = {ESPMainCategory::Specials, specialESPControl.get()};
    m_categoryMappings[static_cast<int>(InteractableCategory::Barrel)] = {ESPMainCategory::Barrels, barrelESPControl.get()};
    m_categoryMappings[static_cast<int>(InteractableCategory::ItemPickup)] = {ESPMainCategory::ItemPickups, itemPickupESPControl.get()};
    m_categoryMappings[static_cast<int>(InteractableCategory::CommandCube)] = {ESPMainCategory::ItemPickups,
                                                                               itemPickupESPControl.get()}; // Display in ItemPickups
    m_categoryMappings[static_cast<int>(InteractableCategory::Portal)] = {ESPMainCategory::Portals, portalESPControl.get()};
    m_categoryMappings[static_cast<int>(InteractableCategory::Unknown)] = {ESPMainCategory::Specials, specialESPControl.get()};

    // Initialize ESPMainCategory to control mapping
    m_mainCategoryControls[static_cast<int>(ESPMainCategory::Chests)] = chestESPControl.get();
    m_mainCategoryControls[static_cast<int>(ESPMainCategory::Shops)] = shopESPControl.get();
    m_mainCategoryControls[static_cast<int>(ESPMainCategory::Drones)] = droneESPControl.get();
    m_mainCategoryControls[static_cast<int>(ESPMainCategory::Shrines)] = shrineESPControl.get();
    m_mainCategoryControls[static_cast<int>(ESPMainCategory::Specials)] = specialESPControl.get();
    m_mainCategoryControls[static_cast<int>(ESPMainCategory::Barrels)] = barrelESPControl.get();
    m_mainCategoryControls[static_cast<int>(ESPMainCategory::ItemPickups)] = itemPickupESPControl.get();
    m_mainCategoryControls[static_cast<int>(ESPMainCategory::Portals)] = portalESPControl.get();
    // Note: Players, Enemies, and Teleporter don't use ChestESPControl, so they're not included here
}

void ESPModule::DrawUI() {
    teleporterESPControl->Draw();
    playerESPControl->Draw();
    enemyESPControl->Draw();

    if (ImGui::CollapsingHeader("Interactables")) {
        chestESPControl->Draw();
        shopESPControl->Draw();
        droneESPControl->Draw();
        shrineESPControl->Draw();
        specialESPControl->Draw();
        barrelESPControl->Draw();
        itemPickupESPControl->Draw();
        portalESPControl->Draw();
    }

    if (ImGui::CollapsingHeader("ESP Rendering Order")) {
        DrawRenderOrderUI();
    }
}

void ESPModule::DrawRenderOrderUI() {
    ImGui::TextWrapped("Configure ESP rendering priority. Higher priority items appear on top of lower priority items.");
    ImGui::Separator();

    // Reset button
    if (ImGui::Button("Reset to Default")) {
        m_renderOrderManager.ResetToDefault();
    }

    ImGui::Separator();
    ImGui::Text("Rendering Priority (Higher = On Top):");

    // Get current main category order
    std::vector<ESPMainCategory> mainOrder = m_renderOrderManager.GetMainOrder();

    // Draw main categories in priority order (reverse of render order)
    for (int i = static_cast<int>(mainOrder.size()) - 1; i >= 0; i--) {
        ESPMainCategory category = mainOrder[i];
        std::string categoryName = ESPRenderOrderManager::GetCategoryDisplayName(category);

        ImGui::PushID(i);

        // Priority indicator
        ImGui::Text("%d.", static_cast<int>(mainOrder.size()) - i);
        ImGui::SameLine();

        // Category label
        ImGui::Text("%s", categoryName.c_str());

        // Position arrow buttons at a fixed X position for alignment
        ImGui::SameLine();
        float currentX = ImGui::GetCursorPosX();
        float targetX = 250.0f; // Fixed position for buttons
        if (currentX < targetX) {
            ImGui::SetCursorPosX(targetX);
        }

        // Up/Down buttons (swap the logic since UI is in reverse order)
        if (ImGui::ArrowButton("up", ImGuiDir_Up) && i < static_cast<int>(mainOrder.size()) - 1) {
            m_renderOrderManager.MoveCategoryDown(category); // Down in array = Up in UI priority
        }
        ImGui::SameLine();
        if (ImGui::ArrowButton("down", ImGuiDir_Down) && i > 0) {
            m_renderOrderManager.MoveCategoryUp(category); // Up in array = Down in UI priority
        }

        // Sub-category controls for Players and Enemies
        if (category == ESPMainCategory::Players || category == ESPMainCategory::Enemies) {
            ImGui::Indent(20.0f);

            std::vector<ESPSubCategory> subOrder = m_renderOrderManager.GetSubOrder(category);

            for (int j = static_cast<int>(subOrder.size()) - 1; j >= 0; j--) {
                ESPSubCategory subCat = subOrder[j];
                std::string subName = ESPRenderOrderManager::GetSubCategoryDisplayName(subCat);

                ImGui::PushID(100 + j); // Offset to avoid ID conflicts

                ImGui::Text("  %d.%d", static_cast<int>(mainOrder.size()) - i, static_cast<int>(subOrder.size()) - j);
                ImGui::SameLine();

                // Sub-category label
                ImGui::Text("%s", subName.c_str());

                // Position arrow buttons so sub-category down arrow aligns with main category up arrow
                ImGui::SameLine();
                float currentSubX = ImGui::GetCursorPosX();
                float mainButtonX = 250.0f; // Same as main category up arrow position

                // Get actual arrow button size
                float buttonSize = ImGui::GetFrameHeight();          // Arrow buttons are square, height = width
                float itemSpacing = ImGui::GetStyle().ItemSpacing.x; // Get spacing between items
                float actualButtonWidth = buttonSize + itemSpacing;

                float targetSubX = mainButtonX + actualButtonWidth; // Sub up arrow = main down arrow position
                if (currentSubX < targetSubX) {
                    ImGui::SetCursorPosX(targetSubX);
                }

                // Sub-category up/down buttons (swap the logic since UI is in reverse order)
                if (ImGui::ArrowButton("sub_up", ImGuiDir_Up) && j < static_cast<int>(subOrder.size()) - 1) {
                    m_renderOrderManager.MoveSubCategoryDown(category, subCat); // Down in array = Up in UI priority
                }
                ImGui::SameLine();
                if (ImGui::ArrowButton("sub_down", ImGuiDir_Down) && j > 0) {
                    m_renderOrderManager.MoveSubCategoryUp(category, subCat); // Up in array = Down in UI priority
                }

                ImGui::PopID();
            }

            ImGui::Unindent(20.0f);
        }

        ImGui::PopID();
        ImGui::Spacing();
    }
}

void ESPModule::OnFrameRender() {
    std::shared_ptr<std::vector<ESPHierarchicalRenderItem>> renderData = std::atomic_load(&collectedItemsBuffer);
    std::vector<ESPHierarchicalRenderItem> allItems = renderData ? *renderData : std::vector<ESPHierarchicalRenderItem>();

    if (allItems.empty())
        return;

    std::vector<ESPCategoryInfo> renderOrder = m_renderOrderManager.GetRenderOrder();

    // Group items by category and sub-category, then sort by distance within each group
    for (const auto& categoryInfo : renderOrder) {
        // Collect items for this specific category/sub-category
        std::vector<ESPHierarchicalRenderItem> categoryItems;
        for (const auto& item : allItems) {
            if (item.mainCategory == categoryInfo.mainCategory && item.subCategory == categoryInfo.subCategory) {
                categoryItems.push_back(item);
            }
        }

        if (categoryItems.empty())
            continue;

        // Sort by distance within this category (far to near for proper depth)
        std::sort(categoryItems.begin(), categoryItems.end(), [](const ESPHierarchicalRenderItem& a, const ESPHierarchicalRenderItem& b) {
            return a.distance > b.distance; // Descending order (far to near)
        });

        // Render all items in this category
        for (const auto& item : categoryItems) {
            RenderESPItem(item);
        }
    }
}

bool ESPModule::CalcEntityBounds(TrackedEntity* entity, ImVec2& outMin, ImVec2& outMax) {
    ImVec2 screenMin(FLT_MAX, FLT_MAX);
    ImVec2 screenMax(-FLT_MAX, -FLT_MAX);
    bool boundsFound = false;

    if (entity->body->hurtBoxGroup_backing && entity->body->hurtBoxGroup_backing->hurtBoxes) {
        Vector3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        uint32_t len = static_cast<uint32_t>((reinterpret_cast<MonoArray_Internal*>(entity->body->hurtBoxGroup_backing->hurtBoxes))->max_length);
        HurtBox** data = mono_array_addr<HurtBox*>(reinterpret_cast<MonoArray_Internal*>(entity->body->hurtBoxGroup_backing->hurtBoxes));

        for (uint32_t i = 0; i < len; ++i) {
            HurtBox* hurtBox = data[i];
            if (hurtBox) {
                // TODO: Fix this component access as it can crash
                Transform* hurtBoxTransform = static_cast<Transform*>(Hooks::Component_get_transform(hurtBox));
                if (hurtBoxTransform) {
                    Vector3 hurtBoxPos;
                    Hooks::Transform_get_position_Injected(hurtBoxTransform, &hurtBoxPos);

                    if (!boundsFound) {
                        minBounds = hurtBoxPos;
                        maxBounds = hurtBoxPos;
                        boundsFound = true;
                    } else {
                        minBounds.x = std::min(minBounds.x, hurtBoxPos.x);
                        minBounds.y = std::min(minBounds.y, hurtBoxPos.y);
                        minBounds.z = std::min(minBounds.z, hurtBoxPos.z);
                        maxBounds.x = std::max(maxBounds.x, hurtBoxPos.x);
                        maxBounds.y = std::max(maxBounds.y, hurtBoxPos.y);
                        maxBounds.z = std::max(maxBounds.z, hurtBoxPos.z);
                    }
                }
            }
        }

        if (boundsFound) {
            Vector3 corners[8] = {Vector3(minBounds.x, minBounds.y, minBounds.z), Vector3(maxBounds.x, minBounds.y, minBounds.z),
                                  Vector3(minBounds.x, maxBounds.y, minBounds.z), Vector3(maxBounds.x, maxBounds.y, minBounds.z),
                                  Vector3(minBounds.x, minBounds.y, maxBounds.z), Vector3(maxBounds.x, minBounds.y, maxBounds.z),
                                  Vector3(minBounds.x, maxBounds.y, maxBounds.z), Vector3(maxBounds.x, maxBounds.y, maxBounds.z)};

            int visibleCorners = 0;

            for (int i = 0; i < 8; i++) {
                ImVec2 cornerScreen;
                if (RenderUtils::WorldToScreen(cameraCache, corners[i], cornerScreen)) {
                    if (visibleCorners == 0) {
                        screenMin = cornerScreen;
                        screenMax = cornerScreen;
                    } else {
                        screenMin.x = std::min(screenMin.x, cornerScreen.x);
                        screenMin.y = std::min(screenMin.y, cornerScreen.y);
                        screenMax.x = std::max(screenMax.x, cornerScreen.x);
                        screenMax.y = std::max(screenMax.y, cornerScreen.y);
                    }
                    visibleCorners++;
                }
            }

            if (visibleCorners == 0) {
                boundsFound = false;
            }
        }
    }

    outMin = screenMin;
    outMax = screenMax;
    return boundsFound;
}

void ESPModule::CollectAllESPItems(std::vector<ESPHierarchicalRenderItem>& items) {
    if (!G::runInstance || !mainCamera || !G::localPlayer->GetPlayerPosition()) {
        return;
    }

    Vector3 localPlayerPos = G::localPlayer->GetPlayerPosition();

    // Collect teleporter ESP
    if (teleporterESPControl->IsEnabled()) {
        std::lock_guard<std::mutex> lock(teleportersMutex);
        for (const auto& teleporter : trackedTeleporters) {
            if (!teleporter)
                continue;

            float distance = teleporter->position.Distance(localPlayerPos);
            if (distance > teleporterESPControl->GetDistance())
                continue;

            bool isVisible = IsVisible(teleporter->position);
            items.emplace_back(ESPMainCategory::Teleporter, ESPSubCategory::Single, teleporter.get(), teleporter->position, distance, isVisible);
        }
    }

    // Collect player ESP
    if (playerESPControl->IsMasterEnabled()) {
        std::lock_guard<std::mutex> lock(entitiesMutex);
        for (const auto& entity : trackedPlayers) {
            if (!entity->body || !entity->body->transform || !entity->body->healthComponent_backing)
                continue;
            if (entity->body == G::localPlayer->GetLocalPlayerBody())
                continue;
            if (entity->body->healthComponent_backing->health <= 0)
                continue;

            Vector3 playerWorldPos;
            Hooks::Transform_get_position_Injected(entity->body->transform, &playerWorldPos);
            float distance = playerWorldPos.Distance(localPlayerPos);
            bool isVisible = IsVisible(playerWorldPos);

            EntityESPSubControl* control = isVisible ? playerESPControl->GetVisibleControl() : playerESPControl->GetNonVisibleControl();

            if (!control->IsEnabled() || distance > control->GetMaxDistance())
                continue;

            ImVec2 boundsMin, boundsMax;
            bool foundBounds = CalcEntityBounds(entity.get(), boundsMin, boundsMax);

            ESPSubCategory subCat = isVisible ? ESPSubCategory::Visible : ESPSubCategory::NonVisible;
            items.emplace_back(ESPMainCategory::Players, subCat, entity.get(), playerWorldPos, distance, isVisible, foundBounds, boundsMin, boundsMax);
        }
    }

    // Collect enemy ESP
    if (enemyESPControl->IsMasterEnabled()) {
        std::lock_guard<std::mutex> lock(entitiesMutex);
        for (const auto& entity : trackedEnemies) {
            if (!entity->body || !entity->body->transform || !entity->body->healthComponent_backing)
                continue;
            if (entity->body->healthComponent_backing->health <= 0)
                continue;

            Vector3 enemyWorldPos;
            Hooks::Transform_get_position_Injected(entity->body->transform, &enemyWorldPos);
            float distance = enemyWorldPos.Distance(localPlayerPos);
            bool isVisible = IsVisible(enemyWorldPos);

            EntityESPSubControl* control = isVisible ? enemyESPControl->GetVisibleControl() : enemyESPControl->GetNonVisibleControl();

            if (!control->IsEnabled() || distance > control->GetMaxDistance())
                continue;

            ImVec2 boundsMin, boundsMax;
            bool foundBounds = CalcEntityBounds(entity.get(), boundsMin, boundsMax);

            ESPSubCategory subCat = isVisible ? ESPSubCategory::Visible : ESPSubCategory::NonVisible;
            items.emplace_back(ESPMainCategory::Enemies, subCat, entity.get(), enemyWorldPos, distance, isVisible, foundBounds, boundsMin, boundsMax);
        }
    }

    // Collect interactable ESP
    {
        std::lock_guard<std::mutex> lock(interactablesMutex);
        for (const auto& interactable : trackedInteractables) {
            if (!interactable || !interactable->gameObject)
                continue;

            // Map interactable categories to main categories using lookup table
            int categoryIndex = static_cast<int>(interactable->category);
            if (categoryIndex < 0 || categoryIndex > static_cast<int>(InteractableCategory::Unknown)) {
                categoryIndex = static_cast<int>(InteractableCategory::Unknown);
            }

            const CategoryMapping& mapping = m_categoryMappings[categoryIndex];
            ESPMainCategory mainCategory = mapping.mainCategory;
            ChestESPControl* categoryControl = mapping.control;

            if (!categoryControl || !categoryControl->IsMasterEnabled())
                continue;

            // Get current position of portals
            Vector3 currentPosition = interactable->position;
            if (interactable->category == InteractableCategory::Portal && interactable->gameObject) {
                if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
                    void* transform = Hooks::Component_get_transform(interactable->gameObject);
                    if (transform) {
                        Hooks::Transform_get_position_Injected(transform, &currentPosition);
                    }
                }
            }

            float distance = currentPosition.Distance(localPlayerPos);
            ChestESPSubControl* control = categoryControl->GetSubControl();
            if (!control->IsEnabled() || distance > control->GetMaxDistance())
                continue;

            // Check availability
            bool isAvailable = true;
            if (interactable->category == InteractableCategory::Barrel && interactable->gameObject) {
                if (interactable->purchaseInteraction) {
                    PurchaseInteraction* pi = static_cast<PurchaseInteraction*>(interactable->purchaseInteraction);
                    isAvailable = pi->available;
                } else {
                    BarrelInteraction* barrel = static_cast<BarrelInteraction*>(interactable->gameObject);
                    isAvailable = !barrel->opened;
                }
            } else if (interactable->category == InteractableCategory::ItemPickup && interactable->gameObject) {
                GenericPickupController* gpc = static_cast<GenericPickupController*>(interactable->gameObject);
                isAvailable = !gpc->consumed && !gpc->Recycled;
            } else if (interactable->category == InteractableCategory::CommandCube) {
                PickupPickerController* pcc = static_cast<PickupPickerController*>(interactable->gameObject);
                isAvailable = pcc->available;
            } else if (interactable->purchaseInteraction) {
                PurchaseInteraction* pi = static_cast<PurchaseInteraction*>(interactable->purchaseInteraction);
                isAvailable = pi->available;
            }

            if (!isAvailable && !control->ShouldShowUnavailable())
                continue;

            bool isVisible = IsVisible(currentPosition);
            items.emplace_back(mainCategory, ESPSubCategory::Single, interactable.get(), currentPosition, distance, isVisible, isAvailable);
        }
    }
}

void ESPModule::RenderESPItem(const ESPHierarchicalRenderItem& item) {
    // Render based on category type
    if (item.mainCategory == ESPMainCategory::Teleporter) {
        // Render teleporter
        if (!item.teleporterData || !G::localPlayer->GetPlayerPosition())
            return;

        ImVec2 screenPos;
        bool onScreen = false;
        if (!RenderUtils::WorldToScreen(cameraCache, item.worldPosition, screenPos, onScreen))
            return;

        TrackedTeleporter* teleporter = static_cast<TrackedTeleporter*>(item.teleporterData);
        const char* baseName = teleporter->displayName.empty() ? "Teleporter" : teleporter->displayName.c_str();

        char teleporterText[256];
        snprintf(teleporterText, sizeof(teleporterText), "%s (%dm)", baseName, static_cast<int>(item.distance));

        RenderUtils::RenderText(screenPos, teleporterESPControl->GetColorU32(), teleporterESPControl->GetOutlineColorU32(),
                                teleporterESPControl->IsOutlineEnabled(), true, "%s", teleporterText);

    } else if (item.mainCategory == ESPMainCategory::Players || item.mainCategory == ESPMainCategory::Enemies) {
        // Render entity
        EntityESPControl* control = (item.mainCategory == ESPMainCategory::Players) ? playerESPControl.get() : enemyESPControl.get();

        EntityESPSubControl* subControl = item.isVisible ? control->GetVisibleControl() : control->GetNonVisibleControl();

        ImVec2 screenPos;
        bool onScreen = false;
        if (!RenderUtils::WorldToScreen(cameraCache, item.worldPosition, screenPos, onScreen))
            return;

        RenderEntityESP(item.entity, screenPos, item.distance, subControl, item.isVisible, onScreen, item.foundBounds, item.boundsMin, item.boundsMax);

    } else {
        // Render interactable using lookup table
        int categoryIndex = static_cast<int>(item.mainCategory);
        ChestESPControl* categoryControl = nullptr;

        if (categoryIndex >= 0 && categoryIndex < static_cast<int>(ESPMainCategory::COUNT) &&
            (categoryIndex == static_cast<int>(ESPMainCategory::Chests) || categoryIndex == static_cast<int>(ESPMainCategory::Shops) ||
             categoryIndex == static_cast<int>(ESPMainCategory::Drones) || categoryIndex == static_cast<int>(ESPMainCategory::Shrines) ||
             categoryIndex == static_cast<int>(ESPMainCategory::Specials) || categoryIndex == static_cast<int>(ESPMainCategory::Barrels) ||
             categoryIndex == static_cast<int>(ESPMainCategory::ItemPickups) || categoryIndex == static_cast<int>(ESPMainCategory::Portals))) {
            categoryControl = m_mainCategoryControls[categoryIndex];
        } else {
            categoryControl = specialESPControl.get(); // Default fallback
        }

        if (categoryControl) {
            ChestESPSubControl* control = categoryControl->GetSubControl();
            ImVec2 screenPos;
            bool onScreen = false;
            if (!RenderUtils::WorldToScreen(cameraCache, item.worldPosition, screenPos, onScreen))
                return;
            RenderInteractableESP(item.interactable, screenPos, item.distance, control, item.isVisible, onScreen, item.isAvailable);
        }
    }
}

void ESPModule::OnGameUpdate() {
    mainCamera = Hooks::Camera_get_main();
    if (!mainCamera) {
        LOG_ERROR("Camera_get_main returned null");
    }

    std::shared_ptr<CachedCameraData> newCache = std::make_shared<CachedCameraData>();
    RenderUtils::PrecomputeViewProjection(mainCamera, newCache.get());
    std::shared_ptr<CachedCameraData> curCache = std::atomic_load(&cameraCache);
    while (!std::atomic_compare_exchange_weak(&cameraCache, &curCache, newCache)) {
    }

    std::shared_ptr<std::vector<ESPHierarchicalRenderItem>> curBuffer = std::atomic_load(&collectedItemsBuffer);
    std::shared_ptr<std::vector<ESPHierarchicalRenderItem>> newBuffer = std::make_shared<std::vector<ESPHierarchicalRenderItem>>();
    CollectAllESPItems(*newBuffer);

    while (!std::atomic_compare_exchange_weak(&collectedItemsBuffer, &curBuffer, newBuffer)) {
    }
}

void ESPModule::OnTeleporterAwake(void* teleporter) {
    if (!teleporter)
        return;

    TeleporterInteraction* teleporter_ptr = static_cast<TeleporterInteraction*>(teleporter);
    if (!teleporter_ptr || !teleporter_ptr->teleporterPositionIndicator)
        return;

    Vector3 position = {0, 0, 0};
    Hooks::Transform_get_position_Injected(teleporter_ptr->teleporterPositionIndicator->targetTransform, &position);

    std::string displayName = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("TELEPORTER_NAME"));

    auto trackedTeleporter = std::make_unique<TrackedTeleporter>();
    trackedTeleporter->teleporterInteraction = teleporter;
    trackedTeleporter->position = position;
    trackedTeleporter->displayName = displayName;

    std::lock_guard<std::mutex> lock(teleportersMutex);
    trackedTeleporters.push_back(std::move(trackedTeleporter));

    LOG_INFO("Tracked new teleporter at position (%.1f, %.1f, %.1f)", position.x, position.y, position.z);
}

void ESPModule::OnTeleporterDestroyed(void* teleporter) {
    if (!teleporter)
        return;

    std::lock_guard<std::mutex> lock(teleportersMutex);
    trackedTeleporters.erase(
        std::remove_if(trackedTeleporters.begin(), trackedTeleporters.end(),
                       [teleporter](const std::unique_ptr<TrackedTeleporter>& tracked) { return tracked->teleporterInteraction == teleporter; }),
        trackedTeleporters.end());
}

void ESPModule::OnTeleporterFixedUpdate(void* teleporter) {
    if (!teleporter)
        return;

    TeleporterInteraction* teleporter_ptr = static_cast<TeleporterInteraction*>(teleporter);
    if (!teleporter_ptr || !teleporter_ptr->teleporterPositionIndicator)
        return;

    std::lock_guard<std::mutex> lock(teleportersMutex);

    // Find the tracked teleporter and update its position
    for (auto& tracked : trackedTeleporters) {
        if (tracked->teleporterInteraction == teleporter) {
            Vector3 newPosition = {0, 0, 0};
            Hooks::Transform_get_position_Injected(teleporter_ptr->teleporterPositionIndicator->targetTransform, &newPosition);
            tracked->position = newPosition;
            break;
        }
    }
}

void ESPModule::OnCharacterBodySpawned(void* characterBody) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    CharacterBody* body = static_cast<CharacterBody*>(characterBody);

    if (!body)
        return;

    auto newEntity = std::make_unique<TrackedEntity>();
    newEntity->displayName = G::gameFunctions->Language_GetString(static_cast<MonoString*>(body->baseNameToken));
    newEntity->body = body;

    TeamIndex_Value teamIndex = body->teamComponent_backing->_teamIndex;

    // For players, try to get the actual username
    if (teamIndex == TeamIndex_Value::Player) {
        if (body->_master && body->_master->playerCharacterMasterController_backing) {
            PlayerCharacterMasterController* pcmc = body->_master->playerCharacterMasterController_backing;
            if (pcmc->resolvedNetworkUserInstance && pcmc->resolvedNetworkUserInstance->userName) {
                std::string playerName = G::g_monoRuntime->StringToUtf8(static_cast<MonoString*>(pcmc->resolvedNetworkUserInstance->userName));
                if (!playerName.empty()) {
                    newEntity->displayName = playerName;
                }
            }
        }
    }

    // Categorize by team
    switch (teamIndex) {
    case TeamIndex_Value::Monster:
    case TeamIndex_Value::Lunar:
    case TeamIndex_Value::Void:
        trackedEnemies.push_back(std::move(newEntity));
        break;
    case TeamIndex_Value::Player:
        trackedPlayers.push_back(std::move(newEntity));
        break;

    default:
        break;
    }
}

void ESPModule::OnCharacterBodyDestroyed(void* characterBody) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    CharacterBody* body = static_cast<CharacterBody*>(characterBody);

    trackedEnemies.erase(
        std::remove_if(trackedEnemies.begin(), trackedEnemies.end(), [body](const std::unique_ptr<TrackedEntity>& enemy) { return enemy->body == body; }),
        trackedEnemies.end());

    trackedPlayers.erase(
        std::remove_if(trackedPlayers.begin(), trackedPlayers.end(), [body](const std::unique_ptr<TrackedEntity>& player) { return player->body == body; }),
        trackedPlayers.end());
}

void ESPModule::RenderEntityESP(TrackedEntity* entity, ImVec2 screenPos, float distance, EntityESPSubControl* control, bool isVisible, bool onScreen,
                                bool hasBounds, ImVec2 screenMin, ImVec2 screenMax) {
    if (!entity || !control)
        return;

    // If off-screen, only draw traceline and return
    if (!onScreen) {
        if (control->ShouldShowTraceline()) {
            ImVec2 screenCenter(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y);
            RenderUtils::RenderLine(screenCenter, screenPos, control->GetTracelineColorU32(), 1.0f);
        }
        return;
    }

    float lineHeight = FontManager::ESPFontSize;
    static float boxBorderThickness = 2.0f;

    // Fallback bounds if we couldn't calculate from hurtboxes or bounds are too small
    bool useFallbackBounds = false;
    if (hasBounds) {
        // Check if bounds are too small (like wisps) - if box is smaller than 5x5 pixels, use fallback
        ImVec2 currentBoxSize(screenMax.x - screenMin.x, screenMax.y - screenMin.y);
        if (currentBoxSize.x < 5.0f || currentBoxSize.y < 5.0f) {
            useFallbackBounds = true;
        }
    } else {
        useFallbackBounds = true;
    }

    if (useFallbackBounds) {
        // Scale fallback bounds based on distance - smaller at far distances
        float distanceScale = std::max(0.3f, std::min(1.0f, 50.0f / distance)); // Scale from 1.0 at 50m to 0.3 at far distances
        float halfWidth = 30.0f * distanceScale;
        float halfHeight = 40.0f * distanceScale;
        screenMin = ImVec2(screenPos.x - halfWidth, screenPos.y - halfHeight);
        screenMax = ImVec2(screenPos.x + halfWidth, screenPos.y + halfHeight);
    }

    ImVec2 boxSize(screenMax.x - screenMin.x, screenMax.y - screenMin.y);
    ImVec2 boxBottomCenter(screenMin.x + boxSize.x / 2, screenMax.y);
    ImVec2 textPos = boxBottomCenter;
    textPos.y += 5; // Small gap below the box

    // Render traceline first so it appears under everything else
    if (control->ShouldShowTraceline()) {
        ImVec2 screenCenter(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y);
        RenderUtils::RenderLine(screenCenter, boxBottomCenter, control->GetTracelineColorU32(), 1.0f);
    }

    if (control->ShouldShowBox()) {
        RenderUtils::RenderBox(screenMin, boxSize, control->GetBoxColorU32(), boxBorderThickness);
    }

    if (control->ShouldShowName() && !entity->displayName.empty()) {
        RenderUtils::RenderText(textPos, control->GetNameColorU32(), IM_COL32(0, 0, 0, 255), true, true, "%s", entity->displayName.c_str());
        textPos.y += lineHeight;
    }

    if (entity->body->healthComponent_backing) {
        bool showHealth = control->ShouldShowHealth();
        bool showMaxHealth = control->ShouldShowMaxHealth();

        if (showHealth && showMaxHealth) {
            char healthPart[64];
            char maxHealthPart[32];
            snprintf(healthPart, sizeof(healthPart), "HP: %d", static_cast<int>(entity->body->healthComponent_backing->health));
            snprintf(maxHealthPart, sizeof(maxHealthPart), "/%d", static_cast<int>(entity->body->maxHealth_backing));

            ImFont* font = FontManager::GetESPFont();
            float scale = FontManager::ESPFontSize / font->FontSize;
            ImVec2 healthPartSize = ImGui::CalcTextSize(healthPart);
            ImVec2 maxHealthPartSize = ImGui::CalcTextSize(maxHealthPart);
            healthPartSize.x *= scale;
            maxHealthPartSize.x *= scale;
            float totalWidth = healthPartSize.x + maxHealthPartSize.x;

            // Render health part (left side, adjusted for centering)
            ImVec2 healthPartPos = ImVec2(textPos.x - totalWidth / 2, textPos.y);
            RenderUtils::RenderText(healthPartPos, control->GetHealthColorU32(), IM_COL32(0, 0, 0, 255), true, false, healthPart);

            // Render max health part (right side)
            ImVec2 maxHealthPartPos = ImVec2(healthPartPos.x + healthPartSize.x, textPos.y);
            RenderUtils::RenderText(maxHealthPartPos, control->GetMaxHealthColorU32(), IM_COL32(0, 0, 0, 255), true, false, maxHealthPart);

            textPos.y += lineHeight;
        } else if (showHealth) {
            char healthText[64];
            snprintf(healthText, sizeof(healthText), "HP: %d", static_cast<int>(entity->body->healthComponent_backing->health));
            RenderUtils::RenderText(textPos, control->GetHealthColorU32(), IM_COL32(0, 0, 0, 255), true, true, healthText);
            textPos.y += lineHeight;
        } else if (showMaxHealth) {
            char maxHealthText[64];
            snprintf(maxHealthText, sizeof(maxHealthText), "Max HP: %d", static_cast<int>(entity->body->maxHealth_backing));
            RenderUtils::RenderText(textPos, control->GetMaxHealthColorU32(), IM_COL32(0, 0, 0, 255), true, true, maxHealthText);
            textPos.y += lineHeight;
        }
    }

    if (control->ShouldShowDistance()) {
        char distanceText[32];
        snprintf(distanceText, sizeof(distanceText), "%dm", static_cast<int>(distance));
        RenderUtils::RenderText(textPos, control->GetDistanceColorU32(), IM_COL32(0, 0, 0, 255), true, true, distanceText);
        textPos.y += lineHeight;
    }

    if (control->ShouldShowHealthbar() && entity->body->healthComponent_backing) {
        ImVec2 healthbarPos(screenMin.x - 8, screenMin.y - boxBorderThickness / 2);
        ImVec2 healthbarSize(8, boxSize.y + boxBorderThickness);
        float health = entity->body->healthComponent_backing->health;
        float maxHealth = entity->body->maxHealth_backing;

        float healthRatio = std::clamp(health / maxHealth, 0.0f, 1.0f);
        float red = std::min(2.0f * (1.0f - healthRatio), 1.0f);
        float green = std::min(2.0f * healthRatio, 1.0f);
        ImU32 healthColor = IM_COL32(static_cast<int>(red * 255), static_cast<int>(green * 255), 0, 255);

        RenderUtils::RenderHealthbar(healthbarPos, healthbarSize, health, maxHealth, healthColor, IM_COL32(50, 50, 50, 180));
    }
}

Vector3 ESPModule::GetCameraPosition() {
    Camera* camera = Hooks::Camera_get_main();
    if (!camera || !Hooks::Component_get_transform) {
        return Vector3{0, 0, 0};
    }

    void* transform = Hooks::Component_get_transform(static_cast<void*>(camera));
    if (!transform) {
        return Vector3{0, 0, 0};
    }

    Vector3 position;
    Hooks::Transform_get_position_Injected(transform, &position);
    return position;
}

bool ESPModule::IsVisible(const Vector3& position) {
    if (!Hooks::Physics_get_defaultPhysicsScene_Injected || !Hooks::PhysicsScene_Internal_Raycast_Injected) {
        return true;
    }

    Vector3 cameraPos = GetCameraPosition();
    Vector3 direction = position - cameraPos;
    float distance = direction.Length();

    if (distance < 0.1f) {
        return true;
    }

    direction.Normalize();

    PhysicsScene_Value defaultScene = {};
    Hooks::Physics_get_defaultPhysicsScene_Injected(&defaultScene);

    Ray_Value ray;
    ray.m_Origin = cameraPos;
    ray.m_Direction = direction;

    RaycastHit_Value hitInfo = {};

    int32_t layerMask = 0;
    if (G::worldLayer >= 0)
        layerMask |= (1 << G::worldLayer);
    if (G::ignoreRaycastLayer >= 0)
        layerMask |= (1 << G::ignoreRaycastLayer);

    bool hitSomething =
        Hooks::PhysicsScene_Internal_Raycast_Injected(&defaultScene, &ray, distance - 1.0f, &hitInfo, layerMask, QueryTriggerInteraction_Value::UseGlobal);

    return !hitSomething;
}

InteractableCategory ESPModule::DetermineInteractableCategory(PurchaseInteraction* pi, MonoString* nameToken) {
    // Check for shrines first
    if (pi->isShrine || pi->isGoldShrine) {
        return InteractableCategory::Shrine;
    }

    std::string token = "";
    if (nameToken) {
        token = G::g_monoRuntime->StringToUtf8(nameToken);
    }

    if (token.find("SHRINE_CLEANSE_") != std::string::npos || // Cleansing Pool
        token.find("DUPLICATOR_") != std::string::npos ||     // 3D Printer variants
        token.find("MULTISHOP_TERMINAL_") != std::string::npos || token.find("BAZAAR_CAULDRON_") != std::string::npos ||
        token.find("SCRAPPER_") != std::string::npos) {
        return InteractableCategory::Shop;
    }

    if (token.find("SHRINE_") != std::string::npos) {
        return InteractableCategory::Shrine;
    }

    if (token.find("DRONE_") != std::string::npos || token.find("TURRET1_") != std::string::npos) {
        return InteractableCategory::Drone;
    }

    if (token.find("PORTAL_") != std::string::npos && token.find("PORTAL_DIALER") == std::string::npos) {
        return InteractableCategory::Portal;
    }

    // Chests - CHEST covers all variants (LUNAR_CHEST_, TIMEDCHEST_, etc.)
    if (token.find("CHEST") != std::string::npos || token.find("LOCKBOX") != std::string::npos || token.find("SCAVBACKPACK_") != std::string::npos) {
        return InteractableCategory::Chest;
    }

    if (token.find("BARREL") != std::string::npos) {
        return InteractableCategory::Barrel;
    }

    // Special interactables
    if (token.find("NEWT_STATUE_") != std::string::npos ||                                                  // Newt Altar
        token.find("MOON_BATTERY_") != std::string::npos ||                                                 // Pillars
        token.find("FROG_") != std::string::npos || token.find("BAZAAR_SEER_") != std::string::npos ||      // Lunar Seer
        token.find("TELEPORTER_") != std::string::npos || token.find("GOLDTOTEM_") != std::string::npos ||  // Halcyon Beacon
        token.find("MSOBELISK_") != std::string::npos ||                                                    // Obelisk
        token.find("DEEPVOIDBATTERY_") != std::string::npos ||                                              // Deep Void Signal
        token.find("NULL_WARD_") != std::string::npos ||                                                    // Cell Vent
        token.find("RADIOTOWER_") != std::string::npos ||                                                   // Radio Scanner
        token.find("FAN_") != std::string::npos || token.find("LOCKEDTREEBOT_") != std::string::npos ||     // Broken Robot
        token.find("PORTAL_DIALER_") != std::string::npos ||                                                // Compound Generator
        token.find("ARTIFACT_TRIAL_") != std::string::npos ||                                               // Artifact Reliquary
        token.find("LUNAR_TERMINAL_") != std::string::npos ||                                               // Lunar Buds
        token.find("LUNAR_REROLL_") != std::string::npos ||                                                 // Lunar Shop Refresher
        token.find("LOCKEDMAGE_") != std::string::npos ||                                                   // Survivor Suspended In Time
        token.find("ZIPLINE_") != std::string::npos ||                                                      // Quantum Tunnel
        token.find("VENDING_MACHINE_") != std::string::npos || token.find("GEODE_") != std::string::npos || // Aurelionite Geode
        token.find("INFINITE_TOWER_SAFE_WARD_") != std::string::npos ||                                     // Assessment Focus (Simulacrum)
        token.find("VOID_SUPPRESSOR_") != std::string::npos ||                                              // Void Suppressor
        token.find("VOID_TRIPLE_") != std::string::npos) {                                                  // Void Potential
        return InteractableCategory::Special;
    }

    // Everything else is unknown
    return InteractableCategory::Unknown;
}

void ESPModule::OnPurchaseInteractionSpawned(void* purchaseInteraction) {
    if (!purchaseInteraction)
        return;

    PurchaseInteraction* pi = static_cast<PurchaseInteraction*>(purchaseInteraction);

    void* gameObject = purchaseInteraction;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(purchaseInteraction);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    std::string displayName = G::gameFunctions->Language_GetString(static_cast<MonoString*>(pi->displayNameToken));

    // Skip interactables with empty or whitespace-only names
    if (displayName.empty() || displayName.find_first_not_of(" \t\r\n") == std::string::npos) {
        return;
    }

    // Determine category using language-independent token
    InteractableCategory category = DetermineInteractableCategory(pi, static_cast<MonoString*>(pi->displayNameToken));

    // Log error if unknown interactable found
    if (category == InteractableCategory::Unknown) {
        std::string token = "";
        if (pi->displayNameToken) {
            token = G::g_monoRuntime->StringToUtf8(static_cast<MonoString*>(pi->displayNameToken));
        }
        LOG_ERROR("Unknown PurchaseInteraction detected: token=\"%s\" display=\"%s\" cost=%d isShrine=%d - Please report this to the developers!", token,
                  displayName, pi->cost, pi->isShrine);
        displayName += " (UNKNOWN)";
    }

    // Create interactable tracking info
    auto trackedInteractable = std::make_unique<TrackedInteractable>();
    trackedInteractable->gameObject = gameObject;
    trackedInteractable->purchaseInteraction = purchaseInteraction;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = "";
    if (pi->displayNameToken) {
        trackedInteractable->nameToken = G::g_monoRuntime->StringToUtf8(static_cast<MonoString*>(pi->displayNameToken));
    }
    trackedInteractable->category = category;
    trackedInteractable->specialType = SpecialInteractableType::None;
    trackedInteractable->consumed = false;
    trackedInteractable->pickupIndex = -1;
    trackedInteractable->itemName = "";

    // Get the localized cost string
    trackedInteractable->costString = "";
    if (pi->cost > 0) {
        trackedInteractable->costString = GetCostString(pi->costType, pi->cost);
    }

    // Add to tracked interactables
    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(std::move(trackedInteractable));

    const char* categoryName = "";
    switch (category) {
    case InteractableCategory::Chest:
        categoryName = "Chest";
        break;
    case InteractableCategory::Shop:
        categoryName = "Shop";
        break;
    case InteractableCategory::Drone:
        categoryName = "Drone";
        break;
    case InteractableCategory::Shrine:
        categoryName = "Shrine";
        break;
    case InteractableCategory::Special:
        categoryName = "Special";
        break;
    case InteractableCategory::Barrel:
        categoryName = "Barrel";
        break;
    case InteractableCategory::ItemPickup:
        categoryName = "ItemPickup";
        break;
    case InteractableCategory::Portal:
        categoryName = "Portal";
        break;
    case InteractableCategory::Unknown:
        categoryName = "Unknown";
        break;
    }
}

void ESPModule::OnPurchaseInteractionDestroyed(void* purchaseInteraction) {
    if (!purchaseInteraction)
        return;

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.erase(
        std::remove_if(trackedInteractables.begin(), trackedInteractables.end(),
                       [purchaseInteraction](const std::unique_ptr<TrackedInteractable>& tracked) { return tracked->gameObject == purchaseInteraction; }),
        trackedInteractables.end());
}

void ESPModule::OnBarrelInteractionSpawned(void* barrelInteraction) {
    if (!barrelInteraction)
        return;

    BarrelInteraction* barrel = static_cast<BarrelInteraction*>(barrelInteraction);

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(barrelInteraction);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    std::string displayName = G::gameFunctions->Language_GetString(static_cast<MonoString*>(barrel->displayNameToken));
    // Create interactable tracking info for barrels
    auto trackedInteractable = std::make_unique<TrackedInteractable>();
    trackedInteractable->gameObject = barrelInteraction;
    trackedInteractable->purchaseInteraction = nullptr; // Barrels don't use PurchaseInteraction
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = "";
    if (barrel->displayNameToken) {
        trackedInteractable->nameToken = G::g_monoRuntime->StringToUtf8(static_cast<MonoString*>(barrel->displayNameToken));
    }
    trackedInteractable->category = InteractableCategory::Barrel;
    trackedInteractable->specialType = SpecialInteractableType::None;
    trackedInteractable->consumed = false;
    trackedInteractable->costString = "";

    // Add to tracked interactables
    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(std::move(trackedInteractable));
}

void ESPModule::OnGenericInteractionSpawned(void* genericInteraction) {
    if (!genericInteraction)
        return;

    GenericInteraction* gi = static_cast<GenericInteraction*>(genericInteraction);

    // Use the component pointer as the identifier
    void* gameObject = genericInteraction;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(genericInteraction);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    // Get display name - GenericInteraction uses contextToken instead of displayNameToken
    std::string displayName = "Generic Interaction"; // Fallback only
    if (gi->contextToken) {
        displayName = G::gameFunctions->Language_GetString(static_cast<MonoString*>(gi->contextToken));
    }

    // Skip interactables with empty or whitespace-only names
    if (displayName.empty() || displayName.find_first_not_of(" \t\r\n") == std::string::npos) {
        return;
    }

    // Convert token to string for filtering (language-independent)
    std::string token = "";
    if (gi->contextToken) {
        token = G::g_monoRuntime->StringToUtf8(static_cast<MonoString*>(gi->contextToken));
    }

    // Filter out unwanted interactions using tokens
    if (token == "SURVIVOR_POD_OPEN_PANEL_CONTEXT" || // "Open Panel"
        token == "PORTAL_DIALER_CONTEXT" ||           // "Cycle Compound"
        token == "LUNAR_TELEPORTER_SHIFT") {          // "Shift Destination"
        return;
    }

    InteractableCategory category = InteractableCategory::Special;

    if (token.find("PORTAL_") != std::string::npos && token.find("PORTAL_DIALER") == std::string::npos) {
        category = InteractableCategory::Portal;
    }

    // Create tracking info
    auto trackedInteractable = std::make_unique<TrackedInteractable>();
    trackedInteractable->gameObject = gameObject;
    trackedInteractable->purchaseInteraction = nullptr;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = token;
    trackedInteractable->category = category;
    trackedInteractable->specialType = SpecialInteractableType::None;
    trackedInteractable->consumed = false;
    trackedInteractable->costString = "";

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(std::move(trackedInteractable));
}

void ESPModule::OnGenericPickupControllerSpawned(void* genericPickupController) {
    if (!genericPickupController)
        return;

    GenericPickupController* gpc = static_cast<GenericPickupController*>(genericPickupController);

    // Use the component pointer as the identifier
    void* gameObject = genericPickupController;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(genericPickupController);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    // Don't track pickups with invalid pickup index
    if (gpc->pickupIndex <= 0) {
        LOG_INFO("GenericPickupController spawned with invalid pickup index: %d", gpc->pickupIndex);
        return;
    }

    std::string displayName = GetPickupName(gpc->pickupIndex);

    LOG_INFO("GenericPickupController (%p) spawned: pickupIndex=%d, name='%s', consumed=%d, recycled=%d", genericPickupController, gpc->pickupIndex,
             displayName.c_str(), gpc->consumed, gpc->Recycled);

    // Create tracking info - categorize as item pickup
    auto trackedInteractable = std::make_unique<TrackedInteractable>();
    trackedInteractable->gameObject = gameObject;
    trackedInteractable->purchaseInteraction = nullptr;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = ""; // Item pickups don't have name tokens
    trackedInteractable->category = InteractableCategory::ItemPickup;
    trackedInteractable->specialType = SpecialInteractableType::None;
    trackedInteractable->consumed = false;
    trackedInteractable->costString = "";
    trackedInteractable->pickupIndex = gpc->pickupIndex;

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(std::move(trackedInteractable));
}

void ESPModule::OnTimedChestControllerSpawned(void* timedChestController) {
    if (!timedChestController)
        return;

    void* gameObject = timedChestController;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(timedChestController);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    std::string displayName = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("TIMEDCHEST_NAME"));
    void* purchaseInteraction = nullptr;

    // Create tracking info
    auto trackedInteractable = std::make_unique<TrackedInteractable>();
    trackedInteractable->gameObject = gameObject;
    trackedInteractable->purchaseInteraction = purchaseInteraction;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = "TIMEDCHEST_NAME";
    trackedInteractable->category = InteractableCategory::Chest;
    trackedInteractable->specialType = SpecialInteractableType::TimedChest;
    trackedInteractable->consumed = false;
    trackedInteractable->costString = "";

    {
        std::lock_guard<std::mutex> lock(interactablesMutex);
        trackedInteractables.push_back(std::move(trackedInteractable));
    }
}

void ESPModule::OnTimedChestControllerDespawned(void* timedChestController) {
    if (!timedChestController)
        return;

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.erase(
        std::remove_if(trackedInteractables.begin(), trackedInteractables.end(),
                       [timedChestController](const std::unique_ptr<TrackedInteractable>& tracked) { return tracked->gameObject == timedChestController; }),
        trackedInteractables.end());
}

void ESPModule::OnPickupPickerControllerSpawned(void* pickupPickerController) {
    PickupPickerController* pcc = static_cast<PickupPickerController*>(pickupPickerController);
    std::string contextToken = pcc->contextString ? G::g_monoRuntime->StringToUtf8(static_cast<MonoString*>(pcc->contextString)) : "null";

    LOG_INFO("PickupPickerController (%p) Spawned: contextToken='%s'", pcc, contextToken.c_str());

    static const std::unordered_map<std::string, std::string> tokenTransformMap = {{"ARTIFACT_COMMAND_CUBE_INTERACTION_PROMPT", "ARTIFACT_COMMAND_NAME"},
                                                                                   {"AURELIONITE_FRAGMENT_PICKUP_PROMPT", "AURELIONITE_FRAGMENT_PICKUP_NAME"}};

    static const std::unordered_set<std::string> ignoredTokens = {"DELUSION_MEMORYGAME_PROMPT", "SCRAPPER_CONTEXT"};

    if (ignoredTokens.find(contextToken) != ignoredTokens.end()) {
        return;
    }

    std::string nameToken;
    bool inMap = false;
    auto it = tokenTransformMap.find(contextToken);
    if (it != tokenTransformMap.end()) {
        nameToken = it->second;
        inMap = true;
    }

    if (!inMap) {
        nameToken = contextToken;
        LOG_WARNING("PickupPickerController spawned with unknown contextToken: '%s' - Please report this!", contextToken.c_str());
    }

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(pickupPickerController);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    MonoString* nameTokenMono = G::g_monoRuntime->CreateString(nameToken.c_str());
    std::string displayName = G::gameFunctions->Language_GetString(nameTokenMono);

    auto trackedInteractable = std::make_unique<TrackedInteractable>();
    trackedInteractable->gameObject = pickupPickerController;
    trackedInteractable->purchaseInteraction = nullptr;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = contextToken;
    trackedInteractable->category = InteractableCategory::CommandCube;
    trackedInteractable->specialType = SpecialInteractableType::None;
    trackedInteractable->consumed = false;
    trackedInteractable->pickupIndex = -1;
    trackedInteractable->itemName = "";
    trackedInteractable->costString = "";

    trackedInteractables.push_back(std::move(trackedInteractable));
}

void ESPModule::OnScrapperControllerSpawned(void* scrapperController) {
    if (!scrapperController)
        return;

    void* gameObject = scrapperController;

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(scrapperController);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    std::string displayName = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("SCRAPPER_NAME"));

    // Create tracking info
    auto trackedInteractable = std::make_unique<TrackedInteractable>();
    trackedInteractable->gameObject = gameObject;
    trackedInteractable->purchaseInteraction = nullptr;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = "SCRAPPER_NAME";
    trackedInteractable->category = InteractableCategory::Shop;
    trackedInteractable->specialType = SpecialInteractableType::None;
    trackedInteractable->consumed = false;
    trackedInteractable->costString = "";

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(std::move(trackedInteractable));
}

void ESPModule::ClearData() {
    {
        std::lock_guard<std::mutex> lock(interactablesMutex);
        trackedInteractables.clear();
    }

    {
        std::lock_guard<std::mutex> lock(entitiesMutex);
        trackedEnemies.clear();
        trackedPlayers.clear();
    }

    {
        std::lock_guard<std::mutex> lock(teleportersMutex);
        trackedTeleporters.clear();
    }

    auto emptyBuffer = std::make_shared<std::vector<ESPHierarchicalRenderItem>>();
    std::atomic_store(&collectedItemsBuffer, emptyBuffer);
}

void ESPModule::OnStageAdvance(void* stage) {
    ClearData();
    LOG_INFO("ESP data cleared due to stage change");
}

void ESPModule::OnStageDisable() {
    ClearData();
    LOG_INFO("ESP data cleared due to stage disable");
}

void ESPModule::OnRunExit() {
    ClearData();
    LOG_INFO("ESP data cleared due to run exit");
}

std::string ESPModule::GetTimedChestTime(TimedChestController* timedChestController) {
    std::string nameTime = "";
    if (!timedChestController)
        return nameTime;

    // Calculate time remaining
    if (timedChestController->lockTime > 0 && !timedChestController->purchased) {
        float currentTime = G::gameFunctions->GetRunStopwatch();
        float timeRemaining = timedChestController->lockTime - currentTime;

        // Format time as MM:SS (can be negative)
        bool isNegative = timeRemaining < 0;
        float absTime = isNegative ? -timeRemaining : timeRemaining;

        int minutes = static_cast<int>(absTime) / 60;
        int seconds = static_cast<int>(absTime) % 60;

        char timeStr[16];
        if (isNegative) {
            snprintf(timeStr, sizeof(timeStr), " [-%02d:%02d]", minutes, seconds);
        } else {
            snprintf(timeStr, sizeof(timeStr), " [%02d:%02d]", minutes, seconds);
        }
        nameTime += timeStr;
    }

    // Update if purchased
    if (timedChestController->purchased) {
        nameTime += " [OPENED]";
    }

    return nameTime;
}

void ESPModule::RenderInteractableESP(TrackedInteractable* interactable, ImVec2 screenPos, float distance, ChestESPSubControl* control, bool isVisible,
                                      bool onScreen, bool isAvailable) {
    if (!interactable || !control->IsEnabled())
        return;

    // If off-screen, only draw traceline and return
    if (!onScreen) {
        if (control->ShouldShowTraceline()) {
            ImVec2 screenCenter(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y);
            RenderUtils::RenderLine(screenCenter, screenPos, control->GetTracelineColorU32(), 1.0f);
        }
        return;
    }

    float fontSize = ImGui::GetFont()->FontSize;

    if (control->ShouldShowTraceline()) {
        ImVec2 startPos(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y);
        RenderUtils::RenderLine(startPos, screenPos, control->GetTracelineColorU32(), 1.0f);
    }

    float yOffset = 0;
    std::string displayName = interactable->displayName;
    if (interactable->category == InteractableCategory::Special && interactable->specialType == SpecialInteractableType::PressurePlate) {
        PressurePlateController* ppc = static_cast<PressurePlateController*>(interactable->gameObject);
        if (ppc->switchDown) {
            displayName += " (Active)";
        } else {
            displayName += " (Inactive)";
        }
    } else if (interactable->category == InteractableCategory::Chest && interactable->specialType == SpecialInteractableType::TimedChest) {
        TimedChestController* tcc = static_cast<TimedChestController*>(interactable->gameObject);
        displayName += GetTimedChestTime(tcc);
    }

    // Draw interactable name with optional distance
    if (control->ShouldShowName()) {
        char nameText[512];

        if (control->ShouldShowDistance() && !isAvailable) {
            snprintf(nameText, sizeof(nameText), "%s (%dm) (Unavailable)", displayName.c_str(), static_cast<int>(distance));
        } else if (control->ShouldShowDistance()) {
            snprintf(nameText, sizeof(nameText), "%s (%dm)", displayName.c_str(), static_cast<int>(distance));
        } else if (!isAvailable) {
            snprintf(nameText, sizeof(nameText), "%s (Unavailable)", displayName.c_str());
        } else {
            snprintf(nameText, sizeof(nameText), "%s", displayName.c_str());
        }

        ImVec2 textPos = RenderUtils::RenderText(ImVec2(screenPos.x, screenPos.y - yOffset), control->GetNameColorU32(), control->GetNameShadowColorU32(),
                                                 control->IsNameShadowEnabled(),
                                                 true, // Center text
                                                 "%s", nameText);
        yOffset += fontSize + 2;

        // Show item name for chests and shops if available
        if (!interactable->itemName.empty() &&
            (interactable->category == InteractableCategory::Chest || interactable->category == InteractableCategory::Shop)) {
            std::string itemText = "[" + interactable->itemName + "]";
            RenderUtils::RenderText(ImVec2(screenPos.x, screenPos.y - yOffset), IM_COL32(255, 215, 0, 255), // Gold color for items
                                    control->GetNameShadowColorU32(), control->IsNameShadowEnabled(),
                                    true, // Center text
                                    "%s", itemText.c_str());
            yOffset += fontSize + 2;
        }
    }

    // Draw cost/reward info
    if (control->ShouldShowCost()) {
        std::string rewardText;

        // Special handling for barrels
        if (interactable->category == InteractableCategory::Barrel) {
            if (interactable->purchaseInteraction) {
                // Equipment barrels - show cost
                if (!interactable->costString.empty()) {
                    rewardText = interactable->costString;
                }
            } else if (interactable->gameObject) {
                // Regular barrels - show gold and XP rewards
                BarrelInteraction* barrel = static_cast<BarrelInteraction*>(interactable->gameObject);
                if (barrel->goldReward > 0 || barrel->expReward > 0) {
                    rewardText = "$" + std::to_string(barrel->goldReward) + " + " + std::to_string(barrel->expReward) + " XP";
                }
            }
        } else if (interactable->purchaseInteraction) {
            // Use the cached cost string that was localized when the interactable was created
            if (!interactable->costString.empty()) {
                rewardText = interactable->costString;
            }
        }

        if (!rewardText.empty()) {
            ImVec2 textPos = RenderUtils::RenderText(ImVec2(screenPos.x, screenPos.y - yOffset), control->GetCostColorU32(), control->GetCostShadowColorU32(),
                                                     control->IsCostShadowEnabled(),
                                                     true, // Center text
                                                     "%s", rewardText.c_str());
            yOffset += fontSize + 2;
        }
    }
}

void ESPModule::OnChestBehaviorSpawned(void* chestBehavior) {
    // TODO: See if we can use this for anything
    return;
}

void ESPModule::OnShopTerminalBehaviorSpawned(void* shopTerminalBehavior) {
    if (!shopTerminalBehavior)
        return;

    ShopTerminalBehavior* shop = static_cast<ShopTerminalBehavior*>(shopTerminalBehavior);

    Vector3 shopPos = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(shopTerminalBehavior);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &shopPos);
        }
    }

    // Find the corresponding tracked interactable by position
    std::lock_guard<std::mutex> lock(interactablesMutex);
    for (auto& tracked : trackedInteractables) {
        // Check if positions match
        if (tracked->position == shopPos && tracked->category == InteractableCategory::Shop) {
            // Get the pickup index from the shop
            if (shop->pickupIndex != -1) {
                tracked->pickupIndex = shop->pickupIndex;
                // Get pickup name from pickup index using PickupCatalog
                PickupDef* pickupDef = G::gameFunctions->GetPickupDef(shop->pickupIndex);
                if (pickupDef && pickupDef->nameToken) {
                    tracked->itemName = G::gameFunctions->Language_GetString(static_cast<MonoString*>(pickupDef->nameToken));
                } else {
                    tracked->itemName = "Unknown [" + std::to_string(shop->pickupIndex) + "]";
                }
            }
            break;
        }
    }
}

void ESPModule::OnPressurePlateControllerSpawned(void* pressurePlateController) {
    if (!pressurePlateController)
        return;

    PressurePlateController* ppc = static_cast<PressurePlateController*>(pressurePlateController);

    Vector3 position = {0, 0, 0};
    if (Hooks::Component_get_transform && Hooks::Transform_get_position_Injected) {
        void* transform = Hooks::Component_get_transform(pressurePlateController);
        if (transform) {
            Hooks::Transform_get_position_Injected(transform, &position);
        }
    }

    // Note: Pressure plates don't seem to have standard language tokens, they're dynamic
    std::string displayName = "Pressure Plate"; // TODO: Find proper localization

    // Create tracking info - categorize as special interactable
    auto trackedInteractable = std::make_unique<TrackedInteractable>();
    trackedInteractable->gameObject = pressurePlateController;
    trackedInteractable->purchaseInteraction = nullptr;
    trackedInteractable->position = position;
    trackedInteractable->displayName = displayName;
    trackedInteractable->nameToken = "PRESSURE_PLATE_DYNAMIC"; // Mark as dynamic
    trackedInteractable->category = InteractableCategory::Special;
    trackedInteractable->specialType = SpecialInteractableType::PressurePlate;
    trackedInteractable->consumed = false;
    trackedInteractable->pickupIndex = -1;
    trackedInteractable->itemName = "";
    trackedInteractable->costString = "";

    std::lock_guard<std::mutex> lock(interactablesMutex);
    trackedInteractables.push_back(std::move(trackedInteractable));
}

void ESPModule::InitializeCostFormats() {
    if (m_costFormatsInitialized)
        return;

    LOG_INFO("Initializing cost formats...");

    m_moneyFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_MONEY_FORMAT"));
    m_percentHealthFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_PERCENTHEALTH_FORMAT"));
    m_itemFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_ITEM_FORMAT"));
    m_lunarFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_LUNAR_FORMAT"));
    m_equipmentFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_EQUIPMENT_FORMAT"));
    m_volatileBatteryFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_VOLATILEBATTERY_FORMAT"));
    m_artifactKeyFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_ARTIFACTSHELLKILLERITEM_FORMAT"));
    m_rustedKeyFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_TREASURECACHEITEM_FORMAT"));
    m_encrustedKeyFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_TREASURECACHEVOIDITEM_FORMAT"));
    m_lunarCoinName = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("PICKUP_LUNAR_COIN"));
    m_voidCoinName = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("PICKUP_VOID_COIN"));
    m_soulCostFormat = G::gameFunctions->Language_GetString(G::g_monoRuntime->CreateString("COST_SOULCOST_FORMAT"));

    m_costFormatsInitialized = true;
    LOG_INFO("Cost formats initialized successfully");
}

std::string ESPModule::GetPickupName(int32_t pickupIndex) {
    auto it = m_pickupIdToNameCache.find(pickupIndex);
    if (it != m_pickupIdToNameCache.end()) {
        return it->second;
    }

    // Fallback for unknown pickup indices
    return "Item Pickup [" + std::to_string(pickupIndex) + "]";
}

void ESPModule::CachePickupName(int32_t pickupIndex, const std::string& name) { m_pickupIdToNameCache[pickupIndex] = name; }

std::string ESPModule::GetCostString(CostTypeIndex_Value costType, int cost) {
    if (!m_costFormatsInitialized) {
        InitializeCostFormats();
    }

    std::string format;

    switch (costType) {
    case CostTypeIndex_Value::None:
        return std::to_string(cost);

    case CostTypeIndex_Value::Money:
        format = m_moneyFormat;
        break;

    case CostTypeIndex_Value::PercentHealth:
        format = m_percentHealthFormat;
        break;

    case CostTypeIndex_Value::LunarCoin:
        return std::to_string(cost) + " " + m_lunarCoinName;

    case CostTypeIndex_Value::WhiteItem:
    case CostTypeIndex_Value::GreenItem:
    case CostTypeIndex_Value::RedItem:
    case CostTypeIndex_Value::BossItem:
        format = m_itemFormat;
        break;

    case CostTypeIndex_Value::Equipment:
        format = m_equipmentFormat;
        break;

    case CostTypeIndex_Value::VolatileBattery:
        format = m_volatileBatteryFormat;
        break;

    case CostTypeIndex_Value::LunarItemOrEquipment:
        format = m_lunarFormat;
        break;

    case CostTypeIndex_Value::ArtifactShellKillerItem:
        format = m_artifactKeyFormat;
        break;

    case CostTypeIndex_Value::TreasureCacheItem:
        format = m_rustedKeyFormat;
        break;

    case CostTypeIndex_Value::TreasureCacheVoidItem:
        format = m_encrustedKeyFormat;
        break;

    case CostTypeIndex_Value::VoidCoin:
        return std::to_string(cost) + " " + m_voidCoinName;

    case CostTypeIndex_Value::SoulCost:
        format = m_soulCostFormat;
        break;

    default:
        LOG_WARNING("Unknown cost type: %d", static_cast<int>(costType));
        return std::to_string(cost) + " Unknown";
    }

    // Replace {0} with the cost value
    size_t pos = format.find("{0}");
    if (pos != std::string::npos) {
        format.replace(pos, 3, std::to_string(cost));
    }

    return format;
}
