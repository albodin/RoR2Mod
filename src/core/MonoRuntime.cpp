#include "MonoRuntime.h"
#include <stdio.h>
#include "globals/globals.h"

MonoRuntime::MonoRuntime() : m_rootDomain(nullptr), m_thread(nullptr) {
}

MonoRuntime::~MonoRuntime() {
    DetachThread();
}

std::string MonoRuntime::MakeClassCacheKey(const char* nameSpace, const char* className) {
    return std::string(nameSpace) + "::" + className;
}

void __cdecl MonoRuntime::AssemblyIterationCallback(MonoAssembly* assembly, void* user_data) {
    auto* self = static_cast<MonoRuntime*>(user_data);
    MonoImage* image = self->m_mono_assembly_get_image(assembly);
    const char* imageName = self->m_mono_image_get_name(image);
    
    // Add to image cache
    self->m_imageCache[imageName] = image;
    
    // Debug output
    G::logger.LogInfo("Found assembly: %s", imageName);
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
    #define GET_MONO_FUNC(name) \
        m_##name = reinterpret_cast<name##_t>(GetProcAddress(monoModule, #name)); \
        if (!m_##name) { \
            G::logger.LogError("Failed to get %s function", #name); \
            return false; \
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
    
    // These might not exist in all Mono versions, so don't fail if not found
    m_mono_free = reinterpret_cast<mono_free_t>(GetProcAddress(monoModule, "mono_free"));
    m_mono_class_get_property_from_name = reinterpret_cast<mono_class_get_property_from_name_t>(
        GetProcAddress(monoModule, "mono_class_get_property_from_name"));
    m_mono_property_get_get_method = reinterpret_cast<mono_property_get_get_method_t>(
        GetProcAddress(monoModule, "mono_property_get_get_method"));
    m_mono_property_get_set_method = reinterpret_cast<mono_property_get_set_method_t>(
        GetProcAddress(monoModule, "mono_property_get_set_method"));
    m_mono_thread_detach = reinterpret_cast<mono_thread_detach_t>(
        GetProcAddress(monoModule, "mono_thread_detach"));

    // Get the root domain
    m_rootDomain = m_mono_get_root_domain();
    if (!m_rootDomain) {
        G::logger.LogError("Failed to get Mono root domain");
        return false;
    }
    
    G::logger.LogInfo("Root domain: 0x%p\n", m_rootDomain);

    // Attach the current thread to the Mono runtime
    if (!AttachThread()) {
        G::logger.LogError("Failed to attach thread to Mono runtime");
        return false;
    }

    // Iterate through assemblies
    m_mono_domain_assembly_foreach(m_rootDomain, reinterpret_cast<void(*)(void*, void*)>(AssemblyIterationCallback), this);

    return true;
}

bool MonoRuntime::AttachThread() {
    // Don't attach if already attached
    if (m_thread) {
        return true;
    }
    
    if (!m_rootDomain || !m_mono_thread_attach) {
        G::logger.LogError("Cannot attach thread - root domain or thread_attach function not available");
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
        G::logger.Log("Thread detached from Mono runtime");
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
    
    // Find the class
    MonoImage* image = GetImage(assemblyName);
    if (!image) {
        G::logger.LogError("Failed to find assembly: %s", assemblyName);
        return nullptr;
    }
    
    MonoClass* klass = m_mono_class_from_name(image, nameSpace, className);
    if (klass) {
        // Add to cache
        m_classCache[cacheKey] = klass;
    }
    
    return klass;
}

MonoMethod* MonoRuntime::GetMethod(MonoClass* klass, const char* methodName, int paramCount) {
    if (!AttachThread() || !klass) return nullptr;
    return m_mono_class_get_method_from_name(klass, methodName, paramCount);
}

LPVOID MonoRuntime::GetMethodAddress(const char* assemblyName, const char* nameSpace, const char* className, const char* methodName, int paramCount) {
    MonoClass* klass = GetClass(assemblyName, nameSpace, className);
    if (!klass) return nullptr;
    MonoMethod* monoMethod = GetMethod(klass, methodName, paramCount);
    return m_mono_compile_method(monoMethod);
}

LPVOID MonoRuntime::GetMethodAddress(const char* assemblyName, const char* nameSpace, const char* className, const char* methodName, int paramCount, const char* returnType, const char** paramTypes) {
    void* targetAddress = nullptr;
    void* iter = nullptr;
    MonoMethod* method = nullptr;
    MonoClass* klass = GetClass(assemblyName, nameSpace, className);
    if (!klass) return nullptr;

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
    if (!AttachThread() || !method) return nullptr;

    MonoObject* exception = nullptr;
    MonoObject* result = m_mono_runtime_invoke(method, obj, params, &exception);

    if (exception) {
        G::logger.LogError("Exception occurred during Mono method invocation, method pointer: %p", method);
        return nullptr;
    }
    
    return result;
}

MonoField* MonoRuntime::GetField(MonoClass* klass, const char* fieldName) {
    if (!AttachThread() || !klass) return nullptr;
    return m_mono_class_get_field_from_name(klass, fieldName);
}

MonoString* MonoRuntime::CreateString(const char* text) {
    return m_mono_string_new(m_rootDomain, text);
}

std::string MonoRuntime::StringToUtf8(MonoString* monoString) {
    if (!AttachThread() || !monoString) return "";
    
    char* utf8 = m_mono_string_to_utf8(monoString);
    if (!utf8) return "";
    
    std::string result(utf8);
    
    // Free the string if the function is available
    if (m_mono_free) {
        m_mono_free(utf8);
    }
    
    return result;
}

MonoProperty* MonoRuntime::GetProperty(MonoClass* klass, const char* propertyName) {
    if (!AttachThread() || !klass || !m_mono_class_get_property_from_name) return nullptr;
    return m_mono_class_get_property_from_name(klass, propertyName);
}

MonoMethod* MonoRuntime::GetPropertyGetMethod(MonoProperty* prop) {
    if (!AttachThread() || !prop || !m_mono_property_get_get_method) return nullptr;
    return m_mono_property_get_get_method(prop);
}

MonoMethod* MonoRuntime::GetPropertySetMethod(MonoProperty* prop) {
    if (!AttachThread() || !prop || !m_mono_property_get_set_method) return nullptr;
    return m_mono_property_get_set_method(prop);
}

MonoDomain* MonoRuntime::GetRootDomain() const {
    return m_rootDomain;
}

int MonoRuntime::GetArrayLength(MonoArray* array) {
    if (!AttachThread() || !array || !m_mono_array_length) return 0;
    return m_mono_array_length(array);
}

MonoClass* MonoRuntime::GetObjectClass(MonoObject* obj) {
    if (!AttachThread() || !obj || !m_mono_object_get_class) return nullptr;
    return m_mono_object_get_class(obj);
}

void* MonoRuntime::GetInternalCallPointer(MonoMethod* method) {
    if (!AttachThread() || !method || !m_mono_lookup_internal_call) return nullptr;
    return m_mono_lookup_internal_call(method);
}