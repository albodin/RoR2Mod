#include "GameFunctions.h"
#include "globals/globals.h"

GameFunctions::GameFunctions(MonoRuntime* runtime) {
    m_runtime = runtime;
    m_localUserManagerClass = runtime->GetClass("Assembly-CSharp", "RoR2", "LocalUserManager");
    m_localUserClass = runtime->GetClass("Assembly-CSharp", "RoR2", "LocalUser");
    m_characterMasterClass = runtime->GetClass("Assembly-CSharp", "RoR2", "CharacterMaster");
    m_characterBodyClass = runtime->GetClass("Assembly-CSharp", "RoR2", "CharacterBody");
    m_cursorClass = runtime->GetClass("UnityEngine.CoreModule", "UnityEngine", "Cursor");
    m_contentManagerClass = runtime->GetClass("Assembly-CSharp", "RoR2.ContentManagement", "ContentManager");
    m_itemDefClass = runtime->GetClass("Assembly-CSharp", "RoR2", "ItemDef");
    m_itemTierDefClass = runtime->GetClass("Assembly-CSharp", "RoR2", "ItemTierDef");
    m_inventoryClass = runtime->GetClass("Assembly-CSharp", "RoR2", "Inventory");
    m_languageClass = runtime->GetClass("Assembly-CSharp", "RoR2", "Language");
    m_RoR2ApplicationClass = runtime->GetClass("Assembly-CSharp", "RoR2", "RoR2Application");
    m_teleportHelperClass = runtime->GetClass("Assembly-CSharp", "RoR2", "TeleportHelper");
    m_pickupCatalogClass = runtime->GetClass("Assembly-CSharp", "RoR2", "PickupCatalog");
    m_pickupDefClass = runtime->GetClass("Assembly-CSharp", "RoR2", "PickupDef");
    m_masterCatalogClass = runtime->GetClass("Assembly-CSharp", "RoR2", "MasterCatalog");
    m_bodyCatalogClass = runtime->GetClass("Assembly-CSharp", "RoR2", "BodyCatalog");
    m_masterSummonClass = runtime->GetClass("Assembly-CSharp", "RoR2", "MasterSummon");
    m_runClass = runtime->GetClass("Assembly-CSharp", "RoR2", "Run");
    m_teamManagerClass = runtime->GetClass("Assembly-CSharp", "RoR2", "TeamManager");
    m_buffCatalogClass = runtime->GetClass("Assembly-CSharp", "RoR2", "BuffCatalog");
    m_buffDefClass = runtime->GetClass("Assembly-CSharp", "RoR2", "BuffDef");

    m_cachedTeamManager = nullptr;
}

void GameFunctions::Cursor_SetLockState(int lockState) {
    std::function<void()> task = [this, lockState]() {
        if (!m_cursorClass) return;

        MonoMethod* method = m_runtime->GetMethod(m_cursorClass, "set_lockState", 1);
        if (!method) {
            G::logger.LogError("Failed to find set_lockState method");
            return;
        }

        int localLockState = lockState;
        void* args[1];
        args[0] = &localLockState;

        m_runtime->InvokeMethod(method, nullptr, args);
    };
    std::unique_lock<std::mutex> lock(G::queuedActionsMutex);
    G::queuedActions.push(task);
}

void GameFunctions::Cursor_SetVisible(bool visible) {
    std::function<void()> task = [this, visible]() {
        if (!m_cursorClass) return;

        MonoMethod* method = m_runtime->GetMethod(m_cursorClass, "set_visible", 1);
        if (!method) {
            G::logger.LogError("Failed to find set_visible method");
            return;
        }

        bool localVisible = visible;
        void* args[1];
        args[0] = &localVisible;

        m_runtime->InvokeMethod(method, nullptr, args);
    };
    std::unique_lock<std::mutex> lock(G::queuedActionsMutex);
    G::queuedActions.push(task);
}

std::string GameFunctions::Language_GetString(MonoString* token) {
    if (!m_languageClass) return "";

    MonoMethod* method = m_runtime->GetMethod(m_languageClass, "GetString", 1);
    if (!method) {
        G::logger.LogError("Failed to find GetString method");
        return "";
    }

    void* args[1];
    args[0] = token;

    MonoObject* result = m_runtime->InvokeMethod(method, nullptr, args);
    if (!result) {
        G::logger.LogError("Failed to get string from token");
        return "";
    }

    return m_runtime->StringToUtf8((MonoString*)result);
}

