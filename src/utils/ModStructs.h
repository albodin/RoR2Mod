#pragma once
#include <string>
#include <vector>
#include "game/GameStructs.h"

struct RoR2Item {
    int index;
    std::string displayName;
    std::string name;
    std::string nameToken;
    std::string pickupToken;
    std::string descriptionToken;
    std::string loreToken;
    ItemTier_Value tier;
    std::string tierName;
    bool isDroppable;
    bool canScrap;
    bool canRestack;
    bool canRemove;
    bool isConsumed;
    bool hidden;
    std::vector<int> tags;
};

struct RoR2Enemy {
    int masterIndex;
    std::string masterName;
    std::string displayName;
};