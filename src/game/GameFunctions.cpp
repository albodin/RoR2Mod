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
                    item.tier = static_cast<ItemTier>(tierValue);
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