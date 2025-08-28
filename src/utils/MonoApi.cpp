#include "MonoApi.h"
#include <globals/globals.h>
#include <iostream>
#include <map>

MonoAPI::~MonoAPI() { monoModule = nullptr; }

bool MonoAPI::Initialize(const std::string& monoDllPath) {
    monoModule = GetModuleHandleA(monoDllPath.c_str());
    if (!monoModule) {
        LOG_ERROR("Failed to get %s handle", monoDllPath.c_str());
        return false;
    } else {
        LOG_INFO("%s base address: 0x%p", monoDllPath.c_str(), monoModule);
    }

    // Load all function pointers
    m_mono_get_root_domain = reinterpret_cast<mono_get_root_domain_fn>(GetProcAddress(monoModule, "mono_get_root_domain"));
    m_mono_domain_assembly_open = reinterpret_cast<mono_domain_assembly_open_fn>(GetProcAddress(monoModule, "mono_domain_assembly_open"));
    m_mono_assembly_get_image = reinterpret_cast<mono_assembly_get_image_fn>(GetProcAddress(monoModule, "mono_assembly_get_image"));
    m_mono_class_from_name = reinterpret_cast<mono_class_from_name_fn>(GetProcAddress(monoModule, "mono_class_from_name"));
    m_mono_class_get_fields = reinterpret_cast<mono_class_get_fields_fn>(GetProcAddress(monoModule, "mono_class_get_fields"));
    m_mono_field_get_name = reinterpret_cast<mono_field_get_name_fn>(GetProcAddress(monoModule, "mono_field_get_name"));
    m_mono_field_get_type = reinterpret_cast<mono_field_get_type_fn>(GetProcAddress(monoModule, "mono_field_get_type"));
    m_mono_field_get_offset = reinterpret_cast<mono_field_get_offset_fn>(GetProcAddress(monoModule, "mono_field_get_offset"));
    m_mono_type_get_type = reinterpret_cast<mono_type_get_type_fn>(GetProcAddress(monoModule, "mono_type_get_type"));
    m_mono_domain_assembly_foreach = reinterpret_cast<mono_domain_assembly_foreach_fn>(GetProcAddress(monoModule, "mono_domain_assembly_foreach"));
    m_mono_image_get_name = reinterpret_cast<mono_image_get_name_fn>(GetProcAddress(monoModule, "mono_image_get_name"));
    m_mono_image_get_table_info = reinterpret_cast<mono_image_get_table_info_fn>(GetProcAddress(monoModule, "mono_image_get_table_info"));
    m_mono_table_info_get_rows = reinterpret_cast<mono_table_info_get_rows_fn>(GetProcAddress(monoModule, "mono_table_info_get_rows"));
    m_mono_class_get = reinterpret_cast<mono_class_get_fn>(GetProcAddress(monoModule, "mono_class_get"));
    m_mono_class_get_name = reinterpret_cast<mono_class_get_name_fn>(GetProcAddress(monoModule, "mono_class_get_name"));
    m_mono_class_get_namespace = reinterpret_cast<mono_class_get_namespace_fn>(GetProcAddress(monoModule, "mono_class_get_namespace"));
    m_mono_type_get_class = reinterpret_cast<mono_type_get_class_fn>(GetProcAddress(monoModule, "mono_type_get_class"));
    m_mono_class_get_image = reinterpret_cast<mono_class_get_image_fn>(GetProcAddress(monoModule, "mono_class_get_image"));
    m_mono_field_get_flags = reinterpret_cast<mono_field_get_flags_fn>(GetProcAddress(monoModule, "mono_field_get_flags"));
    m_mono_thread_attach = reinterpret_cast<mono_thread_attach_fn>(GetProcAddress(monoModule, "mono_thread_attach"));
    m_mono_class_get_methods = reinterpret_cast<mono_class_get_methods_fn>(GetProcAddress(monoModule, "mono_class_get_methods"));
    m_mono_method_get_name = reinterpret_cast<mono_method_get_name_fn>(GetProcAddress(monoModule, "mono_method_get_name"));
    m_mono_method_signature = reinterpret_cast<mono_method_signature_fn>(GetProcAddress(monoModule, "mono_method_signature"));
    m_mono_signature_get_param_count = reinterpret_cast<mono_signature_get_param_count_fn>(GetProcAddress(monoModule, "mono_signature_get_param_count"));
    m_mono_signature_get_return_type = reinterpret_cast<mono_signature_get_return_type_fn>(GetProcAddress(monoModule, "mono_signature_get_return_type"));
    m_mono_signature_get_params = reinterpret_cast<mono_signature_get_params_fn>(GetProcAddress(monoModule, "mono_signature_get_params"));
    m_mono_method_get_flags = reinterpret_cast<mono_method_get_flags_fn>(GetProcAddress(monoModule, "mono_method_get_flags"));

    LOG_INFO("mono_get_root_domain: 0x%p", m_mono_get_root_domain);
    LOG_INFO("mono_domain_assembly_open: 0x%p", m_mono_domain_assembly_open);
    LOG_INFO("mono_assembly_get_image: 0x%p", m_mono_assembly_get_image);
    LOG_INFO("mono_class_from_name: 0x%p", m_mono_class_from_name);
    LOG_INFO("mono_class_get_fields: 0x%p", m_mono_class_get_fields);
    LOG_INFO("mono_field_get_name: 0x%p", m_mono_field_get_name);
    LOG_INFO("mono_field_get_type: 0x%p", m_mono_field_get_type);
    LOG_INFO("mono_field_get_offset: 0x%p", m_mono_field_get_offset);
    LOG_INFO("mono_type_get_type: 0x%p", m_mono_type_get_type);
    LOG_INFO("mono_domain_assembly_foreach: 0x%p", m_mono_domain_assembly_foreach);
    LOG_INFO("mono_image_get_name: 0x%p", m_mono_image_get_name);
    LOG_INFO("mono_image_get_table_info: 0x%p", m_mono_image_get_table_info);
    LOG_INFO("mono_table_info_get_rows: 0x%p", m_mono_table_info_get_rows);
    LOG_INFO("mono_class_get: 0x%p", m_mono_class_get);
    LOG_INFO("mono_class_get_name: 0x%p", m_mono_class_get_name);
    LOG_INFO("mono_class_get_namespace: 0x%p", m_mono_class_get_namespace);
    LOG_INFO("mono_type_get_class: 0x%p", m_mono_type_get_class);
    LOG_INFO("mono_class_get_image: 0x%p", m_mono_class_get_image);
    LOG_INFO("mono_field_get_flags: 0x%p", m_mono_field_get_flags);
    LOG_INFO("mono_thread_attach: 0x%p", m_mono_thread_attach);
    LOG_INFO("mono_class_get_methods: 0x%p", m_mono_class_get_methods);
    LOG_INFO("mono_method_get_name: 0x%p", m_mono_method_get_name);
    LOG_INFO("mono_method_signature: 0x%p", m_mono_method_signature);
    LOG_INFO("mono_signature_get_param_count: 0x%p", m_mono_signature_get_param_count);
    LOG_INFO("mono_signature_get_return_type: 0x%p", m_mono_signature_get_return_type);
    LOG_INFO("mono_signature_get_params: 0x%p", m_mono_signature_get_params);
    LOG_INFO("mono_method_get_flags: 0x%p", m_mono_method_get_flags);

    // Check if all required functions were loaded
    return m_mono_get_root_domain && m_mono_domain_assembly_open && m_mono_assembly_get_image && m_mono_class_from_name && m_mono_class_get_fields &&
           m_mono_field_get_name && m_mono_field_get_type && m_mono_field_get_offset && m_mono_type_get_type && m_mono_domain_assembly_foreach &&
           m_mono_image_get_name && m_mono_image_get_table_info && m_mono_table_info_get_rows && m_mono_class_get && m_mono_class_get_name &&
           m_mono_class_get_namespace && m_mono_type_get_class && m_mono_class_get_image && m_mono_field_get_flags && m_mono_thread_attach &&
           m_mono_class_get_methods && m_mono_method_get_name && m_mono_method_signature && m_mono_signature_get_param_count &&
           m_mono_signature_get_return_type && m_mono_signature_get_params && m_mono_method_get_flags;
}