PickupDef* GameFunctions::GetPickupDef(int pickupIndex) {
    if (!m_pickupCatalogClass) return nullptr;

    MonoField* entriesField = m_runtime->GetField(m_pickupCatalogClass, "entries");
    if (!entriesField) {
        G::logger.LogError("Failed to find PickupCatalog.entries field");
        return nullptr;
    }

    MonoArray* entriesArray = m_runtime->GetStaticFieldValue<MonoArray*>(m_pickupCatalogClass, entriesField);
    if (!entriesArray) {
        G::logger.LogError("Failed to get PickupCatalog.entries array");
        return nullptr;
    }

    int arrayLength = m_runtime->GetArrayLength(entriesArray);
    if (pickupIndex < 0 || pickupIndex >= arrayLength) {
        G::logger.LogError("PickupIndex %d out of bounds (array length: %d)", pickupIndex, arrayLength);
        return nullptr;
    }

    MonoClass* arrayClass = m_runtime->GetObjectClass((MonoObject*)entriesArray);
    MonoMethod* getItemMethod = m_runtime->GetMethod(arrayClass, "Get", 1);
    if (!getItemMethod) {
        G::logger.LogError("Failed to get array access method");
        return nullptr;
    }

    void* params[1] = { &pickupIndex };
    MonoObject* pickupDefObj = m_runtime->InvokeMethod(getItemMethod, entriesArray, params);
    if (!pickupDefObj) {
        G::logger.LogError("Failed to get PickupDef at index %d", pickupIndex);
        return nullptr;
    }

    return reinterpret_cast<PickupDef*>(pickupDefObj);
}

int GameFunctions::LoadPickupNames() {
    if (!m_pickupCatalogClass) return -1;

    MonoField* entriesField = m_runtime->GetField(m_pickupCatalogClass, "entries");
    if (!entriesField) {
        G::logger.LogError("Failed to find PickupCatalog.entries field");
        return -1;
    }

    MonoArray* entriesArray = m_runtime->GetStaticFieldValue<MonoArray*>(m_pickupCatalogClass, entriesField);
    if (!entriesArray) {
        G::logger.LogError("Failed to get PickupCatalog.entries array");
        return -1;
    }

    int arrayLength = m_runtime->GetArrayLength(entriesArray);

    for (int i = 0; i < arrayLength; i++) {
        PickupDef* pickupDef = GetPickupDef(i);
        if (pickupDef && pickupDef->nameToken) {
            std::string name = Language_GetString((MonoString*)pickupDef->nameToken);
            if (!name.empty()) {
                G::espModule->CachePickupName(i, name);
            }
        }
    }

    return arrayLength;
}

