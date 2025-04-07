#include "MonoApi.h"
#include <iostream>
#include <globals/globals.h>
#include <map>

MonoAPI::~MonoAPI() {
    if (monoModule) {
        FreeLibrary(monoModule);
    }
}

bool MonoAPI::Initialize(const std::string& monoDllPath) {
    monoModule = GetModuleHandleA(monoDllPath.c_str());
    if (!monoModule) {
        G::logger.Log("Failed to get %s handle", monoDllPath.c_str());
        return false;
    } else {
        G::logger.Log("%s base address: 0x%p", monoDllPath.c_str(), monoModule);
    }

    // Load all function pointers
    m_mono_get_root_domain = (mono_get_root_domain_fn)GetProcAddress(monoModule, "mono_get_root_domain");
    m_mono_domain_assembly_open = (mono_domain_assembly_open_fn)GetProcAddress(monoModule, "mono_domain_assembly_open");
    m_mono_assembly_get_image = (mono_assembly_get_image_fn)GetProcAddress(monoModule, "mono_assembly_get_image");
    m_mono_class_from_name = (mono_class_from_name_fn)GetProcAddress(monoModule, "mono_class_from_name");
    m_mono_class_get_fields = (mono_class_get_fields_fn)GetProcAddress(monoModule, "mono_class_get_fields");
    m_mono_field_get_name = (mono_field_get_name_fn)GetProcAddress(monoModule, "mono_field_get_name");
    m_mono_field_get_type = (mono_field_get_type_fn)GetProcAddress(monoModule, "mono_field_get_type");
    m_mono_field_get_offset = (mono_field_get_offset_fn)GetProcAddress(monoModule, "mono_field_get_offset");
    m_mono_type_get_type = (mono_type_get_type_fn)GetProcAddress(monoModule, "mono_type_get_type");
    m_mono_domain_assembly_foreach = (mono_domain_assembly_foreach_fn)GetProcAddress(monoModule, "mono_domain_assembly_foreach");
    m_mono_image_get_name = (mono_image_get_name_fn)GetProcAddress(monoModule, "mono_image_get_name");
    m_mono_image_get_table_info = (mono_image_get_table_info_fn)GetProcAddress(monoModule, "mono_image_get_table_info");
    m_mono_table_info_get_rows = (mono_table_info_get_rows_fn)GetProcAddress(monoModule, "mono_table_info_get_rows");
    m_mono_class_get = (mono_class_get_fn)GetProcAddress(monoModule, "mono_class_get");
    m_mono_class_get_name = (mono_class_get_name_fn)GetProcAddress(monoModule, "mono_class_get_name");
    m_mono_class_get_namespace = (mono_class_get_namespace_fn)GetProcAddress(monoModule, "mono_class_get_namespace");
    m_mono_type_get_class = (mono_type_get_class_fn)GetProcAddress(monoModule, "mono_type_get_class");
    m_mono_class_get_image = (mono_class_get_image_fn)GetProcAddress(monoModule, "mono_class_get_image");
    m_mono_field_get_flags = (mono_field_get_flags_fn)GetProcAddress(monoModule, "mono_field_get_flags");
    m_mono_thread_attach = (mono_thread_attach_fn)GetProcAddress(monoModule, "mono_thread_attach");

    G::logger.Log("mono_get_root_domain: 0x%p", m_mono_get_root_domain);
    G::logger.Log("mono_domain_assembly_open: 0x%p", m_mono_domain_assembly_open);
    G::logger.Log("mono_assembly_get_image: 0x%p", m_mono_assembly_get_image);
    G::logger.Log("mono_class_from_name: 0x%p", m_mono_class_from_name);
    G::logger.Log("mono_class_get_fields: 0x%p", m_mono_class_get_fields);
    G::logger.Log("mono_field_get_name: 0x%p", m_mono_field_get_name);
    G::logger.Log("mono_field_get_type: 0x%p", m_mono_field_get_type);
    G::logger.Log("mono_field_get_offset: 0x%p", m_mono_field_get_offset);
    G::logger.Log("mono_type_get_type: 0x%p", m_mono_type_get_type);
    G::logger.Log("mono_domain_assembly_foreach: 0x%p", m_mono_domain_assembly_foreach);
    G::logger.Log("mono_image_get_name: 0x%p", m_mono_image_get_name);
    G::logger.Log("mono_image_get_table_info: 0x%p", m_mono_image_get_table_info);
    G::logger.Log("mono_table_info_get_rows: 0x%p", m_mono_table_info_get_rows);
    G::logger.Log("mono_class_get: 0x%p", m_mono_class_get);
    G::logger.Log("mono_class_get_name: 0x%p", m_mono_class_get_name);
    G::logger.Log("mono_class_get_namespace: 0x%p", m_mono_class_get_namespace);
    G::logger.Log("mono_type_get_class: 0x%p", m_mono_type_get_class);
    G::logger.Log("mono_class_get_image: 0x%p", m_mono_class_get_image);
    G::logger.Log("mono_field_get_flags: 0x%p", m_mono_field_get_flags);
    G::logger.Log("mono_thread_attach: 0x%p", m_mono_thread_attach);

    // Check if all required functions were loaded
    return m_mono_get_root_domain && m_mono_domain_assembly_open && m_mono_assembly_get_image && 
           m_mono_class_from_name && m_mono_class_get_fields && m_mono_field_get_name && 
           m_mono_field_get_type && m_mono_field_get_offset && m_mono_type_get_type &&
           m_mono_domain_assembly_foreach && m_mono_image_get_name && m_mono_image_get_table_info &&
           m_mono_table_info_get_rows && m_mono_class_get && m_mono_class_get_name && 
           m_mono_class_get_namespace && m_mono_type_get_class && m_mono_class_get_image &&
           m_mono_field_get_flags && m_mono_thread_attach;
}

