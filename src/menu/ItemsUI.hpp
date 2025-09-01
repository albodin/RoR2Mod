#pragma once
#include "InputControls.hpp"
#include "game/GameStructs.hpp"
#include "utils/ModStructs.hpp"
#include <map>
#include <memory>
#include <vector>

class ItemsUI {
  private:
    static char searchBuffer[256];

  public:
    static void DrawItemsSection(const std::vector<RoR2Item>& items, std::map<int, std::unique_ptr<IntControl>>& itemControls,
                                 const std::vector<int>* itemStacks = nullptr);

  private:
    static void DrawFilteredItems(ItemTier_Value tier, const std::vector<RoR2Item>& items, std::map<int, std::unique_ptr<IntControl>>& itemControls,
                                  const std::vector<int>* itemStacks, const char* searchText);

    static bool ItemMatchesSearch(const RoR2Item& item, const char* searchText);
};