// Function should only be called from safe threads so queue is not needed
int GameFunctions::LoadItems() {
    if (!m_contentManagerClass || !m_itemDefClass) {
        G::logger.LogError("Failed to load item definitions, classes not found");
        return -1;
    }

    MonoField* itemDefsField = m_runtime->GetField(m_contentManagerClass, "_itemDefs");
    if (!itemDefsField) {
        G::logger.LogError("Failed to find _itemDefs field");
        return -1;
    }

    MonoArray* itemDefsArray = m_runtime->GetStaticFieldValue<MonoArray*>(m_contentManagerClass, itemDefsField);
    if (!itemDefsArray) {
        G::logger.LogError("Failed to get _itemDefs array");
        return -1;
    }

    int itemCount = m_runtime->GetArrayLength(itemDefsArray);
    if (itemCount <= 0) {
        G::logger.LogError("Item count is zero or negative");
        return -1;
    }

    G::logger.LogInfo("Found " + std::to_string(itemCount) + " items");

    MonoField* nameTokenField =  m_runtime->GetField(m_itemDefClass, "nameToken");
    MonoField* pickupTokenField =  m_runtime->GetField(m_itemDefClass, "pickupToken");
    MonoField* descTokenField =  m_runtime->GetField(m_itemDefClass, "descriptionToken");
    MonoField* loreTokenField =  m_runtime->GetField(m_itemDefClass, "loreToken");
    MonoField* itemTierDefField =  m_runtime->GetField(m_itemDefClass, "_itemTierDef");
    MonoField* itemIndexField =  m_runtime->GetField(m_itemDefClass, "_itemIndex");
    MonoField* canRemoveField =  m_runtime->GetField(m_itemDefClass, "canRemove");
    MonoField* isConsumedField =  m_runtime->GetField(m_itemDefClass, "isConsumed");
    MonoField* hiddenField =  m_runtime->GetField(m_itemDefClass, "hidden");
    MonoField* tagsField =  m_runtime->GetField(m_itemDefClass, "tags");

    MonoClass* arrayClass = m_runtime->GetObjectClass((MonoObject*)itemDefsArray);
    MonoMethod* getItemMethod = m_runtime->GetMethod(arrayClass, "Get", 1);
    if (!getItemMethod) {
        G::logger.LogError("Failed to get array access method");
        return -1;
    }

    std::unique_lock<std::shared_mutex> lock(G::itemsMutex);
    G::items.clear();
    for (int i = 0; i < itemCount; i++) {
        void* params[1] = { &i };
        MonoObject* itemDefObj = m_runtime->InvokeMethod(getItemMethod, itemDefsArray, params);
        if (!itemDefObj) continue;

        RoR2Item item;
        item.index = -1;
        if (itemIndexField) {
            item.index = m_runtime->GetFieldValue<int>(itemDefObj, itemIndexField);
        }
        if (item.index < 0) {
            G::logger.LogError("Item index is negative, failing");
            return -1;
        }

        MonoClass* objClass = m_runtime->GetObjectClass(itemDefObj);
        MonoProperty* nameProp = m_runtime->GetProperty(objClass, "name");
        if (nameProp) {
            MonoMethod* getNameMethod = m_runtime->GetPropertyGetMethod(nameProp);
            if (getNameMethod) {
                MonoObject* nameObj = m_runtime->InvokeMethod(getNameMethod, itemDefObj, nullptr);
                if (nameObj) {
                    item.name = m_runtime->StringToUtf8((MonoString*)nameObj);
                }
            }
        }

        if (nameTokenField) {
            MonoString* str = m_runtime->GetFieldValue<MonoString*>(itemDefObj, nameTokenField);
            if (str) {
                item.nameToken = m_runtime->StringToUtf8(str);
                item.displayName = Language_GetString(str);
            }
        }

        if (pickupTokenField) {
            MonoString* str = m_runtime->GetFieldValue<MonoString*>(itemDefObj, pickupTokenField);
            if (str) item.pickupToken = m_runtime->StringToUtf8(str);
        }

        if (descTokenField) {
            MonoString* str = m_runtime->GetFieldValue<MonoString*>(itemDefObj, descTokenField);
            if (str) item.descriptionToken = m_runtime->StringToUtf8(str);
        }

        if (loreTokenField) {
            MonoString* str = m_runtime->GetFieldValue<MonoString*>(itemDefObj, loreTokenField);
            if (str) item.loreToken = m_runtime->StringToUtf8(str);
        }

        if (itemTierDefField) {
            MonoObject* tierDefObj = m_runtime->GetFieldValue<MonoObject*>(itemDefObj, itemTierDefField);

            if (tierDefObj) {
                MonoField* tierField = m_runtime->GetField(m_itemTierDefClass, "_tier");
                if (tierField) {
                    int tierValue = m_runtime->GetFieldValue<int>(tierDefObj, tierField);
                    item.tier = static_cast<ItemTier_Value>(tierValue);
                    MonoField* isDroppableField = m_runtime->GetField(m_itemTierDefClass, "isDroppable");
                    MonoField* canScrapField = m_runtime->GetField(m_itemTierDefClass, "canScrap");
                    MonoField* canRestackField = m_runtime->GetField(m_itemTierDefClass, "canRestack");

                    if (isDroppableField) {
                        item.isDroppable = m_runtime->GetFieldValue<bool>(tierDefObj, isDroppableField);
                    }

                    if (canScrapField) {
                        item.canScrap = m_runtime->GetFieldValue<bool>(tierDefObj, canScrapField);
                    }

                    if (canRestackField) {
                        item.canRestack = m_runtime->GetFieldValue<bool>(tierDefObj, canRestackField);
                    }

                    // Get the tier name
                    MonoClass* objClass = m_runtime->GetObjectClass(tierDefObj);
                    MonoProperty* nameProp = m_runtime->GetProperty(objClass, "name");
                    if (nameProp) {
                        MonoMethod* getNameMethod = m_runtime->GetPropertyGetMethod(nameProp);
                        if (getNameMethod) {
                            MonoObject* nameObj = m_runtime->InvokeMethod(getNameMethod, tierDefObj, nullptr);
                            if (nameObj) {
                                item.tierName = m_runtime->StringToUtf8((MonoString*)nameObj);
                            }
                        }
                    }
                }
            }
        }

        if (canRemoveField) {
            item.canRemove = m_runtime->GetFieldValue<bool>(itemDefObj, canRemoveField);
        }

        if (isConsumedField) {
            item.isConsumed = m_runtime->GetFieldValue<bool>(itemDefObj, isConsumedField);
        }

        if (hiddenField) {
            item.hidden = m_runtime->GetFieldValue<bool>(itemDefObj, hiddenField);
        }

        if (tagsField) {
            MonoArray* tagsArray = m_runtime->GetFieldValue<MonoArray*>(itemDefObj, tagsField);
            if (tagsArray) {
                int tagCount = m_runtime->GetArrayLength(tagsArray);

                MonoClass* tagsArrayClass = m_runtime->GetObjectClass((MonoObject*)tagsArray);
                MonoMethod* tagsGetItemMethod = m_runtime->GetMethod(tagsArrayClass, "Get", 1);
                if (tagsGetItemMethod) {
                    for (int j = 0; j < tagCount; j++) {
                        void* tagParams[1] = { &j };
                        MonoObject* tagObj = m_runtime->InvokeMethod(tagsGetItemMethod, tagsArray, tagParams);
                        if (tagObj) {
                            int tag = *(int*)m_runtime->m_mono_object_unbox(tagObj);
                            item.tags.push_back(tag);
                        }
                    }
                }
            }
        }

        if (item.displayName == item.nameToken && !item.nameToken.empty()) {
            G::logger.LogError("Item displayName '%s' is the same as nameToken '%s', skipping", item.displayName.c_str(), item.nameToken.c_str());
        } else if (item.displayName.empty()) {
            // Store special items with empty display names in separate map
            G::specialItems[item.name] = item.index;
            G::logger.LogInfo("Stored special item '%s' at index %d", item.name.c_str(), item.index);
        } else if (item.displayName.find("(Consumed)") != std::string::npos) {
            G::logger.LogError("Item displayName '%s' contains '(Consumed)', skipping", item.displayName.c_str());
        } else {
            G::items.push_back(item);
        }

    }

    return itemCount;
}