void __cdecl MonoAPI::AssemblyIterationCallback(void* assembly, void* user_data) {
    auto* self = static_cast<MonoAPI*>(user_data);
    void* image = self->m_mono_assembly_get_image(assembly);
    self->m_assemblies.push_back(image);

    const char* imageName = self->m_mono_image_get_name(image);
    LOG_INFO("Found assembly: %s", imageName);
}

std::string MonoAPI::GetCppTypeFromMonoType(void* type) {
    if (!type || !m_mono_type_get_type)
        return "void*";

    int type_enum = m_mono_type_get_type(type);

    switch (type_enum) {
    case MONO_TYPE_VOID:
        return "void";
    case MONO_TYPE_BOOLEAN:
        return "bool";
    case MONO_TYPE_CHAR:
        return "char16_t";
    case MONO_TYPE_I1:
        return "int8_t";
    case MONO_TYPE_U1:
        return "uint8_t";
    case MONO_TYPE_I2:
        return "int16_t";
    case MONO_TYPE_U2:
        return "uint16_t";
    case MONO_TYPE_I4:
        return "int32_t";
    case MONO_TYPE_U4:
        return "uint32_t";
    case MONO_TYPE_I8:
        return "int64_t";
    case MONO_TYPE_U8:
        return "uint64_t";
    case MONO_TYPE_R4:
        return "float";
    case MONO_TYPE_R8:
        return "double";
    case MONO_TYPE_STRING:
        return "MonoString*";
    case MONO_TYPE_PTR:
        return "void*"; // Need context for specific type
    case MONO_TYPE_BYREF:
        return "void*"; // Need context for specific type
    case MONO_TYPE_VALUETYPE: {
        void* klass = GetTypeClass(type);
        if (klass && m_mono_class_get_name) {
            const char* className = m_mono_class_get_name(klass);
            if (className) {
                const char* nsName = m_mono_class_get_namespace(klass);
                std::string fullName = std::string(nsName ? nsName : "") + "." + className;
                LOG_INFO("Using value type: %s", fullName.c_str());

                return std::string(className) + "_Value";
            }
        }
        return "MONO_TYPE_VALUETYPE";
    }
    case MONO_TYPE_CLASS: {
        void* klass = GetTypeClass(type);
        if (klass && m_mono_class_get_name) {
            const char* className = m_mono_class_get_name(klass);
            if (className) {
                return std::string(className) + "*";
            }
        }
        return "void*";
    }
    case MONO_TYPE_ARRAY:
        return "MonoArray*";
    case MONO_TYPE_SZARRAY:
        return "MonoArray*";
    case MONO_TYPE_I:
        return "intptr_t";
    case MONO_TYPE_U:
        return "uintptr_t";
    case MONO_TYPE_OBJECT:
        return "MonoObject*";
    case MONO_TYPE_ENUM:
        return "enum"; // Need context for specific type
    case MONO_TYPE_END:
        return "MONO_TYPE_END"; // Not a real type, end marker
    case MONO_TYPE_VAR:
        return "MONO_TYPE_VAR"; // Need context
    case MONO_TYPE_GENERICINST:
        return "MONO_TYPE_GENERICINST"; // Need type args
    case MONO_TYPE_TYPEDBYREF:
        return "MONO_TYPE_TYPEDBYREF"; // Custom struct
    case MONO_TYPE_FNPTR:
        return "MONO_TYPE_FNPTR"; // Generic function pointer, needs signature
    case MONO_TYPE_MVAR:
        return "MONO_TYPE_MVAR";
    case MONO_TYPE_CMOD_REQD:
        return "MONO_TYPE_CMOD_REQD";
    case MONO_TYPE_CMOD_OPT:
        return "MONO_TYPE_CMOD_OPT";
    case MONO_TYPE_INTERNAL:
        return "MONO_TYPE_INTERNAL";
    case MONO_TYPE_MODIFIER:
        return "MONO_TYPE_MODIFIER";
    case MONO_TYPE_SENTINEL:
        return "MONO_TYPE_SENTINEL";
    case MONO_TYPE_PINNED:
        return "MONO_TYPE_PINNED";
    default:
        return "UNKNOWN_ENUM_" + std::to_string(type_enum);
    }
}