void __cdecl MonoAPI::AssemblyIterationCallback(void* assembly, void* user_data) {
    auto* self = static_cast<MonoAPI*>(user_data);
    void* image = self->m_mono_assembly_get_image(assembly);
    self->m_assemblies.push_back(image);

    const char* imageName = self->m_mono_image_get_name(image);
    G::logger.Log("Found assembly: " + std::string(imageName));
}

std::string MonoAPI::GetCppTypeFromMonoType(void* type) {
    if (!type || !m_mono_type_get_type) return "void*";
    
    int type_enum = m_mono_type_get_type(type);

    switch (type_enum) {
        case MONO_TYPE_VOID: return "void";
        case MONO_TYPE_BOOLEAN: return "bool";
        case MONO_TYPE_CHAR: return "char16_t";
        case MONO_TYPE_I1: return "int8_t";
        case MONO_TYPE_U1: return "uint8_t";
        case MONO_TYPE_I2: return "int16_t";
        case MONO_TYPE_U2: return "uint16_t";
        case MONO_TYPE_I4: return "int32_t";
        case MONO_TYPE_U4: return "uint32_t";
        case MONO_TYPE_I8: return "int64_t";
        case MONO_TYPE_U8: return "uint64_t";
        case MONO_TYPE_R4: return "float";
        case MONO_TYPE_R8: return "double";
        case MONO_TYPE_STRING: return "MonoString*";
        case MONO_TYPE_PTR: return "void*"; // Need context for specific type
        case MONO_TYPE_BYREF: return "void*"; // Need context for specific type
        case MONO_TYPE_VALUETYPE: return "MONO_TYPE_VALUETYPE"; // Need context for specific type
        case MONO_TYPE_CLASS: return "void*"; // Need context for specific name
        case MONO_TYPE_ARRAY: return "MonoArray*";
        case MONO_TYPE_SZARRAY: return "MonoArray*";
        case MONO_TYPE_I: return "intptr_t";
        case MONO_TYPE_U: return "uintptr_t";
        case MONO_TYPE_OBJECT: return "MonoObject*";
        case MONO_TYPE_ENUM: return "enum"; // Need context for specific type
        case MONO_TYPE_END: return "MONO_TYPE_END"; // Not a real type, end marker
        case MONO_TYPE_VAR: return "MONO_TYPE_VAR"; // Need context
        case MONO_TYPE_GENERICINST: return "MONO_TYPE_GENERICINST"; // Need type args
        case MONO_TYPE_TYPEDBYREF: return "MONO_TYPE_TYPEDBYREF"; // Custom struct
        case MONO_TYPE_FNPTR: return "MONO_TYPE_FNPTR"; // Generic function pointer, needs signature
        default: return "/* Unknown type */";
    }
}