int GameFunctions::LoadEnemies() {
    if (!m_masterCatalogClass) {
        G::logger.LogError("Failed to load enemy definitions, MasterCatalog class not found");
        return -1;
    }

    MonoField* masterPrefabsField = m_runtime->GetField(m_masterCatalogClass, "masterPrefabs");
    if (!masterPrefabsField) {
        G::logger.LogError("Failed to find masterPrefabs field");
        return -1;
    }

    MonoArray* masterPrefabsArray = m_runtime->GetStaticFieldValue<MonoArray*>(m_masterCatalogClass, masterPrefabsField);
    if (!masterPrefabsArray) {
        G::logger.LogError("Failed to get masterPrefabs array");
        return -1;
    }

    int masterCount = m_runtime->GetArrayLength(masterPrefabsArray);
    if (masterCount <= 0) {
        G::logger.LogError("Master count is zero or negative");
        return -1;
    }

    G::logger.LogInfo("Found " + std::to_string(masterCount) + " masters");

    MonoClass* arrayClass = m_runtime->GetObjectClass((MonoObject*)masterPrefabsArray);
    MonoMethod* getItemMethod = m_runtime->GetMethod(arrayClass, "Get", 1);
    if (!getItemMethod) {
        G::logger.LogError("Failed to get array access method");
        return -1;
    }

    std::unique_lock<std::shared_mutex> lock(G::enemiesMutex);
    G::enemies.clear();

    for (int i = 0; i < masterCount; i++) {
        void* params[1] = { &i };
        MonoObject* masterPrefab = m_runtime->InvokeMethod(getItemMethod, masterPrefabsArray, params);
        if (!masterPrefab) continue;

        // Get GameObject name
        MonoClass* gameObjectClass = m_runtime->GetObjectClass(masterPrefab);
        MonoProperty* nameProp = m_runtime->GetProperty(gameObjectClass, "name");
        if (!nameProp) continue;

        MonoMethod* getNameMethod = m_runtime->GetPropertyGetMethod(nameProp);
        if (!getNameMethod) continue;

        MonoObject* nameObj = m_runtime->InvokeMethod(getNameMethod, masterPrefab, nullptr);
        if (!nameObj) continue;

        std::string masterName = m_runtime->StringToUtf8((MonoString*)nameObj);
        if (masterName.empty()) continue;

        RoR2Enemy enemy;
        enemy.masterIndex = i;
        enemy.masterName = masterName;

        // Remove "Master" suffix if present
        if (masterName.size() > 6 && masterName.substr(masterName.size() - 6) == "Master") {
            enemy.displayName = masterName.substr(0, masterName.size() - 6);
        } else {
            enemy.displayName = masterName;
        }

        G::enemies.push_back(enemy);
    }

    G::logger.LogInfo("Loaded " + std::to_string(G::enemies.size()) + " enemies");
    return static_cast<int>(G::enemies.size());
}

