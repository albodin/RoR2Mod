#pragma once

#include <windows.h>
#include <string>
#include <unordered_map>
#include "MonoTypes.h"


// MonoRuntime.h - Base class for interacting with the Mono runtime
class MonoRuntime {
private:
    // Mono API function pointers
    mono_get_root_domain_t m_mono_get_root_domain;
    mono_domain_assembly_foreach_t m_mono_domain_assembly_foreach;
    mono_assembly_get_image_t m_mono_assembly_get_image;
    mono_image_get_name_t m_mono_image_get_name;
    mono_class_from_name_t m_mono_class_from_name;
    mono_class_get_method_from_name_t m_mono_class_get_method_from_name;
    mono_runtime_invoke_t m_mono_runtime_invoke;
    mono_class_get_field_from_name_t m_mono_class_get_field_from_name;
    mono_field_get_value_t m_mono_field_get_value;
    mono_field_static_get_value_t m_mono_field_static_get_value;
    mono_string_new_t m_mono_string_new;
    mono_string_to_utf8_t m_mono_string_to_utf8;
    mono_free_t m_mono_free;
    mono_class_get_property_from_name_t m_mono_class_get_property_from_name;
    mono_property_get_get_method_t m_mono_property_get_get_method;
    mono_property_get_set_method_t m_mono_property_get_set_method;
    mono_thread_attach_t m_mono_thread_attach;
    mono_thread_detach_t m_mono_thread_detach;
    mono_class_vtable_t m_mono_class_vtable;
    mono_object_get_class_t m_mono_object_get_class;
    mono_compile_method_t m_mono_compile_method;
    mono_class_get_methods_t m_mono_class_get_methods;
    mono_method_get_name_t m_mono_method_get_name;
    mono_method_signature_t m_mono_method_signature;
    mono_signature_get_param_count_t m_mono_signature_get_param_count;
    mono_signature_get_return_type_t m_mono_signature_get_return_type;
    mono_type_get_name_t m_mono_type_get_name;
    mono_signature_get_params_t m_mono_signature_get_params;
    mono_field_get_offset_t m_mono_field_get_offset;
    mono_class_get_fields_t m_mono_class_get_fields;
    mono_field_get_name_t m_mono_field_get_name;

    // Mono runtime state
    MonoDomain* m_rootDomain;
    MonoThread* m_thread;
    
    // Cache for found assemblies, images, and classes
    std::unordered_map<std::string, MonoImage*> m_imageCache;
    std::unordered_map<std::string, MonoClass*> m_classCache;
    
    // Helper function to generate cache keys
    std::string MakeClassCacheKey(const char* nameSpace, const char* className);
    
    // Assembly iteration callback
    static void __cdecl AssemblyIterationCallback(MonoAssembly* assembly, void* user_data);

public:
    mono_object_unbox_t m_mono_object_unbox;

    MonoRuntime();
    ~MonoRuntime();

    // Initialize the Mono runtime access
    bool Initialize(const char* monoDllName = "mono-2.0-bdwgc.dll");

    // Attach current thread to Mono runtime
    bool AttachThread();
    
    // Detach current thread from Mono runtime
    void DetachThread();
    
    // Get a MonoImage by name
    MonoImage* GetImage(const char* assemblyName);
    
    // Get a MonoClass by namespace and name
    MonoClass* GetClass(const char* assemblyName, const char* nameSpace, const char* className);
    
    // Get a method from a class
    MonoMethod* GetMethod(MonoClass* klass, const char* methodName, int paramCount);
    LPVOID GetMethodAddress(const char* assemblyName, const char* nameSpace, const char* className, const char* methodName, int paramCount);
    LPVOID GetMethodAddress(const char* assemblyName, const char* nameSpace, const char* className, const char* methodName, int paramCount, const char* returnType, const char** paramTypes);

    // Invoke a method
    MonoObject* InvokeMethod(MonoMethod* method, void* obj, void** params);
    
    // Get a field from a class
    MonoField* GetField(MonoClass* klass, const char* fieldName);
    
    // Get a field value
    template<typename T>
    T GetFieldValue(MonoObject* obj, MonoField* field);
    
    // Get a static field value
    template<typename T>
    T GetStaticFieldValue(MonoClass* klass, MonoField* field);
    
    // Create a MonoString
    MonoString* CreateString(const char* text);
    
    // Convert MonoString to C++ string
    std::string StringToUtf8(MonoString* monoString);
    
    // Get a property
    MonoProperty* GetProperty(MonoClass* klass, const char* propertyName);
    
    // Get a property's getter method
    MonoMethod* GetPropertyGetMethod(MonoProperty* prop);
    
    // Get a property's setter method
    MonoMethod* GetPropertySetMethod(MonoProperty* prop);
    
    // Get the root domain
    MonoDomain* GetRootDomain() const;
};

// Template method implementations
template<typename T>
T MonoRuntime::GetFieldValue(MonoObject* obj, MonoField* field) {
    if (!obj || !field) {
        T defaultValue = T();
        return defaultValue;
    }
    
    T value;
    m_mono_field_get_value(obj, field, &value);
    return value;
}

template<typename T>
T MonoRuntime::GetStaticFieldValue(MonoClass* klass, MonoField* field) {
    if (!klass || !field) {
        T defaultValue = T();
        return defaultValue;
    }
    
    // Get the vtable for the class
    MonoVTable* vtable = m_mono_class_vtable(m_rootDomain, klass);
    if (!vtable) {
        T defaultValue = T();
        return defaultValue;
    }
    
    T value;
    m_mono_field_static_get_value(vtable, field, &value);
    return value;
}