std::string CleanupFieldName(const std::string& fieldName) {
    std::string result = fieldName;
    
    // Look for pattern <name>k__BackingField and clean it up
    size_t startBracket = result.find('<');
    size_t endBracket = result.find('>');
    size_t backing = result.find("k__BackingField");
    
    if (startBracket != std::string::npos && endBracket != std::string::npos && 
        backing != std::string::npos && startBracket < endBracket && endBracket < backing) {
        // Extract the name between brackets
        std::string propertyName = result.substr(startBracket + 1, endBracket - startBracket - 1);
        // Replace with propertyName_backing
        result = propertyName + "_backing";
    }
    
    return result;
}

void MonoAPI::CatalogClasses() {
    void* domain = m_mono_get_root_domain();
    if (!domain) return;
    
    m_assemblies.clear();
    m_mono_domain_assembly_foreach(domain, AssemblyIterationCallback, this);
    G::logger.Log("Found %d assemblies", m_assemblies.size());

    for (auto assembly : m_assemblies) {
        const void* typeDefTable = m_mono_image_get_table_info(
            assembly, MONO_TABLE_TYPEDEF);
        
        int rows = m_mono_table_info_get_rows(typeDefTable);
        
        for (int j = 0; j < rows; j++) {
            void* klass = m_mono_class_get(assembly, j + 1 | MONO_TOKEN_TYPE_DEF);
            if (klass) {
                const char* className = m_mono_class_get_name(klass);
                const char* namespaceName = m_mono_class_get_namespace(klass);
                std::string fullName = std::string(namespaceName ? namespaceName : "") + 
                                        "." + std::string(className);
                
                classNameMap[klass] = fullName;
            }
        }
    }
}

