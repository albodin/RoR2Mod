#include "MonoRuntime.h"
#include "globals/globals.h"
#include <chrono>
#include <stdio.h>
#include <vector>

MonoRuntime::MonoRuntime() : m_rootDomain(nullptr), m_thread(nullptr) {}

MonoRuntime::~MonoRuntime() { DetachThread(); }

std::string MonoRuntime::MakeClassCacheKey(const char* nameSpace, const char* className) { return std::string(nameSpace) + "::" + className; }

void __cdecl MonoRuntime::AssemblyIterationCallback(MonoAssembly* assembly, void* user_data) {
    auto* self = static_cast<MonoRuntime*>(user_data);

    if (!assembly || !self) {
        G::logger.LogWarning("AssemblyIterationCallback: Skipping invalid - "
                             "assembly=%p, user_data=%p",
                             assembly, user_data);
        return;
    }

    MonoImage* image = self->m_mono_assembly_get_image(assembly);
    if (!image) {
        G::logger.LogWarning("AssemblyIterationCallback: Skipping assembly=%p with "
                             "no image (returned null)",
                             assembly);
        return;
    }

    const char* imageName = self->m_mono_image_get_name(image);
    if (!imageName) {
        G::logger.LogWarning("AssemblyIterationCallback: Skipping assembly=%p, "
                             "image=%p with no name",
                             assembly, image);
        return;
    }

    std::string imageNameStr(imageName);
    if (imageNameStr.find("RoR2Mod") != std::string::npos) {
        G::logger.LogInfo("AssemblyIterationCallback: Skipping our helper "
                          "assembly: %s (assembly=%p, image=%p)",
                          imageName, assembly, image);
        return;
    }

    self->m_imageCache[imageName] = image;

    G::logger.LogInfo("Found assembly: %s (assembly=%p, image=%p)", imageName, assembly, image);
}

