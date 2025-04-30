#pragma once

// MonoTypes.h - Common type definitions for Mono interaction

// Mono basic types
typedef void* MonoDomain;
typedef void* MonoAssembly;
typedef void* MonoImage;
typedef void* MonoClass;
typedef void* MonoMethod;
typedef void* MonoObject;
typedef void* MonoString;
typedef void* MonoArray;
typedef void* MonoField;
typedef void* MonoProperty;
typedef void* MonoThread;
typedef void* MonoVTable;
typedef void* MonoMethodSignature;
typedef void* MonoType;

// Mono function pointer types
typedef MonoDomain* (__cdecl *mono_get_root_domain_t)();
typedef MonoVTable* (__cdecl *mono_class_vtable_t)(MonoDomain* domain, MonoClass* klass);
typedef void (__cdecl *mono_domain_assembly_foreach_t)(MonoDomain* domain, void(*func)(void* assembly, void* user_data), void* user_data);
typedef MonoImage* (__cdecl *mono_assembly_get_image_t)(MonoAssembly* assembly);
typedef const char* (__cdecl *mono_image_get_name_t)(MonoImage* image);
typedef MonoClass* (__cdecl *mono_class_from_name_t)(MonoImage* image, const char* name_space, const char* name);
typedef MonoMethod* (__cdecl *mono_class_get_method_from_name_t)(MonoClass* klass, const char* name, int param_count);
typedef MonoObject* (__cdecl *mono_runtime_invoke_t)(MonoMethod* method, void* obj, void** params, MonoObject** exc);
typedef MonoField* (__cdecl *mono_class_get_field_from_name_t)(MonoClass* klass, const char* name);
typedef void (__cdecl *mono_field_get_value_t)(MonoObject* obj, MonoField* field, void* value);
typedef void (__cdecl *mono_field_static_get_value_t)(MonoVTable* vt, MonoField* field, void* value);
typedef MonoString* (__cdecl *mono_string_new_t)(MonoDomain* domain, const char* text);
typedef char* (__cdecl *mono_string_to_utf8_t)(MonoString* string_obj);
typedef void (__cdecl *mono_free_t)(void* ptr);
typedef MonoProperty* (__cdecl *mono_class_get_property_from_name_t)(MonoClass* klass, const char* name);
typedef MonoMethod* (__cdecl *mono_property_get_get_method_t)(MonoProperty* prop);
typedef MonoMethod* (__cdecl *mono_property_get_set_method_t)(MonoProperty* prop);
typedef MonoThread* (__cdecl *mono_thread_attach_t)(MonoDomain* domain);
typedef void (__cdecl *mono_thread_detach_t)(MonoThread* thread);
typedef MonoClass* (__cdecl *mono_object_get_class_t)(MonoObject* obj);
typedef void* (__cdecl *mono_compile_method_t)(MonoMethod* method);
typedef MonoMethod* (__cdecl *mono_class_get_methods_t)(MonoClass* klass, void** iter);
typedef const char* (__cdecl *mono_method_get_name_t)(MonoMethod* method);
typedef MonoMethodSignature* (__cdecl *mono_method_signature_t)(MonoMethod* method);
typedef int (__cdecl *mono_signature_get_param_count_t)(MonoMethodSignature* sig);
typedef MonoType* (__cdecl *mono_signature_get_return_type_t)(MonoMethodSignature* sig);
typedef const char* (__cdecl *mono_type_get_name_t)(MonoType* type);
typedef MonoType* (__cdecl *mono_signature_get_params_t)(MonoMethodSignature* sig, void** iter);
typedef void* (__cdecl *mono_object_unbox_t)(MonoObject* obj);
typedef int (__cdecl *mono_field_get_offset_t)(MonoField* field);
typedef MonoField* (__cdecl *mono_class_get_fields_t)(MonoClass* klass, void** iter);
typedef const char* (__cdecl *mono_field_get_name_t)(MonoField* field);
typedef int (*mono_array_length_t)(MonoArray* array);
typedef void* (*mono_lookup_internal_call_t)(MonoMethod* method);