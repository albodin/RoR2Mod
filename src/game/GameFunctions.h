#pragma once

#include "core/MonoRuntime.h"
#include "utils/Math.h"

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

public:
    GameFunctions(MonoRuntime* runtime);
    ~GameFunctions() = default;

    void CharacterMaster_UpdateBodyGodMode(void* m_characterMaster);
    void Cursor_SetLockState(int lockState);
    void Cursor_SetVisible(bool visible);
    std::string Language_GetString(MonoString* token);
    int LoadItems();
    void Inventory_GiveItem(void* m_inventory, int itemIndex, int count);
    bool RoR2Application_IsLoading();
    bool RoR2Application_IsLoadFinished();
    bool RoR2Application_IsModded();
    void RoR2Application_SetModded(bool modded);
    int RoR2Application_GetLoadGameContentPercentage();
    void TeleportHelper_TeleportBody(void* m_characterBody, Vector3 position);
};