std::string CleanupFieldName(const std::string& fieldName) {
    std::string result = fieldName;

    // Look for pattern <name>k__BackingField and clean it up
    size_t startBracket = result.find('<');
    size_t endBracket = result.find('>');
    size_t backing = result.find("k__BackingField");

    if (startBracket != std::string::npos && endBracket != std::string::npos && backing != std::string::npos && startBracket < endBracket &&
        endBracket < backing) {
        // Extract the name between brackets
        std::string propertyName = result.substr(startBracket + 1, endBracket - startBracket - 1);
        // Replace with propertyName_backing
        result = propertyName + "_backing";
    }

    return result;
}

void* MonoAPI::GetTypeClass(void* type) {
    if (!type || !m_mono_type_get_class) {
        return nullptr;
    }

    int typeEnum = m_mono_type_get_type(type);
    if (typeEnum != MONO_TYPE_CLASS && typeEnum != MONO_TYPE_STRING && typeEnum != MONO_TYPE_OBJECT && typeEnum != MONO_TYPE_ARRAY &&
        typeEnum != MONO_TYPE_SZARRAY && typeEnum != MONO_TYPE_GENERICINST && typeEnum != MONO_TYPE_VALUETYPE) {
        // Not a class or value type
        return nullptr;
    }

    if (typeEnum == MONO_TYPE_GENERICINST) {
        // TODO: Finish using `mono_type_get_generic_class`
        return nullptr;
    }

    return m_mono_type_get_class(type);
}

