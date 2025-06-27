#pragma once

#include "core/MonoRuntime.h"
#include "utils/Math.h"
#include "game/GameStructs.h"

// Nullable<TeamIndex> structure for Mono interop
#pragma pack(push, 1)
struct NullableTeamIndex {
    bool hasValue;
    int8_t value;
};
#pragma pack(pop)

class GameFunctions {
private:
    MonoRuntime* m_runtime;
    MonoClass* m_localUserManagerClass;
    MonoClass* m_localUserClass;
    MonoClass* m_characterMasterClass;
    MonoClass* m_characterBodyClass;
    MonoClass* m_cursorClass;
    MonoClass* m_contentManagerClass;
    MonoClass* m_itemDefClass;
    MonoClass* m_itemTierDefClass;
    MonoClass* m_inventoryClass;
    MonoClass* m_languageClass;
    MonoClass* m_RoR2ApplicationClass;
    MonoClass* m_teleportHelperClass;
    MonoClass* m_pickupCatalogClass;
    MonoClass* m_pickupDefClass;
    MonoClass* m_masterCatalogClass;
    MonoClass* m_bodyCatalogClass;
    MonoClass* m_masterSummonClass;
    MonoClass* m_runClass;
    MonoClass* m_teamManagerClass;
    MonoClass* m_buffCatalogClass;
    MonoClass* m_buffDefClass;

    TeamManager* m_cachedTeamManager;

public:
    GameFunctions(MonoRuntime* runtime);
    ~GameFunctions() = default;

    void Cursor_SetLockState(int lockState);
    void Cursor_SetVisible(bool visible);
    std::string Language_GetString(MonoString* token);
    PickupDef* GetPickupDef(int pickupIndex);
    int LoadPickupNames();
    int LoadItems();
    int LoadEnemies();
    int LoadElites();
    bool ApplyEliteToMaster(void* characterMaster, int eliteIndex);
    void Inventory_GiveItem(void* m_inventory, int itemIndex, int count);
    bool RoR2Application_IsLoading();
    bool RoR2Application_IsLoadFinished();
    bool RoR2Application_IsModded();
    void RoR2Application_SetModded(bool modded);
    int RoR2Application_GetLoadGameContentPercentage();
    void TeleportHelper_TeleportBody(void* m_characterBody, Vector3 position);
    float GetRunStopwatch();
    bool SpawnEnemyAtPosition(int masterIndex, Vector3 position, int teamIndex = 2, bool matchDifficulty = false, int eliteIndex = 0, const std::vector<std::pair<int, int>>& items = {});

    TeamManager* GetTeamManagerInstance();
    void CacheTeamManagerInstance(TeamManager* instance);
    void ClearTeamManagerInstance();
    uint32_t GetTeamLevel(TeamIndex_Value teamIndex);
    void SetTeamLevel(TeamIndex_Value teamIndex, uint32_t level);
};