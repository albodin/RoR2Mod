#pragma once
#include "core/MonoRuntime.hpp"
#include <string>
#include <vector>

class MonoList {
  private:
    MonoObject* list;
    MonoMethod* getItemMethod;
    MonoMethod* addMethod;
    MonoMethod* removeMethod;
    MonoMethod* clearMethod;
    MonoMethod* getCountMethod;
    int count;

  public:
    MonoList(MonoObject* listObject);

    void UpdateCount();
    int Count() const;
    bool IsValid() const;
    std::string GetItem(int index);
    bool AddItem(const std::string& item);
    bool RemoveItem(const std::string& item);
    bool Contains(const std::string& item);
    void Clear();
    std::vector<std::string> GetAllItems();
    bool ReplaceAllItems(const std::vector<std::string>& newItems);
};