int GameFunctions::LoadElites() {
    if (!m_buffCatalogClass) {
        G::logger.LogError("Failed to load elite definitions, BuffCatalog class not found");
        return -1;
    }

    G::eliteNames.clear();
    G::eliteBuffIndices.clear();

    // Add "None" option for no elite
    G::eliteNames.push_back("None");

    // Get the buff definitions array from BuffCatalog
    MonoField* buffDefsField = m_runtime->GetField(m_buffCatalogClass, "buffDefs");
    if (!buffDefsField) {
        G::logger.LogError("Failed to find BuffCatalog.buffDefs field");
        return -1;
    }

    MonoArray* buffDefsArray = m_runtime->GetStaticFieldValue<MonoArray*>(m_buffCatalogClass, buffDefsField);
    if (!buffDefsArray) {
        G::logger.LogError("Failed to get BuffCatalog.buffDefs array");
        return -1;
    }

    int buffCount = m_runtime->GetArrayLength(buffDefsArray);
    G::logger.LogInfo("Found %d buffs in BuffCatalog", buffCount);

    if (!m_buffDefClass) {
        G::logger.LogError("BuffDef class not available");
        return -1;
    }

    MonoField* eliteDefField = m_runtime->GetField(m_buffDefClass, "eliteDef");
    if (!eliteDefField) {
        G::logger.LogError("Failed to find BuffDef.eliteDef field");
        return -1;
    }

    MonoClass* arrayClass = m_runtime->GetObjectClass((MonoObject*)buffDefsArray);
    MonoMethod* getItemMethod = m_runtime->GetMethod(arrayClass, "Get", 1);
    if (!getItemMethod) {
        G::logger.LogError("Failed to get array access method");
        return -1;
    }

    // Iterate through all buffs to find elite ones
    for (int i = 0; i < buffCount; i++) {
        void* params[1] = { &i };
        MonoObject* buffDefObj = m_runtime->InvokeMethod(getItemMethod, buffDefsArray, params);
        if (!buffDefObj) continue;

        // Check if this buff is an elite buff (eliteDef != null)
        MonoObject* eliteDefObj = m_runtime->GetFieldValue<MonoObject*>(buffDefObj, eliteDefField);
        if (!eliteDefObj) continue;

        // Get the elite name from EliteDef
        MonoClass* eliteDefClass = m_runtime->GetObjectClass(eliteDefObj);
        MonoProperty* eliteNameProp = m_runtime->GetProperty(eliteDefClass, "name");
        if (!eliteNameProp) continue;

        MonoMethod* getEliteNameMethod = m_runtime->GetPropertyGetMethod(eliteNameProp);
        if (!getEliteNameMethod) continue;

        MonoObject* eliteNameObj = m_runtime->InvokeMethod(getEliteNameMethod, eliteDefObj, nullptr);
        if (!eliteNameObj) continue;

        std::string eliteName = m_runtime->StringToUtf8((MonoString*)eliteNameObj);
        if (eliteName.empty()) continue;

        G::eliteNames.push_back(eliteName);
        G::eliteBuffIndices[eliteName] = i;

        G::logger.LogInfo("Found elite buff: %s at index %d", eliteName.c_str(), i);
    }

    G::logger.LogInfo("Loaded %zu elite types", G::eliteNames.size() - 1); // -1 for "None"
    return static_cast<int>(G::eliteNames.size() - 1);
}

bool GameFunctions::ApplyEliteToMaster(void* characterMaster, int eliteBuffIndex) {
    if (!characterMaster || eliteBuffIndex <= 0) {
        return false;
    }

    // Get the inventory from the CharacterMaster
    MonoProperty* inventoryProp = m_runtime->GetProperty(m_characterMasterClass, "inventory");
    if (!inventoryProp) {
        G::logger.LogError("Failed to find CharacterMaster.inventory property");
        return false;
    }

    MonoMethod* getInventoryMethod = m_runtime->GetPropertyGetMethod(inventoryProp);
    if (!getInventoryMethod) {
        G::logger.LogError("Failed to find inventory getter method");
        return false;
    }

    MonoObject* inventory = m_runtime->InvokeMethod(getInventoryMethod, characterMaster, nullptr);
    if (!inventory) {
        G::logger.LogError("Failed to get inventory from CharacterMaster");
        return false;
    }

    // Get the BuffDef for the elite using cached BuffCatalog class
    MonoMethod* getBuffDefMethod = m_runtime->GetMethod(m_buffCatalogClass, "GetBuffDef", 1);
    if (!getBuffDefMethod) {
        G::logger.LogError("Failed to find BuffCatalog.GetBuffDef method");
        return false;
    }

    void* buffIndexParams[1] = { &eliteBuffIndex };
    MonoObject* buffDef = m_runtime->InvokeMethod(getBuffDefMethod, nullptr, buffIndexParams);
    if (!buffDef) {
        G::logger.LogError("Failed to get BuffDef for elite index %d", eliteBuffIndex);
        return false;
    }

    // Get the eliteDef from the BuffDef using cached class
    MonoField* eliteDefField = m_runtime->GetField(m_buffDefClass, "eliteDef");
    if (!eliteDefField) {
        G::logger.LogError("Failed to find BuffDef.eliteDef field");
        return false;
    }

    MonoObject* eliteDef = m_runtime->GetFieldValue<MonoObject*>(buffDef, eliteDefField);
    if (!eliteDef) {
        G::logger.LogError("BuffDef at index %d does not have an eliteDef", eliteBuffIndex);
        return false;
    }

    // Get the eliteEquipmentDef from the EliteDef
    MonoClass* eliteDefClass = m_runtime->GetObjectClass(eliteDef);
    MonoField* eliteEquipmentDefField = m_runtime->GetField(eliteDefClass, "eliteEquipmentDef");
    if (!eliteEquipmentDefField) {
        G::logger.LogError("Failed to find EliteDef.eliteEquipmentDef field");
        return false;
    }

    MonoObject* equipmentDef = m_runtime->GetFieldValue<MonoObject*>(eliteDef, eliteEquipmentDefField);
    if (!equipmentDef) {
        G::logger.LogWarning("Elite at index %d has no equipment", eliteBuffIndex);
        return false;
    }

    // Get the equipment index from the EquipmentDef
    MonoClass* equipmentDefClass = m_runtime->GetObjectClass(equipmentDef);
    MonoProperty* equipmentIndexProp = m_runtime->GetProperty(equipmentDefClass, "equipmentIndex");
    if (!equipmentIndexProp) {
        G::logger.LogError("Failed to find EquipmentDef.equipmentIndex property");
        return false;
    }

    MonoMethod* getEquipmentIndexMethod = m_runtime->GetPropertyGetMethod(equipmentIndexProp);
    if (!getEquipmentIndexMethod) {
        G::logger.LogError("Failed to find equipmentIndex getter method");
        return false;
    }

    MonoObject* equipmentIndexObj = m_runtime->InvokeMethod(getEquipmentIndexMethod, equipmentDef, nullptr);
    if (!equipmentIndexObj) {
        G::logger.LogError("Failed to get equipment index");
        return false;
    }

    int equipmentIndex = *(int*)m_runtime->m_mono_object_unbox(equipmentIndexObj);

    // Set the equipment on the inventory
    MonoMethod* setEquipmentIndexMethod = m_runtime->GetMethod(m_inventoryClass, "SetEquipmentIndex", 1);
    if (!setEquipmentIndexMethod) {
        G::logger.LogError("Failed to find Inventory.SetEquipmentIndex method");
        return false;
    }

    void* equipParams[1] = { &equipmentIndex };
    m_runtime->InvokeMethod(setEquipmentIndexMethod, inventory, equipParams);

    G::logger.LogInfo("Applied elite equipment index %d (from buff index %d) to spawned enemy", equipmentIndex, eliteBuffIndex);
    return true;
}