std::string MonoAPI::GetClassAssemblyName(void* klass) {
    if (!klass || !m_mono_class_get_image) {
        return "";
    }

    void* image = m_mono_class_get_image(klass);
    if (!image || !m_mono_image_get_name) {
        return "";
    }

    const char* assemblyName = m_mono_image_get_name(image);
    return assemblyName ? assemblyName : "";
}

size_t MonoAPI::CalculateClassSize(void* klass) {
    if (classSizes.find(klass) != classSizes.end()) {
        return classSizes[klass];
    }

    classSizes[klass] = 8;

    if (!klass || !m_mono_class_get_fields) {
        return 8;
    }

    const char* className = m_mono_class_get_name(klass);
    const char* namespaceName = m_mono_class_get_namespace(klass);
    std::string fullClassName;
    if (namespaceName && namespaceName[0] != '\0') {
        fullClassName = std::string(namespaceName) + "." + className;
    } else {
        fullClassName = std::string("<global>.") + className;
    }
    LOG_INFO("Calculating size for class: %s", fullClassName.c_str());

    void* iter = NULL;
    void* field;
    size_t maxOffset = 0;
    size_t maxFieldSize = 0;
    size_t minOffset = SIZE_MAX;
    bool hasFields = false;

    while ((field = m_mono_class_get_fields(klass, &iter))) {
        int flags = m_mono_field_get_flags(field);
        bool isStatic = (flags & 0x0010) != 0; // FIELD_ATTRIBUTE_STATIC
        if (isStatic)
            continue;

        int offset = m_mono_field_get_offset(field);
        if (offset < minOffset) {
            minOffset = offset;
        }
        hasFields = true;
    }

    if (!hasFields) {
        size_t totalSize = 1;
        classSizes[klass] = totalSize;
        fullNameToSize[fullClassName] = totalSize;
        if (namespaceName && className) {
            namespaceClassToSize[namespaceName][className] = totalSize;
        }
        LOG_INFO("Class %s has no fields, size: %zu bytes", fullClassName.c_str(), totalSize);
        return totalSize;
    }

    if (minOffset == SIZE_MAX) {
        minOffset = 0;
    }

    LOG_INFO("Class %s minimum field offset: %zu", fullClassName.c_str(), minOffset);

    iter = NULL;
    while ((field = m_mono_class_get_fields(klass, &iter))) {
        // Skip static fields
        int flags = m_mono_field_get_flags(field);
        bool isStatic = (flags & 0x0010) != 0; // FIELD_ATTRIBUTE_STATIC
        if (isStatic)
            continue;

        const char* originalFieldName = m_mono_field_get_name(field);
        int rawOffset = m_mono_field_get_offset(field);
        int offset = rawOffset - minOffset; // Normalize the offset
        void* fieldType = m_mono_field_get_type(field);
        int typeEnum = m_mono_type_get_type(fieldType);

        LOG_INFO("Field: %s, Raw offset: %d, Normalized offset: %d", originalFieldName, rawOffset, offset);

        size_t fieldSize = 8;
        if (typeEnum == MONO_TYPE_BOOLEAN || typeEnum == MONO_TYPE_I1 || typeEnum == MONO_TYPE_U1) {
            fieldSize = 1;
        } else if (typeEnum == MONO_TYPE_CHAR || typeEnum == MONO_TYPE_I2 || typeEnum == MONO_TYPE_U2) {
            fieldSize = 2;
        } else if (typeEnum == MONO_TYPE_I4 || typeEnum == MONO_TYPE_U4 || typeEnum == MONO_TYPE_R4 || typeEnum == MONO_TYPE_ENUM) {
            fieldSize = 4;
        } else if (typeEnum == MONO_TYPE_VALUETYPE) {
            void* fieldClass = GetTypeClass(fieldType);
            if (fieldClass) {
                const char* fieldClassName = m_mono_class_get_name(fieldClass);
                const char* fieldNamespace = m_mono_class_get_namespace(fieldClass);
                std::string fieldFullName = std::string(fieldNamespace ? fieldNamespace : "") + "." + fieldClassName;
                LOG_INFO("Value type field: %s, class: %s", originalFieldName, fieldFullName.c_str());

                // Recursive calculation of field size
                fieldSize = CalculateClassSize(fieldClass);
            }
        }

        if (offset + fieldSize > maxOffset + maxFieldSize) {
            maxOffset = offset;
            maxFieldSize = fieldSize;
        }
    }

    size_t totalSize = maxOffset + maxFieldSize;
    if (totalSize == 0)
        totalSize = 1;

    classSizes[klass] = totalSize;
    std::string nsPrefix = (namespaceName && namespaceName[0] != '\0') ? std::string(namespaceName) + "." : ".";
    std::string storedFullName = nsPrefix + className;
    fullNameToSize[storedFullName] = totalSize;

    std::string nsKey = (namespaceName && namespaceName[0] != '\0') ? namespaceName : "";
    namespaceClassToSize[nsKey][className] = totalSize;

    LOG_INFO("Class %s size: %zu bytes", fullClassName.c_str(), totalSize);
    return totalSize;
}

