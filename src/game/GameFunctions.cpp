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
}


void GameFunctions::CharacterMaster_UpdateBodyGodMode(void* m_characterMaster) {
    std::function<void()> task = [this, m_characterMaster]() {
        if (!m_characterMasterClass) return;

        MonoMethod* method = m_runtime->GetMethod(m_characterMasterClass, "UpdateBodyGodMode", 0);
        if (!method) {
            G::logger.LogError("Failed to find UpdateBodyGodMode method");
            return;
        }

        m_runtime->InvokeMethod(method, m_characterMaster, nullptr);
    };
    std::unique_lock<std::mutex> lock(G::queuedActionsMutex);
    G::queuedActions.push(task);
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
            G::logger.LogError("Item displayName is empty, name: '%s', skipping", item.name.c_str());
        } else if (item.displayName.find("(Consumed)") != std::string::npos) {
            G::logger.LogError("Item displayName '%s' contains '(Consumed)', skipping", item.displayName.c_str());
        } else {
            G::items.push_back(item);
        }

    }

    return itemCount;
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