void GameFunctions::Inventory_GiveItem(void* m_inventory, int itemIndex, int count) {
    std::function<void()> task = [this, m_inventory, itemIndex, count]() {
        if (!m_inventoryClass) return;

        MonoMethod* method = m_runtime->GetMethod(m_inventoryClass, "GiveItem", 2);
        if (!method) {
            G::logger.LogError("Failed to find GiveItem method");
            return;
        }

        int localItemIndex = itemIndex;
        int localCount = count;
        void* params[2] = { &localItemIndex, &localCount };
        m_runtime->InvokeMethod(method, m_inventory, params);
    };
    std::unique_lock<std::mutex> lock(G::queuedActionsMutex);
    G::queuedActions.push(task);
}

bool GameFunctions::RoR2Application_IsLoading() {
    if (!m_RoR2ApplicationClass) return false;

    MonoMethod* method = m_runtime->GetMethod(m_RoR2ApplicationClass, "get_isLoading", 0);
    if (!method) {
        G::logger.LogError("Failed to find get_isLoading method");
        return false;
    }

    MonoObject* result = m_runtime->InvokeMethod(method, nullptr, nullptr);
    if (!result) {
        G::logger.LogError("Failed to get loading state");
        return false;
    }

    return *(bool*)m_runtime->m_mono_object_unbox(result);
}

bool GameFunctions::RoR2Application_IsLoadFinished() {
    if (!m_RoR2ApplicationClass) return false;

    MonoMethod* method = m_runtime->GetMethod(m_RoR2ApplicationClass, "get_loadFinished", 0);
    if (!method) {
        G::logger.LogError("Failed to find get_loadFinished method");
        return false;
    }

    MonoObject* result = m_runtime->InvokeMethod(method, nullptr, nullptr);
    if (!result) {
        G::logger.LogError("Failed to get load finished state");
        return false;
    }

    return *(bool*)m_runtime->m_mono_object_unbox(result);
}

bool GameFunctions::RoR2Application_IsModded() {
    if (!m_RoR2ApplicationClass) return false;

    MonoField* field = m_runtime->GetField(m_RoR2ApplicationClass, "isModded");
    if (!field) {
        G::logger.LogError("Failed to find isModded field");
        return false;
    }
    bool isModded = m_runtime->GetStaticFieldValue<bool>(m_RoR2ApplicationClass, field);
    return isModded;
}

void GameFunctions::RoR2Application_SetModded(bool modded) {
    if (!m_RoR2ApplicationClass) return;

    MonoField* field = m_runtime->GetField(m_RoR2ApplicationClass, "isModded");
    if (!field) {
        G::logger.LogError("Failed to find isModded field");
        return;
    }
    m_runtime->SetStaticFieldValue<bool>(m_RoR2ApplicationClass, field, modded);
}

int GameFunctions::RoR2Application_GetLoadGameContentPercentage() {
    if (!m_RoR2ApplicationClass) return 0;

    MonoProperty* instanceProperty = m_runtime->GetProperty(m_RoR2ApplicationClass, "instance");
    if (!instanceProperty) {
        G::logger.LogError("Failed to find instance property in RoR2Application");
        return 0;
    }

    MonoMethod* getInstanceMethod = m_runtime->GetPropertyGetMethod(instanceProperty);
    if (!getInstanceMethod) {
        G::logger.LogError("Failed to get instance getter method");
        return 0;
    }

    MonoObject* instance = m_runtime->InvokeMethod(getInstanceMethod, nullptr, nullptr);
    if (!instance) {
        G::logger.LogError("Failed to get RoR2Application instance");
        return 0;
    }

    MonoField* percentageField = m_runtime->GetField(m_RoR2ApplicationClass, "loadGameContentPercentage");
    if (!percentageField) {
        G::logger.LogError("Failed to find loadGameContentPercentage field");
        return 0;
    }

    return m_runtime->GetFieldValue<int>(instance, percentageField);
}