void MonoAPI::BuildClassSizeMap() {
    LOG_INFO("Building class size map...");

    classSizes.clear();
    fullNameToSize.clear();
    namespaceClassToSize.clear();

    for (auto image : m_assemblies) {
        const void* typeDefTable = m_mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int rows = m_mono_table_info_get_rows(typeDefTable);

        for (int j = 0; j < rows; j++) {
            void* klass = m_mono_class_get(image, j + 1 | MONO_TOKEN_TYPE_DEF);
            if (klass) {
                CalculateClassSize(klass);
            }
        }
    }

    LOG_INFO("Built size map for %zu classes", classSizes.size());
}

size_t MonoAPI::GetClassSizeByName(const std::string& className, const std::string& namespaceName) {
    if (!namespaceName.empty()) {
        std::string fullName = namespaceName + "." + className;
        if (fullNameToSize.find(fullName) != fullNameToSize.end()) {
            return fullNameToSize[fullName];
        }
    } else {
        std::string globalFullName = "." + className;
        if (fullNameToSize.find(globalFullName) != fullNameToSize.end()) {
            return fullNameToSize[globalFullName];
        }
    }

    // Prioritization for common classes
    if (namespaceClassToSize.find("UnityEngine") != namespaceClassToSize.end()) {
        auto& unityClasses = namespaceClassToSize["UnityEngine"];
        if (unityClasses.find(className) != unityClasses.end()) {
            LOG_INFO("Prioritizing UnityEngine.%s over other namespaces", className.c_str());
            return unityClasses[className];
        }
    } else if (namespaceClassToSize.find("System.Numerics") != namespaceClassToSize.end()) {
        auto& numericsClasses = namespaceClassToSize["System.Numerics"];
        if (numericsClasses.find(className) != numericsClasses.end()) {
            LOG_INFO("Using System.Numerics.%s", className.c_str());
            return numericsClasses[className];
        }
    }

    size_t foundSize = 0;
    bool foundMultiple = false;
    std::string foundNamespace;

    for (const auto& [ns, classMap] : namespaceClassToSize) {
        if (classMap.find(className) != classMap.end()) {
            if (foundSize == 0) {
                foundSize = classMap.at(className);
                foundNamespace = ns;
            } else {
                LOG_WARNING("Class %s found in multiple namespaces (%s, %s)", className.c_str(), foundNamespace.empty() ? "<global>" : foundNamespace.c_str(),
                            ns.empty() ? "<global>" : ns.c_str());
                foundMultiple = true;
            }
        }
    }

    if (foundSize > 0) {
        if (foundMultiple) {
            LOG_INFO("Using size %zu from namespace %s for ambiguous class %s", foundSize, foundNamespace.empty() ? "<global>" : foundNamespace.c_str(),
                     className.c_str());
        }
        return foundSize;
    }

    LOG_WARNING("Class %s%s%s not found or ambiguous, using default size", namespaceName.empty() ? "" : namespaceName.c_str(), namespaceName.empty() ? "" : ".",
                className.c_str());
    return 8;
}