void* MonoAPI::GetTypeClass(void* type) {
    if (!type || !m_mono_type_get_class) {
        return nullptr;
    }
    
    int typeEnum = m_mono_type_get_type(type);
    if (typeEnum != MONO_TYPE_CLASS && 
        typeEnum != MONO_TYPE_STRING && 
        typeEnum != MONO_TYPE_OBJECT && 
        typeEnum != MONO_TYPE_ARRAY && 
        typeEnum != MONO_TYPE_SZARRAY &&
        typeEnum != MONO_TYPE_GENERICINST) {
        // Not a class type
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

void MonoAPI::GenerateStructFromClass(void* klass, std::ofstream& file, std::set<std::string>& requiredIncludes) {
    if (!klass || !m_mono_class_get_name || !m_mono_class_get_namespace || !m_mono_class_get_fields) {
        return;
    }

    const char* originalClassName = m_mono_class_get_name(klass);
    std::string className = CleanupFieldName(originalClassName);
    const char* namespaceName = m_mono_class_get_namespace(klass);
    std::string fullClassName = std::string(namespaceName ? namespaceName : "") + "." + className;
    G::logger.Log("Generating struct for class: %s", fullClassName.c_str());
    
    //file << "// Generated from " << fullClassName << std::endl;
    //file << "struct " << className << " {" << std::endl;
    
    // First collect all fields to analyze them
    //std::vector<std::tuple<int, std::string, std::string>> allFields; // offset, type, name
    //std::map<int, std::vector<std::pair<std::string, std::string>>> offsetToFields; // offset -> [(type, name)]
    std::vector<std::pair<std::string, std::string>> constants; // type, name
    std::vector<std::pair<std::string, std::string>> staticFields; // type, name
    std::vector<std::tuple<int, std::string, std::string>> instanceFields; // offset, type, name
    
    void* iter = NULL;
    void* field;
    
    while ((field = m_mono_class_get_fields(klass, &iter))) {
        const char* originalFieldName = m_mono_field_get_name(field);
        std::string fieldName = CleanupFieldName(originalFieldName);
        void* fieldType = m_mono_field_get_type(field);
        int offset = m_mono_field_get_offset(field);
        G::logger.Log("Field: %s, Offset: %d", fieldName.c_str(), offset);
        
        // Get type information
        int typeEnum = m_mono_type_get_type(fieldType);
        bool isReferenceType = (typeEnum == MONO_TYPE_CLASS || 
                               typeEnum == MONO_TYPE_STRING || 
                               typeEnum == MONO_TYPE_OBJECT || 
                               typeEnum == MONO_TYPE_ARRAY || 
                               typeEnum == MONO_TYPE_SZARRAY ||
                               typeEnum == MONO_TYPE_GENERICINST);
        G::logger.Log("Field typeEnum: %d, Reference: %d", typeEnum, isReferenceType);

        // Check if this is a static field
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
        G::logger.Log("Field typeClass: 0x%p", typeClass);
        std::string typeName;
        
        if (typeClass && typeEnum != MONO_TYPE_ARRAY) {
            G::logger.Log("Field typeClass found");
            const char* typeClassName = nullptr;
            bool classNameValid = false;
            try {
                typeClassName = m_mono_class_get_name(typeClass);
                classNameValid = true;
                G::logger.Log("Field typeClass name: %s", typeClassName);
                const char* typeNamespace = m_mono_class_get_namespace(typeClass);
                G::logger.Log("Field typeNamespace: %s", typeNamespace);
                std::string fullTypeName = std::string(typeNamespace ? typeNamespace : "") + "." + typeClassName;
                G::logger.Log("Field fullTypeName: %s", fullTypeName);
            } catch (...) {
                typeClassName = nullptr;
                G::logger.Log("Failed to get type class name");
            }
            
            // If this is a class we know about, use it directly
            if (typeClassName && classNameMap.count(typeClass)) {
                typeName = typeClassName;
                
                // Add to required includes if it's from a different assembly
                std::string typeAssemblyName = GetClassAssemblyName(typeClass);
                G::logger.Log("Field type assembly: %s", typeAssemblyName);
                std::string currentAssemblyName = GetClassAssemblyName(klass);
                G::logger.Log("Current assembly: %s", currentAssemblyName);
                
                if (typeAssemblyName != currentAssemblyName) {
                    requiredIncludes.insert(typeAssemblyName);
                }
                
                // Reference types should be pointers
                if (isReferenceType) {
                    typeName += "*";
                }
            } else {
                // Fall back to void* for unknown reference types
                typeName = isReferenceType ? "void*" : GetCppTypeFromMonoType(fieldType);
            }
        } else {
            // Use primitive type or void* for reference types
            typeName = isReferenceType ? "void*" : GetCppTypeFromMonoType(fieldType);
        }

        if (isConstant) {
            constants.push_back({typeName, fieldName});
        } else if (isStatic) {
            staticFields.push_back({typeName, fieldName});
        } else {
            instanceFields.push_back(std::make_tuple(offset, typeName, fieldName));
        }
        
        //allFields.push_back(std::make_tuple(offset, typeName, fieldName));
        //offsetToFields[offset].push_back({typeName, fieldName});
    }
    
    // Sort instance fields by offset
    std::sort(instanceFields.begin(), instanceFields.end(),
              [](const auto& a, const auto& b) { return std::get<0>(a) < std::get<0>(b); });
    
    // Output the struct with only instance fields
    file << "// Generated from " << (namespaceName ? namespaceName : "") 
         << "." << className << std::endl;
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
                file << "const " << type << " " << className << "_Static::" << name 
                     << " = /* value here */;" << std::endl;
            }
            file << std::endl;
        }
    }

}

void MonoAPI::DumpAllClassesToStructs(const std::string& outputDir) {
    void* domain = m_mono_get_root_domain();
    if (!domain) {
        G::logger.Log("Failed to get mono domain");
        return;
    }
    if (!monoThread) {
        monoThread = m_mono_thread_attach(domain);
        if (!monoThread) {
            G::logger.Log("Failed to attach thread to mono domain");
            return;
        } else {
            G::logger.Log("Thread attached to mono runtime: %p", monoThread);
        }
    }

    CatalogClasses();

    // Maps to store assembly-specific details
    std::map<std::string, std::set<std::string>> assemblyDependencies; // assembly -> required assemblies
    std::map<std::string, std::vector<void*>> assemblyClasses; // assembly -> classes
    
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
        if (!file.is_open()) continue;
        
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