void GameFunctions::TeleportHelper_TeleportBody(void* m_characterBody, Vector3 position) {
    std::function<void()> task = [this, m_characterBody, position]() {
        if (!m_teleportHelperClass) return;

        MonoMethod* method = m_runtime->GetMethod(m_teleportHelperClass, "TeleportBody", 3);
        if (!method) {
            G::logger.LogError("Failed to find TeleportBody method");
            return;
        }

        Vector3 localPosition = position;
        bool forceOutOfVehicle = false;
        void* params[3] = { m_characterBody, &localPosition, &forceOutOfVehicle };
        m_runtime->InvokeMethod(method, nullptr, params);
    };
    std::unique_lock<std::mutex> lock(G::queuedActionsMutex);
    G::queuedActions.push(task);
}

float GameFunctions::GetRunStopwatch() {
    if (!G::runInstance) {
        return 0.0f;
    }

    Run* run = (Run*)G::runInstance;

    // Calculate run stopwatch: if paused, return offset; otherwise return fixedTime + offset
    if (run->runStopwatch.isPaused) {
        return run->runStopwatch.offsetFromFixedTime;
    } else {
        return run->fixedTime + run->runStopwatch.offsetFromFixedTime;
    }
}

bool GameFunctions::SpawnEnemyAtPosition(int masterIndex, Vector3 position, int teamIndex, bool matchDifficulty, int eliteIndex, const std::vector<std::pair<int, int>>& items) {
    std::function<bool()> task = [this, masterIndex, position, teamIndex, matchDifficulty, eliteIndex, items]() -> bool {
        if (!m_masterSummonClass || !m_masterCatalogClass) {
            return false;
        }

        MonoField* masterPrefabsField = m_runtime->GetField(m_masterCatalogClass, "masterPrefabs");
        if (!masterPrefabsField) return false;

        MonoArray* masterPrefabsArray = m_runtime->GetStaticFieldValue<MonoArray*>(m_masterCatalogClass, masterPrefabsField);
        if (!masterPrefabsArray) return false;

        int masterCount = m_runtime->GetArrayLength(masterPrefabsArray);
        if (masterIndex < 0 || masterIndex >= masterCount) return false;

        MonoClass* arrayClass = m_runtime->GetObjectClass((MonoObject*)masterPrefabsArray);
        MonoMethod* getItemMethod = m_runtime->GetMethod(arrayClass, "Get", 1);
        if (!getItemMethod) return false;

        int localMasterIndex = masterIndex;
        void* params[1] = { &localMasterIndex };
        MonoObject* masterPrefab = m_runtime->InvokeMethod(getItemMethod, masterPrefabsArray, params);
        if (!masterPrefab) return false;

        MonoObject* masterSummon = m_runtime->CreateObject(m_masterSummonClass);
        if (!masterSummon) return false;

        // Set masterPrefab
        MonoField* masterPrefabField = m_runtime->GetField(m_masterSummonClass, "masterPrefab");
        if (masterPrefabField) {
            m_runtime->SetFieldValue(masterSummon, masterPrefabField, masterPrefab);
        }

        // Set position
        MonoField* positionField = m_runtime->GetField(m_masterSummonClass, "position");
        if (positionField) {
            Vector3 localPosition = position;
            m_runtime->SetFieldValue(masterSummon, positionField, &localPosition);
        }

        // Set rotation (identity quaternion)
        MonoField* rotationField = m_runtime->GetField(m_masterSummonClass, "rotation");
        if (rotationField) {
            struct { float x, y, z, w; } rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
            m_runtime->SetFieldValue(masterSummon, rotationField, &rotation);
        }

        // Set teamIndexOverride (nullable enum)
        MonoField* teamIndexField = m_runtime->GetField(m_masterSummonClass, "teamIndexOverride");
        if (teamIndexField) {
            NullableTeamIndex nullableTeamIndex;
            nullableTeamIndex.hasValue = true;
            nullableTeamIndex.value = static_cast<int8_t>(teamIndex);
            m_runtime->SetFieldValue(masterSummon, teamIndexField, &nullableTeamIndex);
        }

        // Set ignoreTeamMemberLimit
        MonoField* ignoreTeamLimitField = m_runtime->GetField(m_masterSummonClass, "ignoreTeamMemberLimit");
        if (ignoreTeamLimitField) {
            bool ignoreLimit = true;
            m_runtime->SetFieldValue(masterSummon, ignoreTeamLimitField, &ignoreLimit);
        }

        // Set summonerBodyObject to null
        MonoField* summonerBodyField = m_runtime->GetField(m_masterSummonClass, "summonerBodyObject");
        if (summonerBodyField) {
            void* nullPtr = nullptr;
            m_runtime->SetFieldValue(masterSummon, summonerBodyField, &nullPtr);
        }

        // Perform the spawn
        MonoMethod* performMethod = m_runtime->GetMethod(m_masterSummonClass, "Perform", 0);
        if (!performMethod) return false;

        MonoObject* spawnedMaster = m_runtime->InvokeMethod(performMethod, masterSummon, nullptr);
        if (!spawnedMaster) return false;

        // Give items if difficulty matching is enabled or custom items are requested
        if ((matchDifficulty || !items.empty()) && m_characterMasterClass && m_inventoryClass) {
            // Get the inventory from the spawned CharacterMaster
            MonoProperty* inventoryProp = m_runtime->GetProperty(m_characterMasterClass, "inventory");
            if (inventoryProp) {
                MonoMethod* getInventoryMethod = m_runtime->GetPropertyGetMethod(inventoryProp);
                if (getInventoryMethod) {
                    MonoObject* inventory = m_runtime->InvokeMethod(getInventoryMethod, spawnedMaster, nullptr);
                    if (inventory) {
                        // Give UseAmbientLevel item if difficulty matching is enabled
                        if (matchDifficulty) {
                            static int useAmbientLevelIndex = -1;

                            // Find UseAmbientLevel item index if not already cached
                            if (useAmbientLevelIndex == -1) {
                                std::shared_lock<std::shared_mutex> lock(G::itemsMutex);
                                auto it = G::specialItems.find("UseAmbientLevel");
                                if (it != G::specialItems.end()) {
                                    useAmbientLevelIndex = it->second;
                                    G::logger.LogInfo("Found UseAmbientLevel item at index %d", useAmbientLevelIndex);
                                }
                            }

                            if (useAmbientLevelIndex >= 0) {
                                Inventory_GiveItem(inventory, useAmbientLevelIndex, 1);
                            } else {
                                G::logger.LogError("Could not find UseAmbientLevel item in items list");
                            }
                        }

                        // Give custom items
                        for (const auto& [itemIndex, count] : items) {
                            if (count > 0) {
                                Inventory_GiveItem(inventory, itemIndex, count);
                                G::logger.LogInfo("Gave %d of item %d to spawned enemy", count, itemIndex);
                            }
                        }
                    } else {
                        G::logger.LogWarning("Could not get inventory from spawned master");
                    }
                } else {
                    G::logger.LogError("Failed to find inventory getter method");
                }
            } else {
                G::logger.LogError("Failed to find inventory property on CharacterMaster");
            }
        }

        // Apply elite status if requested
        if (eliteIndex > 0) {
            ApplyEliteToMaster(spawnedMaster, eliteIndex);
        }

        return true;
    };

    std::unique_lock<std::mutex> lock(G::queuedActionsMutex);
    G::queuedActions.push([task]() { task(); });
    return true;
}