bool MonoAPI::IsInternalOrExternalMethod(void* method) {
    if (!method || !m_mono_method_get_flags) {
        return false;
    }

    uint32_t iflags = 0;
    uint32_t flags = 0;

    try {
        flags = m_mono_method_get_flags(method, &iflags);
    } catch (...) {
        LOG_ERROR("Exception getting method flags");
        return false;
    }

    // Method attribute flags
    // 0x0800 = METHOD_ATTRIBUTE_PINVOKE_IMPL (external method)
    // 0x1000 = METHOD_ATTRIBUTE_ICALL (internal call)
    bool isExternal = (flags & 0x0800) != 0;
    bool isInternal = (flags & 0x1000) != 0;

    return isExternal || isInternal;
}

std::string MonoAPI::GetMethodSignature(void* method) {
    if (!method || !m_mono_method_get_name || !m_mono_method_signature) {
        return "unknown()";
    }

    const char* methodName = m_mono_method_get_name(method);
    void* signature = m_mono_method_signature(method);
    if (!signature) {
        return std::string(methodName) + "()";
    }

    void* returnType = m_mono_signature_get_return_type(signature);
    std::string returnTypeName = GetCppTypeFromMonoType(returnType);

    int paramCount = m_mono_signature_get_param_count(signature);
    std::string signatureStr = returnTypeName + " " + methodName + "(";

    void* iter = nullptr;
    for (int i = 0; i < paramCount; i++) {
        void* paramType = m_mono_signature_get_params(signature, &iter);
        std::string paramTypeName = GetCppTypeFromMonoType(paramType);

        signatureStr += paramTypeName;
        signatureStr += " param" + std::to_string(i);

        if (i < paramCount - 1) {
            signatureStr += ", ";
        }
    }

    signatureStr += ")";
    return signatureStr;
}

void MonoAPI::ExtractMethodInformation(void* klass, std::ofstream& file) {
    if (!klass || !m_mono_class_get_methods || !m_mono_method_get_name) {
        return;
    }

    const char* className = nullptr;
    try {
        className = m_mono_class_get_name(klass);
        if (!className)
            className = "UnknownClass";
    } catch (...) {
        LOG_ERROR("Exception getting class name");
        return;
    }

    LOG_INFO("Extracting methods for class: %s", className);

    file << "// Method signatures for " << className << std::endl;
    file << "/*" << std::endl;

    try {
        void* iter = nullptr;
        void* method = nullptr;

        while ((method = m_mono_class_get_methods(klass, &iter))) {
            try {
                const char* methodName = m_mono_method_get_name(method);
                if (!methodName)
                    methodName = "unnamed_method";

                std::string signature = GetMethodSignature(method);
                file << "    " << signature << ";";

                bool isNative = IsInternalOrExternalMethod(method);
                if (isNative) {
                    file << " // Native method (internal call or P/Invoke)";
                }
                file << std::endl;
            } catch (...) {
                LOG_ERROR("Exception processing method");
                file << "    // Error processing method" << std::endl;
            }
        }
    } catch (...) {
        LOG_ERROR("Exception in method extraction for %s", className);
        file << "    // Fatal error in method extraction" << std::endl;
    }

    file << "*/" << std::endl << std::endl;
}

