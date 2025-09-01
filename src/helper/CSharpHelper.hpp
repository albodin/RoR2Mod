#pragma once
#include "core/MonoTypes.hpp"
#include <memory>
#include <string>

class MonoRuntime;

class CSharpHelper {
  public:
    explicit CSharpHelper(MonoRuntime* runtime);
    ~CSharpHelper();

    CSharpHelper(const CSharpHelper&) = delete;
    CSharpHelper& operator=(const CSharpHelper&) = delete;

    bool Initialize();
    bool IsLoaded() const { return m_isLoaded; }

    void SpawnInteractable(const std::string& resourcePath, float x, float y, float z);

  private:
    MonoRuntime* m_runtime;

    MonoAssembly* m_helperAssembly;
    MonoImage* m_helperImage;

    MonoClass* m_spawnHelperClass;

    MonoMethod* m_spawnInteractableMethod;

    bool m_isLoaded;
    std::string m_assemblyName;

    bool LoadAssembly();
    bool ResolveClasses();
    bool ResolveMethods();
};