TeamManager* GameFunctions::GetTeamManagerInstance() {
    return m_cachedTeamManager;
}

void GameFunctions::CacheTeamManagerInstance(TeamManager* instance) {
    m_cachedTeamManager = instance;
}

void GameFunctions::ClearTeamManagerInstance() {
    m_cachedTeamManager = nullptr;
}

uint32_t GameFunctions::GetTeamLevel(TeamIndex_Value teamIndex) {
    TeamManager* teamManager = GetTeamManagerInstance();
    if (!teamManager || !teamManager->teamLevels) {
        return 0;
    }

    int index = static_cast<int>(teamIndex);
    if (index < 0 || index >= static_cast<int>(TeamIndex_Value::Count)) {
        return 0;
    }

    return teamManager->teamLevels[index];
}

void GameFunctions::SetTeamLevel(TeamIndex_Value teamIndex, uint32_t level) {
    std::function<void()> task = [this, teamIndex, level]() {
        TeamManager* teamManager = GetTeamManagerInstance();
        if (!teamManager) {
            G::logger.LogWarning("SetTeamLevel: TeamManager instance not available");
            return;
        }

        MonoMethod* method = m_runtime->GetMethod(m_teamManagerClass, "SetTeamLevel", 2);
        if (!method) {
            G::logger.LogError("SetTeamLevel: Failed to find SetTeamLevel method");
            return;
        }

        TeamIndex_Value localTeamIndex = teamIndex;
        uint32_t localLevel = level;
        void* params[2] = { &localTeamIndex, &localLevel };
        m_runtime->InvokeMethod(method, teamManager, params);

        G::logger.LogInfo("SetTeamLevel: Called C# method for team %d level %u", (int)teamIndex, level);
    };
    std::unique_lock<std::mutex> lock(G::queuedActionsMutex);
    G::queuedActions.push(task);
}