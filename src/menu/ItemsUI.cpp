#include "ItemsUI.hpp"
#include "globals/globals.hpp"
#include <algorithm>
#include <cctype>
#include <imgui.h>
#include <shared_mutex>
#include <string>

char ItemsUI::searchBuffer[256] = "";

void ItemsUI::DrawItemsSection(const std::vector<RoR2Item>& items, std::map<int, std::unique_ptr<IntControl>>& itemControls,
                               const std::vector<int>* itemStacks) {
    if (ImGui::CollapsingHeader("Items")) {
        std::shared_lock<std::shared_mutex> lock(G::itemsMutex);

        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputText("Search Items", searchBuffer, sizeof(searchBuffer));
        ImGui::Separator();

        bool hasSearch = searchBuffer[0] != '\0';

        static const ItemTier_Value tiers[] = {ItemTier_Value::Tier1,     ItemTier_Value::Tier2,     ItemTier_Value::Tier3,
                                               ItemTier_Value::Boss,      ItemTier_Value::Lunar,     ItemTier_Value::VoidTier1,
                                               ItemTier_Value::VoidTier2, ItemTier_Value::VoidTier3, ItemTier_Value::VoidBoss};
        static const char* tierNames[] = {"Common", "Uncommon", "Legendary", "Boss", "Lunar", "Void Common", "Void Uncommon", "Void Legendary", "Void Boss"};
        static const int tierCount = sizeof(tiers) / sizeof(tiers[0]);

        for (int i = 0; i < tierCount; i++) {
            ItemTier_Value tier = tiers[i];
            const char* tierName = tierNames[i];

            // If searching, check if any items in this tier match
            if (hasSearch) {
                bool hasTierItems = false;
                for (const auto& item : items) {
                    if (item.tier == tier && ItemMatchesSearch(item, searchBuffer)) {
                        hasTierItems = true;
                        break;
                    }
                }
                if (!hasTierItems)
                    continue;
            }

            if (ImGui::CollapsingHeader(tierName)) {
                DrawFilteredItems(tier, items, itemControls, itemStacks, searchBuffer);
            }
        }
    }
}

void ItemsUI::DrawFilteredItems(ItemTier_Value tier, const std::vector<RoR2Item>& items, std::map<int, std::unique_ptr<IntControl>>& itemControls,
                                const std::vector<int>* itemStacks, const char* searchText) {
    std::shared_lock<std::shared_mutex> lock(G::itemsMutex);

    for (const auto& item : items) {
        if (item.tier != tier)
            continue;

        if (!ItemMatchesSearch(item, searchText))
            continue;

        int index = item.index;
        if (itemControls.count(index) > 0) {
            // If we have item stacks, sync the value
            if (itemStacks && index < itemStacks->size() && itemControls[index]->GetValue() != (*itemStacks)[index]) {
                itemControls[index]->SetValue((*itemStacks)[index]);
            }
            itemControls[index]->Draw();
        }
    }
}

bool ItemsUI::ItemMatchesSearch(const RoR2Item& item, const char* searchText) {
    if (searchText[0] == '\0')
        return true;

    auto it = std::search(item.displayName.begin(), item.displayName.end(), searchText, searchText + strlen(searchText),
                          [](char a, char b) { return std::tolower(a) == std::tolower(b); });
    return it != item.displayName.end();
}
