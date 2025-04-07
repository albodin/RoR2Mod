#pragma once
#include <windows.h>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <list>
#include <set>

class MonoAPI {
private:
    HMODULE monoModule = nullptr;
    std::list<void*> m_assemblies;
    std::unordered_map<void*, std::string> classNameMap;
    std::set<std::string> processedClasses;
    void* monoThread = nullptr;

    // Function pointer types for Mono API functions
    typedef void* (*mono_get_root_domain_fn)();
    typedef void* (*mono_domain_assembly_open_fn)(void* domain, const char* name);
    typedef void* (*mono_assembly_get_image_fn)(void* assembly);
    typedef void* (*mono_class_from_name_fn)(void* image, const char* name_space, const char* name);
    typedef void* (*mono_class_get_fields_fn)(void* klass, void** iter);
    typedef const char* (*mono_field_get_name_fn)(void* field);
    typedef void* (*mono_field_get_type_fn)(void* field);
    typedef int (*mono_field_get_offset_fn)(void* field);
    typedef int (*mono_type_get_type_fn)(void* type);
    typedef void (*mono_domain_assembly_foreach_fn)(void* domain, void(*func)(void* assembly, void* user_data), void* user_data);
    typedef const char* (*mono_image_get_name_fn)(void* image);
    typedef void* (*mono_image_get_table_info_fn)(void* image, int table_id);
    typedef int (*mono_table_info_get_rows_fn)(const void* table);
    typedef void* (*mono_class_get_fn)(void* image, unsigned int token);
    typedef const char* (*mono_class_get_name_fn)(void* klass);
    typedef const char* (*mono_class_get_namespace_fn)(void* klass);
    typedef void* (*mono_type_get_class_fn)(void* type);
    typedef void* (*mono_class_get_image_fn)(void* klass);
    typedef int (*mono_field_get_flags_fn)(void* field);
    typedef void* (*mono_thread_attach_fn)(void* domain);

    // Function pointers
    mono_get_root_domain_fn m_mono_get_root_domain = nullptr;
    mono_domain_assembly_open_fn m_mono_domain_assembly_open = nullptr;
    mono_assembly_get_image_fn m_mono_assembly_get_image = nullptr;
    mono_class_from_name_fn m_mono_class_from_name = nullptr;
    mono_class_get_fields_fn m_mono_class_get_fields = nullptr;
    mono_field_get_name_fn m_mono_field_get_name = nullptr;
    mono_field_get_type_fn m_mono_field_get_type = nullptr;
    mono_field_get_offset_fn m_mono_field_get_offset = nullptr;
    mono_type_get_type_fn m_mono_type_get_type = nullptr;
    mono_domain_assembly_foreach_fn m_mono_domain_assembly_foreach = nullptr;
    mono_image_get_name_fn m_mono_image_get_name = nullptr;
    mono_image_get_table_info_fn m_mono_image_get_table_info = nullptr;
    mono_table_info_get_rows_fn m_mono_table_info_get_rows = nullptr;
    mono_class_get_fn m_mono_class_get = nullptr;
    mono_class_get_name_fn m_mono_class_get_name = nullptr;
    mono_class_get_namespace_fn m_mono_class_get_namespace = nullptr;
    mono_type_get_class_fn m_mono_type_get_class = nullptr;
    mono_class_get_image_fn m_mono_class_get_image = nullptr;
    mono_field_get_flags_fn m_mono_field_get_flags = nullptr;
    mono_thread_attach_fn m_mono_thread_attach = nullptr;

    // Mono type enum values
    enum MonoTypeEnum {
        MONO_TYPE_END        = 0x00,
        MONO_TYPE_VOID       = 0x01,
        MONO_TYPE_BOOLEAN    = 0x02,
        MONO_TYPE_CHAR       = 0x03,
        MONO_TYPE_I1         = 0x04,
        MONO_TYPE_U1         = 0x05,
        MONO_TYPE_I2         = 0x06,
        MONO_TYPE_U2         = 0x07,
        MONO_TYPE_I4         = 0x08,
        MONO_TYPE_U4         = 0x09,
        MONO_TYPE_I8         = 0x0a,
        MONO_TYPE_U8         = 0x0b,
        MONO_TYPE_R4         = 0x0c,
        MONO_TYPE_R8         = 0x0d,
        MONO_TYPE_STRING     = 0x0e,
        MONO_TYPE_PTR        = 0x0f,
        MONO_TYPE_BYREF      = 0x10,
        MONO_TYPE_VALUETYPE  = 0x11,
        MONO_TYPE_CLASS      = 0x12,
        MONO_TYPE_VAR	     = 0x13,
        MONO_TYPE_ARRAY      = 0x14,
        MONO_TYPE_GENERICINST= 0x15,
        MONO_TYPE_TYPEDBYREF = 0x16,
        MONO_TYPE_I          = 0x18,
        MONO_TYPE_U          = 0x19,
        MONO_TYPE_FNPTR      = 0x1b,
        MONO_TYPE_OBJECT     = 0x1c,
        MONO_TYPE_SZARRAY    = 0x1d,
        MONO_TYPE_MVAR	     = 0x1e,
        MONO_TYPE_CMOD_REQD  = 0x1f,
        MONO_TYPE_CMOD_OPT   = 0x20,
        MONO_TYPE_INTERNAL   = 0x21,

        MONO_TYPE_MODIFIER   = 0x40,
        MONO_TYPE_SENTINEL   = 0x41,
        MONO_TYPE_PINNED     = 0x45,

        MONO_TYPE_ENUM       = 0x55
    };

    // Mono table ID for type definitions
    enum MonoTableID {
        MONO_TABLE_TYPEDEF = 0x02
    };

    // Mono token type for type definitions
    const unsigned int MONO_TOKEN_TYPE_DEF = 0x02000000;

    static void AssemblyIterationCallback(void* assembly, void* assemblies);
    std::string GetFieldTypeName(void* fieldType);

public:
    MonoAPI() = default;
    ~MonoAPI();

    bool Initialize(const std::string& monoDllPath = "mono-2.0-bdwgc.dll");
    std::string GetCppTypeFromMonoType(void* type);
    void CatalogClasses();
    void* GetTypeClass(void* type);
    std::string GetClassAssemblyName(void* klass);
    void GenerateStructFromClass(void* klass, std::ofstream& file, std::set<std::string>& requiredIncludes);
    void DumpAllClassesToStructs(const std::string& outputDir);
};