bool MonoRuntime::Initialize(const char* monoDllName) {
    // Get mono module
    HMODULE monoModule = GetModuleHandleA(monoDllName);
    if (!monoModule) {
        G::logger.LogError("Failed to get %s handle", monoDllName);
        return false;
    }

    G::logger.LogInfo("%s base address: 0x%p", monoDllName, monoModule);

// Get function addresses
#define GET_MONO_FUNC(name)                                                                                                                                    \
    m_##name = reinterpret_cast<name##_t>(GetProcAddress(monoModule, #name));                                                                                  \
    if (!m_##name) {                                                                                                                                           \
        G::logger.LogError("Failed to get %s function", #name);                                                                                                \
        return false;                                                                                                                                          \
    }

    GET_MONO_FUNC(mono_get_root_domain);
    GET_MONO_FUNC(mono_domain_assembly_foreach);
    GET_MONO_FUNC(mono_assembly_get_image);
    GET_MONO_FUNC(mono_image_get_name);
    GET_MONO_FUNC(mono_class_from_name);
    GET_MONO_FUNC(mono_class_get_method_from_name);
    GET_MONO_FUNC(mono_runtime_invoke);
    GET_MONO_FUNC(mono_class_get_field_from_name);
    GET_MONO_FUNC(mono_field_get_value);
    GET_MONO_FUNC(mono_field_static_get_value);
    GET_MONO_FUNC(mono_field_static_set_value);
    GET_MONO_FUNC(mono_string_new);
    GET_MONO_FUNC(mono_string_to_utf8);
    GET_MONO_FUNC(mono_thread_attach);
    GET_MONO_FUNC(mono_class_vtable);
    GET_MONO_FUNC(mono_object_get_class);
    GET_MONO_FUNC(mono_compile_method);
    GET_MONO_FUNC(mono_class_get_methods);
    GET_MONO_FUNC(mono_method_get_name);
    GET_MONO_FUNC(mono_method_signature);
    GET_MONO_FUNC(mono_signature_get_param_count);
    GET_MONO_FUNC(mono_signature_get_return_type);
    GET_MONO_FUNC(mono_type_get_name);
    GET_MONO_FUNC(mono_signature_get_params);
    GET_MONO_FUNC(mono_object_unbox);
    GET_MONO_FUNC(mono_field_get_offset);
    GET_MONO_FUNC(mono_class_get_fields);
    GET_MONO_FUNC(mono_field_get_name);
    GET_MONO_FUNC(mono_array_length);
    GET_MONO_FUNC(mono_lookup_internal_call);
    GET_MONO_FUNC(mono_class_get_type);
    GET_MONO_FUNC(mono_type_get_object);
    GET_MONO_FUNC(mono_class_get_nested_types);
    GET_MONO_FUNC(mono_class_get_name);
    GET_MONO_FUNC(mono_object_new);
    GET_MONO_FUNC(mono_field_set_value);
    GET_MONO_FUNC(mono_image_open_from_data_with_name);
    GET_MONO_FUNC(mono_free);
    GET_MONO_FUNC(mono_class_get_property_from_name);
    GET_MONO_FUNC(mono_property_get_get_method);
    GET_MONO_FUNC(mono_property_get_set_method);
    GET_MONO_FUNC(mono_thread_detach);
    GET_MONO_FUNC(mono_image_open_from_data);
    GET_MONO_FUNC(mono_assembly_load_from_full);
    GET_MONO_FUNC(mono_image_close);
    GET_MONO_FUNC(mono_assembly_close);

    // Get the root domain
    m_rootDomain = m_mono_get_root_domain();
    if (!m_rootDomain) {
        G::logger.LogError("Failed to get Mono root domain");
        return false;
    }

    G::logger.LogInfo("Root domain: 0x%p", m_rootDomain);

    // Attach the current thread to the Mono runtime
    if (!AttachThread()) {
        G::logger.LogError("Failed to attach thread to Mono runtime");
        return false;
    }

    // Iterate through assemblies
    m_mono_domain_assembly_foreach(m_rootDomain, reinterpret_cast<void (*)(void*, void*)>(AssemblyIterationCallback), this);

    G::logger.LogInfo("MonoRuntime initialized successfully");

    return true;
}

bool MonoRuntime::AttachThread() {
    // Don't attach if already attached
    if (m_thread) {
        return true;
    }

    if (!m_rootDomain || !m_mono_thread_attach) {
        G::logger.LogError("Cannot attach thread - root domain or thread_attach "
                           "function not available");
        return false;
    }

    // Attach the current thread to the Mono runtime
    m_thread = m_mono_thread_attach(m_rootDomain);
    if (!m_thread) {
        G::logger.LogInfo("Failed to attach thread to Mono runtime");
        return false;
    }

    G::logger.LogInfo("Thread attached to Mono runtime: %p", m_thread);
    return true;
}

void MonoRuntime::DetachThread() {
    if (m_thread && m_mono_thread_detach) {
        m_mono_thread_detach(m_thread);
        m_thread = nullptr;
        G::logger.LogInfo("Thread detached from Mono runtime");
    }
}

MonoAssembly* MonoRuntime::LoadAssemblyFromMemory(const char* data, size_t size, const char* name) {
    if (!data || size == 0 || !name) {
        G::logger.LogError("LoadAssemblyFromMemory: Invalid parameters");
        return nullptr;
    }

    MonoImageOpenStatus status;
    MonoImage* image = m_mono_image_open_from_data_with_name(const_cast<char*>(data), static_cast<uint32_t>(size), 1, &status, 0, name);
    if (!image || status != MONO_IMAGE_OK) {
        G::logger.LogError("LoadAssemblyFromMemory: Failed to open image from data "
                           "with name '%s', status: %d",
                           name, status);
        return nullptr;
    }

    MonoAssembly* assembly = m_mono_assembly_load_from_full(image, name, &status, 0);
    if (!assembly || status != MONO_IMAGE_OK) {
        G::logger.LogError("LoadAssemblyFromMemory: Failed to load assembly, status: %d", status);
        return nullptr;
    }

    m_imageCache[name] = image;

    G::logger.LogInfo("LoadAssemblyFromMemory: Successfully loaded assembly '%s'", name);
    return assembly;
}

MonoImage* MonoRuntime::GetAssemblyImage(MonoAssembly* assembly) {
    if (!assembly) {
        return nullptr;
    }
    return m_mono_assembly_get_image(assembly);
}

void MonoRuntime::UnloadAssembly(MonoAssembly* assembly) {
    if (!assembly) {
        return;
    }

    MonoImage* image = m_mono_assembly_get_image(assembly);
    const char* imageName = nullptr;
    if (image) {
        imageName = m_mono_image_get_name(image);
    }

    if (m_mono_assembly_close) {
        m_mono_assembly_close(assembly);
        G::logger.LogInfo("MonoRuntime: Unloaded assembly %p", assembly);
    }

    if (imageName) {
        auto it = m_imageCache.find(imageName);
        if (it != m_imageCache.end()) {
            m_imageCache.erase(it);
            G::logger.LogInfo("MonoRuntime: Removed '%s' from image cache", imageName);
        }

        for (auto it = m_classCache.begin(); it != m_classCache.end();) {
            it = m_classCache.erase(it);
        }
        G::logger.LogInfo("MonoRuntime: Cleared class cache after assembly unload");
    }
}

MonoImage* MonoRuntime::GetImage(const char* assemblyName) {
    auto it = m_imageCache.find(assemblyName);
    if (it != m_imageCache.end()) {
        return it->second;
    }
    return nullptr;
}

MonoClass* MonoRuntime::GetClass(const char* assemblyName, const char* nameSpace, const char* className) {
    // Check cache first
    std::string cacheKey = MakeClassCacheKey(nameSpace, className);
    auto it = m_classCache.find(cacheKey);
    if (it != m_classCache.end()) {
        return it->second;
    }

    // Check if this is a nested class (contains +)
    std::string classNameStr(className);
    size_t plusPos = classNameStr.find('+');
    if (plusPos != std::string::npos) {
        // This is a nested class - split into parent and nested parts
        std::string parentClassName = classNameStr.substr(0, plusPos);
        std::string nestedClassName = classNameStr.substr(plusPos + 1);

        // Get the parent class first
        MonoClass* parentClass = GetClass(assemblyName, nameSpace, parentClassName.c_str());
        if (parentClass) {
            // Search for the nested class
            void* iter = nullptr;
            MonoClass* nestedClass = nullptr;
            while ((nestedClass = m_mono_class_get_nested_types(parentClass, &iter))) {
                if (nestedClass) {
                    const char* nestedName = m_mono_class_get_name(nestedClass);
                    if (nestedName && strcmp(nestedName, nestedClassName.c_str()) == 0) {
                        // Add to cache and return
                        m_classCache[cacheKey] = nestedClass;
                        return nestedClass;
                    }
                }
            }
        }
        return nullptr;
    }

    // Find the class normally
    MonoImage* image = GetImage(assemblyName);
    if (!image) {
        G::logger.LogError("Failed to find assembly: %s", assemblyName);
        return nullptr;
    }

    MonoClass* klass = m_mono_class_from_name(image, nameSpace, className);
    if (klass) {
        m_classCache[cacheKey] = klass;
    }

    return klass;
}

MonoMethod* MonoRuntime::GetMethod(MonoClass* klass, const char* methodName, int paramCount) {
    if (!AttachThread() || !klass)
        return nullptr;
    return m_mono_class_get_method_from_name(klass, methodName, paramCount);
}

LPVOID MonoRuntime::GetMethodAddress(const char* assemblyName, const char* nameSpace, const char* className, const char* methodName, int paramCount) {
    MonoClass* klass = GetClass(assemblyName, nameSpace, className);
    if (!klass)
        return nullptr;
    MonoMethod* monoMethod = GetMethod(klass, methodName, paramCount);
    return m_mono_compile_method(monoMethod);
}

LPVOID MonoRuntime::GetMethodAddress(const char* assemblyName, const char* nameSpace, const char* className, const char* methodName, int paramCount,
                                     const char* returnType, const char** paramTypes) {
    void* targetAddress = nullptr;
    void* iter = nullptr;
    MonoMethod* method = nullptr;

    MonoClass* klass = GetClass(assemblyName, nameSpace, className);
    if (!klass) {
        return nullptr;
    }

    while ((method = m_mono_class_get_methods(klass, &iter))) {
        const char* name = m_mono_method_get_name(method);

        if (strcmp(name, methodName) == 0) {
            MonoMethodSignature* sig = m_mono_method_signature(method);

            // Check parameter count
            if (m_mono_signature_get_param_count(sig) == paramCount) {
                bool match = true;

                // Check return type
                MonoType* retType = m_mono_signature_get_return_type(sig);
                if (strcmp(m_mono_type_get_name(retType), returnType) != 0) {
                    match = false;
                }

                // Check parameter types
                void* paramIter = nullptr;
                for (int i = 0; i < paramCount; i++) {
                    MonoType* paramType = m_mono_signature_get_params(sig, &paramIter);
                    if (strcmp(m_mono_type_get_name(paramType), paramTypes[i]) != 0) {
                        match = false;
                        break;
                    }
                }

                // If we found the correct method
                if (match) {
                    targetAddress = m_mono_compile_method(method);
                    break;
                }
            }
        }
    }
    return targetAddress;
}

MonoObject* MonoRuntime::InvokeMethod(MonoMethod* method, void* obj, void** params) {
    if (!AttachThread() || !method)
        return nullptr;

    MonoObject* exception = nullptr;
    MonoObject* result = m_mono_runtime_invoke(method, obj, params, &exception);

    if (exception) {
        G::logger.LogError("Exception occurred during Mono method invocation, "
                           "method pointer: %p",
                           method);
        return nullptr;
    }

    return result;
}

MonoField* MonoRuntime::GetField(MonoClass* klass, const char* fieldName) {
    if (!AttachThread() || !klass)
        return nullptr;
    return m_mono_class_get_field_from_name(klass, fieldName);
}

MonoString* MonoRuntime::CreateString(const char* text) { return m_mono_string_new(m_rootDomain, text); }

std::string MonoRuntime::StringToUtf8(MonoString* monoString) {
    if (!AttachThread() || !monoString)
        return "";

    char* utf8 = m_mono_string_to_utf8(monoString);
    if (!utf8)
        return "";

    std::string result(utf8);

    if (m_mono_free) {
        m_mono_free(utf8);
    }

    return result;
}

MonoProperty* MonoRuntime::GetProperty(MonoClass* klass, const char* propertyName) {
    if (!AttachThread() || !klass || !m_mono_class_get_property_from_name)
        return nullptr;
    return m_mono_class_get_property_from_name(klass, propertyName);
}

MonoMethod* MonoRuntime::GetPropertyGetMethod(MonoProperty* prop) {
    if (!AttachThread() || !prop || !m_mono_property_get_get_method)
        return nullptr;
    return m_mono_property_get_get_method(prop);
}

MonoMethod* MonoRuntime::GetPropertySetMethod(MonoProperty* prop) {
    if (!AttachThread() || !prop || !m_mono_property_get_set_method)
        return nullptr;
    return m_mono_property_get_set_method(prop);
}

MonoDomain* MonoRuntime::GetRootDomain() const { return m_rootDomain; }

int MonoRuntime::GetArrayLength(MonoArray* array) {
    if (!AttachThread() || !array || !m_mono_array_length)
        return 0;
    return m_mono_array_length(array);
}

MonoClass* MonoRuntime::GetObjectClass(MonoObject* obj) {
    if (!AttachThread() || !obj || !m_mono_object_get_class)
        return nullptr;
    return m_mono_object_get_class(obj);
}

void* MonoRuntime::GetInternalCallPointer(MonoMethod* method) {
    if (!AttachThread() || !method || !m_mono_lookup_internal_call)
        return nullptr;
    return m_mono_lookup_internal_call(method);
}

MonoObject* MonoRuntime::CreateObject(MonoClass* klass) {
    if (!AttachThread() || !klass || !m_mono_object_new)
        return nullptr;
    return m_mono_object_new(m_rootDomain, klass);
}

void MonoRuntime::SetFieldValue(MonoObject* obj, MonoField* field, void* value) {
    if (!AttachThread() || !obj || !field || !m_mono_field_set_value)
        return;
    m_mono_field_set_value(obj, field, value);
}

MonoObject* MonoRuntime::GetTypeObject(MonoClass* klass) {
    if (!AttachThread() || !klass || !m_mono_class_get_type || !m_mono_type_get_object)
        return nullptr;

    MonoType* type = m_mono_class_get_type(klass);
    if (!type)
        return nullptr;

    return m_mono_type_get_object(m_rootDomain, type);
}

void* MonoRuntime::UnboxObject(MonoObject* obj) {
    if (!AttachThread() || !obj || !m_mono_object_unbox)
        return nullptr;
    return m_mono_object_unbox(obj);
}
