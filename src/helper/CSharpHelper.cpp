#include "CSharpHelper.h"
#include "core/MonoRuntime.h"
#include "generated/RoR2ModHelper_dll.h"
#include "globals/globals.h"
#include <chrono>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <vector>

CSharpHelper::CSharpHelper(MonoRuntime* runtime)
    : m_runtime(runtime), m_helperAssembly(nullptr), m_helperImage(nullptr), m_spawnHelperClass(nullptr), m_spawnInteractableMethod(nullptr),
      m_isLoaded(false) {

#ifdef _DEBUG
    m_assemblyName = "RoR2ModHelper";
#else
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() % 1000000;

    std::stringstream ss;
    ss << "RoR2Mod" << std::setfill('0') << std::setw(6) << timestamp;
    m_assemblyName = ss.str();

    if (m_assemblyName.length() != 13) {
        m_assemblyName.resize(13, '0');
    }
#endif
}

CSharpHelper::~CSharpHelper() {
    if (!m_isLoaded) {
        return;
    }

    if (m_spawnHelperClass && m_runtime) {
        G::logger.LogInfo("CSharpHelper: Calling C# cleanup method");
        G::logger.LogInfo("CSharpHelper: Before cleanup - Assembly=%p, Image=%p, Class=%p, Method=%p", m_helperAssembly, m_helperImage, m_spawnHelperClass,
                          m_spawnInteractableMethod);

        MonoMethod* cleanupMethod = m_runtime->GetMethod(m_spawnHelperClass, "Cleanup", 0);
        if (cleanupMethod) {
            MonoObject* result = m_runtime->InvokeMethod(cleanupMethod, nullptr, nullptr);
            if (result) {
                bool success = *(bool*)m_runtime->m_mono_object_unbox(result);
                if (success) {
                    G::logger.LogInfo("CSharpHelper: C# cleanup completed successfully");
                } else {
                    G::logger.LogWarning("CSharpHelper: C# cleanup method returned false (cleanup failed)");
                }
            } else {
                G::logger.LogError("CSharpHelper: C# cleanup method returned null (invocation failed)");
            }
        } else {
            G::logger.LogError("CSharpHelper: Failed to find Cleanup method in SpawnHelper class");
        }
    }

    m_spawnInteractableMethod = nullptr;
    m_spawnHelperClass = nullptr;

#ifdef _DEBUG
    G::logger.LogInfo("CSharpHelper: Debug mode - skipping assembly unload");
    m_helperAssembly = nullptr;
#else
    if (m_helperAssembly && m_runtime) {
        G::logger.LogInfo("CSharpHelper: Unloading assembly '%s' from Mono runtime", m_assemblyName.c_str());
        m_runtime->UnloadAssembly(m_helperAssembly);
        m_helperAssembly = nullptr;
    }
#endif

    m_helperImage = nullptr;
    m_helperAssembly = nullptr;

    m_isLoaded = false;
    G::logger.LogInfo("CSharpHelper: After cleanup - Assembly=%p, Image=%p, Class=%p, Method=%p", m_helperAssembly, m_helperImage, m_spawnHelperClass,
                      m_spawnInteractableMethod);
}

bool CSharpHelper::Initialize() {
    if (m_isLoaded) {
        return true;
    }

    if (!LoadAssembly()) {
        G::logger.LogError("CSharpHelper: Failed to load assembly");
        return false;
    }

    if (!ResolveClasses()) {
        G::logger.LogError("CSharpHelper: Failed to resolve classes");
        return false;
    }

    if (!ResolveMethods()) {
        G::logger.LogError("CSharpHelper: Failed to resolve methods");
        return false;
    }

    m_isLoaded = true;

    G::logger.LogInfo("CSharpHelper: Assembly=%p, Image=%p, Class=%p, Method=%p", m_helperAssembly, m_helperImage, m_spawnHelperClass,
                      m_spawnInteractableMethod);

    return true;
}

bool CSharpHelper::LoadAssembly() {
    if (!m_runtime || !m_runtime->AttachThread()) {
        return false;
    }

#ifdef _DEBUG
    G::logger.LogInfo("CSharpHelper: Debug mode - loading assembly without patching");
    m_helperAssembly = m_runtime->LoadAssemblyFromMemory(RoR2ModHelper_dll_bytes, RoR2ModHelper_dll_size, m_assemblyName.c_str());
#else
    std::vector<unsigned char> patchedData(RoR2ModHelper_dll_bytes, RoR2ModHelper_dll_bytes + RoR2ModHelper_dll_size);

    G::logger.LogInfo("CSharpHelper: Patching assembly name to '%s'", m_assemblyName.c_str());

    for (size_t i = 0; i < RoR2ModHelper_string_offset_count; i++) {
        size_t offset = RoR2ModHelper_string_offsets[i];
        if (offset + RoR2ModHelper_string_length <= patchedData.size()) {
            memcpy(&patchedData[offset], m_assemblyName.c_str(), RoR2ModHelper_string_length);
            G::logger.LogInfo("CSharpHelper: Patched offset %zu with '%s'", offset, m_assemblyName.c_str());
        }
    }

    m_helperAssembly = m_runtime->LoadAssemblyFromMemory(patchedData.data(), patchedData.size(), m_assemblyName.c_str());
#endif

    if (!m_helperAssembly) {
        return false;
    }

    m_helperImage = m_runtime->GetAssemblyImage(m_helperAssembly);
    return m_helperImage != nullptr;
}

bool CSharpHelper::ResolveClasses() {
    if (!m_helperImage) {
        return false;
    }

    m_spawnHelperClass = m_runtime->GetClass(m_assemblyName.c_str(), m_assemblyName.c_str(), "SpawnHelper");

    return m_spawnHelperClass != nullptr;
}

bool CSharpHelper::ResolveMethods() {
    if (!m_spawnHelperClass) {
        return false;
    }

    m_spawnInteractableMethod = m_runtime->GetMethod(m_spawnHelperClass, "SpawnInteractable", 4);

    return m_spawnInteractableMethod != nullptr;
}

void CSharpHelper::SpawnInteractable(const std::string& resourcePath, float x, float y, float z) {
    if (!m_isLoaded || !m_spawnInteractableMethod) {
        G::logger.LogError("CSharpHelper: Not initialized");
        return;
    }

    auto task = [this, resourcePath, x, y, z]() {
        if (!m_runtime->AttachThread()) {
            G::logger.LogError("CSharpHelper: Failed to attach thread");
            return;
        }

        MonoString* pathString = m_runtime->CreateString(resourcePath.c_str());
        if (!pathString) {
            G::logger.LogError("CSharpHelper: Failed to create path string");
            return;
        }

        float xVal = x, yVal = y, zVal = z;
        void* args[4];
        args[0] = pathString;
        args[1] = (void*)&xVal;
        args[2] = (void*)&yVal;
        args[3] = (void*)&zVal;

        m_runtime->InvokeMethod(m_spawnInteractableMethod, nullptr, args);
    };

    std::unique_lock<std::mutex> lock(G::queuedActionsMutex);
    G::queuedActions.push(task);
}