void MonoAPI::GenerateStructFromClass(void* klass, std::ofstream& file, std::set<std::string>& requiredIncludes) {
    if (!klass || !m_mono_class_get_name || !m_mono_class_get_namespace || !m_mono_class_get_fields) {
        return;
    }

    const char* originalClassName = m_mono_class_get_name(klass);
    std::string className = CleanupFieldName(originalClassName);
    const char* namespaceName = m_mono_class_get_namespace(klass);
    std::string fullClassName;
    if (namespaceName && namespaceName[0] != '\0') {
        fullClassName = std::string(namespaceName) + "." + className;
    } else {
        fullClassName = std::string("<global>.") + className; // For logging
    }
    LOG_INFO("Generating struct for class: %s", fullClassName.c_str());

    std::vector<std::pair<std::string, std::string>> constants;            // type, name
    std::vector<std::pair<std::string, std::string>> staticFields;         // type, name
    std::vector<std::tuple<int, std::string, std::string>> instanceFields; // offset, type, name

    void* iter = NULL;
    void* field;

    while ((field = m_mono_class_get_fields(klass, &iter))) {
        const char* originalFieldName = m_mono_field_get_name(field);
        std::string fieldName = CleanupFieldName(originalFieldName);
        void* fieldType = m_mono_field_get_type(field);
        int offset = m_mono_field_get_offset(field);

        // Get type information
        int typeEnum = m_mono_type_get_type(fieldType);
        bool isReferenceType = (typeEnum == MONO_TYPE_CLASS || typeEnum == MONO_TYPE_STRING || typeEnum == MONO_TYPE_OBJECT || typeEnum == MONO_TYPE_ARRAY ||
                                typeEnum == MONO_TYPE_SZARRAY || typeEnum == MONO_TYPE_GENERICINST);
        bool isValueType = (typeEnum == MONO_TYPE_VALUETYPE);
        LOG_INFO("Field typeEnum: %d, Reference: %d, ValueType: %d", typeEnum, isReferenceType, isValueType);

        int flags = m_mono_field_get_flags(field);
        bool isStatic = (flags & 0x0010) != 0; // FIELD_ATTRIBUTE_STATIC = 0x0010

        // Check if this appears to be a constant (offset 0 and multiple fields share it)
        // This is a heuristic since const fields don't always have a distinguishing flag
        bool isConstant = false;
        if ((flags & 0x0040) != 0) { // FIELD_ATTRIBUTE_LITERAL = 0x0040
            isConstant = true;
        }

        // Try to get the actual class type
        void* typeClass = GetTypeClass(fieldType);
        std::string typeName;

        if (typeClass && typeEnum != MONO_TYPE_ARRAY) {
            const char* typeClassName = nullptr;
            bool classNameValid = false;
            try {
                typeClassName = m_mono_class_get_name(typeClass);
                classNameValid = typeClassName != nullptr;

                if (classNameValid) {
                    if (typeEnum == MONO_TYPE_VALUETYPE) {
                        typeName = std::string(typeClassName) + "_Value";

                        const char* typeNamespace = m_mono_class_get_namespace(typeClass);
                        LOG_INFO("Field %s is value type: %s.%s", fieldName.c_str(), typeNamespace ? typeNamespace : "", typeClassName);
                    } else if (isReferenceType) {
                        typeName = std::string(typeClassName) + "*";
                    } else {
                        typeName = typeClassName;
                    }

                    std::string typeAssemblyName = GetClassAssemblyName(typeClass);
                    std::string currentAssemblyName = GetClassAssemblyName(klass);

                    if (typeAssemblyName != currentAssemblyName) {
                        requiredIncludes.insert(typeAssemblyName);
                    }
                } else {
                    typeName = typeEnum == MONO_TYPE_VALUETYPE ? "/* Unknown value type */" : (isReferenceType ? "void*" : GetCppTypeFromMonoType(fieldType));
                }
            } catch (...) {
                LOG_INFO("Error getting type information for field %s", fieldName.c_str());
                typeName = typeEnum == MONO_TYPE_VALUETYPE ? "/* Unknown value type */" : (isReferenceType ? "void*" : GetCppTypeFromMonoType(fieldType));
            }
        } else {
            typeName = isReferenceType ? "void*" : GetCppTypeFromMonoType(fieldType);
        }

        if (isConstant) {
            constants.push_back({typeName, fieldName});
        } else if (isStatic) {
            staticFields.push_back({typeName, fieldName});
        } else {
            instanceFields.push_back(std::make_tuple(offset, typeName, fieldName));
        }

        // allFields.push_back(std::make_tuple(offset, typeName, fieldName));
        // offsetToFields[offset].push_back({typeName, fieldName});
    }

    // Sort instance fields by offset
    std::sort(instanceFields.begin(), instanceFields.end(), [](const auto& a, const auto& b) { return std::get<0>(a) < std::get<0>(b); });

    // Output the struct with only instance fields
    file << "// Generated from " << (namespaceName ? namespaceName : "") << "." << className << std::endl;
    file << "struct " << className << " {" << std::endl;

    // Add instance fields with padding
    size_t expectedOffset = 0;
    for (size_t i = 0; i < instanceFields.size(); i++) {
        int offset = std::get<0>(instanceFields[i]);
        std::string type = std::get<1>(instanceFields[i]);
        std::string name = std::get<2>(instanceFields[i]);

        // Add padding if needed
        if (offset > expectedOffset) {
            file << "    char padding" << i << "[" << (offset - expectedOffset) << "]; // Padding\n";
        }

        // Add the field
        file << "    " << type << " " << name << "; // Offset: " << offset << "\n";

        // Calculate expected offset for next field
        size_t fieldSize = 8; // Default size for reference types and 64bit values
        if (type == "void" || type == "int8_t" || type == "uint8_t" || type == "bool") {
            fieldSize = 1;
        } else if (type == "char16_t" || type == "int16_t" || type == "uint16_t") {
            fieldSize = 2;
        } else if (type == "int32_t" || type == "uint32_t" || type == "float" || type == "enum") {
            fieldSize = 4;
        } else if (type.find("_Value") != std::string::npos) {
            std::string baseClassName = type.substr(0, type.length() - 6);
            size_t valueTypeSize = GetClassSizeByName(baseClassName);
            if (valueTypeSize > 0) {
                fieldSize = valueTypeSize;
            }
        }

        expectedOffset = offset + fieldSize;
    }

    file << "};\n\n";

    // Output a separate static companion struct if there are any static fields or constants
    if (!staticFields.empty() || !constants.empty()) {
        file << "// Static companion for " << className << std::endl;
        file << "struct " << className << "_Static {" << std::endl;

        // Add static fields
        for (const auto& [type, name] : staticFields) {
            file << "    " << type << " " << name << ";" << std::endl;
        }

        // Add constants
        if (!constants.empty()) {
            file << "    // Constants" << std::endl;
            for (const auto& [type, name] : constants) {
                file << "    static const " << type << " " << name << ";" << std::endl;
            }
        }

        file << "};" << std::endl << std::endl;

        // Add initializers for constants
        if (!constants.empty()) {
            file << "// Constants initialization (for reference only)" << std::endl;
            for (const auto& [type, name] : constants) {
                file << "const " << type << " " << className << "_Static::" << name << " = /* value here */;" << std::endl;
            }
            file << std::endl;
        }
    }
}

