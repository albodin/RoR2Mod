#include "MonoList.h"
#include "globals/globals.h"

MonoList::MonoList(MonoObject* listObject)
    : list(listObject), getItemMethod(nullptr), addMethod(nullptr), removeMethod(nullptr), clearMethod(nullptr), getCountMethod(nullptr), count(0) {
    if (!list)
        return;

    MonoClass* listClass = G::g_monoRuntime->GetObjectClass(list);
    if (!listClass)
        return;

    MonoProperty* countProp = G::g_monoRuntime->GetProperty(listClass, "Count");
    if (countProp) {
        getCountMethod = G::g_monoRuntime->GetPropertyGetMethod(countProp);
        UpdateCount();
    }

    getItemMethod = G::g_monoRuntime->GetMethod(listClass, "get_Item", 1);
    addMethod = G::g_monoRuntime->GetMethod(listClass, "Add", 1);
    removeMethod = G::g_monoRuntime->GetMethod(listClass, "Remove", 1);
    clearMethod = G::g_monoRuntime->GetMethod(listClass, "Clear", 0);
}

void MonoList::UpdateCount() {
    if (!list || !getCountMethod) {
        count = 0;
        return;
    }

    MonoObject* countObj = G::g_monoRuntime->InvokeMethod(getCountMethod, list, nullptr);
    if (countObj) {
        count = *static_cast<int*>(G::g_monoRuntime->m_mono_object_unbox(countObj));
    }
}

int MonoList::Count() const { return count; }

bool MonoList::IsValid() const { return list != nullptr && getItemMethod != nullptr; }

std::string MonoList::GetItem(int index) {
    if (!list || !getItemMethod || index < 0 || index >= count) {
        return "";
    }

    void* args[1] = {&index};
    MonoObject* stringObj = G::g_monoRuntime->InvokeMethod(getItemMethod, list, args);
    if (!stringObj) {
        return "";
    }

    return G::g_monoRuntime->StringToUtf8(static_cast<MonoString*>(stringObj));
}

bool MonoList::AddItem(const std::string& item) {
    if (!list || !addMethod) {
        return false;
    }

    MonoString* monoString = G::g_monoRuntime->CreateString(item.c_str());
    if (!monoString) {
        return false;
    }

    void* args[1] = {monoString};
    G::g_monoRuntime->InvokeMethod(addMethod, list, args);
    UpdateCount();
    return true;
}

bool MonoList::RemoveItem(const std::string& item) {
    if (!list || !removeMethod) {
        return false;
    }

    MonoString* monoString = G::g_monoRuntime->CreateString(item.c_str());
    if (!monoString) {
        return false;
    }

    void* args[1] = {monoString};
    MonoObject* result = G::g_monoRuntime->InvokeMethod(removeMethod, list, args);
    if (result) {
        bool removed = *static_cast<bool*>(G::g_monoRuntime->m_mono_object_unbox(result));
        if (removed) {
            UpdateCount();
        }
        return removed;
    }
    return false;
}

bool MonoList::Contains(const std::string& item) {
    if (!list) {
        return false;
    }

    MonoClass* listClass = G::g_monoRuntime->GetObjectClass(list);
    if (!listClass)
        return false;

    MonoMethod* containsMethod = G::g_monoRuntime->GetMethod(listClass, "Contains", 1);
    if (!containsMethod)
        return false;

    MonoString* monoString = G::g_monoRuntime->CreateString(item.c_str());
    if (!monoString) {
        return false;
    }

    void* args[1] = {monoString};
    MonoObject* result = G::g_monoRuntime->InvokeMethod(containsMethod, list, args);
    if (result) {
        return *static_cast<bool*>(G::g_monoRuntime->m_mono_object_unbox(result));
    }
    return false;
}

void MonoList::Clear() {
    if (!list || !clearMethod) {
        return;
    }

    G::g_monoRuntime->InvokeMethod(clearMethod, list, nullptr);
    count = 0;
}

std::vector<std::string> MonoList::GetAllItems() {
    std::vector<std::string> result;
    if (!IsValid())
        return result;

    result.reserve(count);
    for (int i = 0; i < count; i++) {
        result.push_back(GetItem(i));
    }
    return result;
}

bool MonoList::ReplaceAllItems(const std::vector<std::string>& newItems) {
    if (!list || !clearMethod || !addMethod) {
        return false;
    }

    Clear();
    for (const auto& item : newItems) {
        AddItem(item);
    }
    return true;
}