void MonoAPI::DumpAllClassesToStructs(const std::string& outputDir) {
    void* domain = m_mono_get_root_domain();
    if (!domain) {
        LOG_ERROR("Failed to get mono domain");
        return;
    }
    if (!monoThread) {
        monoThread = m_mono_thread_attach(domain);
        if (!monoThread) {
            LOG_ERROR("Failed to attach thread to mono domain");
            return;
        } else {
            LOG_INFO("Thread attached to mono runtime: %p", monoThread);
        }
    }

    m_assemblies.clear();
    m_mono_domain_assembly_foreach(domain, (void (*)(void*, void*))(AssemblyIterationCallback), this);

    BuildClassSizeMap();

    // Maps to store assembly-specific details
    std::map<std::string, std::set<std::string>> assemblyDependencies; // assembly -> required assemblies
    std::map<std::string, std::vector<void*>> assemblyClasses;         // assembly -> classes

    // First pass: collect all classes by assembly
    for (auto image : m_assemblies) {
        const char* assemblyName = m_mono_image_get_name(image);

        const void* typeDefTable = m_mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int rows = m_mono_table_info_get_rows(typeDefTable);

        for (int j = 0; j < rows; j++) {
            void* klass = m_mono_class_get(image, j + 1 | MONO_TOKEN_TYPE_DEF);
            if (klass) {
                assemblyClasses[assemblyName].push_back(klass);
            }
        }
    }

    // Second pass: generate structs and track dependencies
    for (const auto& [assemblyName, classes] : assemblyClasses) {
        std::string headerPath = outputDir + "/" + assemblyName + ".h";
        std::ofstream file(headerPath);
        if (!file.is_open())
            continue;

        // Write header
        file << "#pragma once" << std::endl;
        file << "// Auto-generated C++ structs for " << assemblyName << std::endl;
        file << "#include <cstdint>" << std::endl;

        // Forward declarations for all classes in this assembly
        /*
        file << "// Forward declarations" << std::endl;
        for (void* klass : classes) {
            const char* originalClassName = m_mono_class_get_name(klass);
            std::string className = CleanupFieldName(originalClassName);
            file << "struct " << className << ";" << std::endl;
        }
        file << std::endl;
        */

        // Generate structs and track dependencies
        for (void* klass : classes) {
            std::set<std::string> requiredIncludes;
            GenerateStructFromClass(klass, file, requiredIncludes);
            ExtractMethodInformation(klass, file);

            // Add dependencies to the assembly
            for (const auto& include : requiredIncludes) {
                assemblyDependencies[assemblyName].insert(include);
            }
        }

        file.close();
    }

    // Third pass: create a master header file that manages dependencies
    /*
    std::ofstream masterFile(outputDir + "/AllStructs.h");
    masterFile << "#pragma once" << std::endl;
    masterFile << "// Master include file for all generated structs" << std::endl << std::endl;

    // Include all headers
    for (const auto& [assemblyName, _] : assemblyClasses) {
        masterFile << "#include \"" << assemblyName << ".h\"" << std::endl;
    }

    masterFile.close();
    */
}
