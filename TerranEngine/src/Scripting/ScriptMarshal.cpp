#include "trpch.h"
#include "ScriptMarshal.h"
#include "ScriptCache.h"
#include "ManagedClass.h"

#include "Utils/Debug/OptickProfiler.h"

#include <mono/metadata/appdomain.h>

namespace TerranEngine 
{
	template<typename T>
	static T Unbox(MonoObject* monoObject)
	{
		TR_PROFILE_FUNCTION();
		return *(T*)mono_object_unbox(monoObject);
	}

	template<typename T>
	static T* UnboxAddr(MonoObject* monoObject)
	{
		TR_PROFILE_FUNCTION();
		return (T*)mono_object_unbox(monoObject);
	}
	
	MonoArray* ScriptMarshal::UUIDToMonoArray(const UUID& id)
	{
		TR_PROFILE_FUNCTION();
		const uint8_t* idData = id.GetRaw();
		MonoArray* uuidArray = mono_array_new(mono_domain_get(), ScriptCache::GetCachedClassFromName("System.Byte")->GetMonoClass(), 16);
		constexpr uint32_t uuidArrayLength = 16;
		mono_value_copy_array(uuidArray, 0, (void*)idData, 16);

		return uuidArray;
	}

	UUID ScriptMarshal::MonoArrayToUUID(MonoArray* id)
	{
		TR_PROFILE_FUNCTION();
		if (mono_array_length(id) != 16)
		{
			TR_ERROR("Mono UUID array is invalid");
			return UUID({ 0 });
		}

		std::array<uint8_t, 16> idData = { 0 };

		const uint8_t* uuidArrayAddr = mono_array_addr(id, uint8_t, 0);
		memcpy(&idData, uuidArrayAddr, 16 * sizeof(uint8_t));

		return UUID(idData);
	}

	MonoString* ScriptMarshal::UTF8ToMonoString(const std::string& str)
	{
		TR_PROFILE_FUNCTION();
		MonoDomain* mainDomain = mono_domain_get();
		return mono_string_new(mainDomain, str.c_str());
	}

	std::string ScriptMarshal::MonoStringToUTF8(MonoString* monoStr)
	{
		TR_PROFILE_FUNCTION();
		MonoError error;
		char* str = mono_string_to_utf8_checked(monoStr, &error);

		if (!str)
			return "";
		
		if (error.error_code != MONO_ERROR_NONE) 
		{
			TR_ERROR("Mono Error ID: {0}; Message {1}", mono_error_get_error_code(&error), mono_error_get_message(&error));
			return "";
		}

		std::string result(str);
		mono_free((char*)str);

		return result;
	}

	Utils::Variant ScriptMarshal::MonoObjectToVariant(MonoObject* monoObject, const ManagedType& type)
	{
		TR_PROFILE_FUNCTION();
		switch (type.GetNativeType())
		{
		case NativeType::Bool: 		return Unbox<bool>(monoObject);
		case NativeType::Char: 		return (char)Unbox<wchar_t>(monoObject);
		case NativeType::Int8: 		return Unbox<int8_t>(monoObject);
		case NativeType::Int16: 	return Unbox<int16_t>(monoObject);
		case NativeType::Int32: 	return Unbox<int32_t>(monoObject);
		case NativeType::Int64: 	return Unbox<int64_t>(monoObject);
		case NativeType::UInt8: 	return Unbox<uint8_t>(monoObject);
		case NativeType::UInt16: 	return Unbox<uint16_t>(monoObject);
		case NativeType::UInt32: 	return Unbox<uint32_t>(monoObject);
		case NativeType::UInt64: 	return Unbox<uint64_t>(monoObject);
		case NativeType::Float:		return Unbox<float>(monoObject);
		case NativeType::Double: 	return Unbox<double>(monoObject);
		case NativeType::String: 	return MonoStringToUTF8((MonoString*)monoObject);
		case NativeType::Vector2:	return *UnboxAddr<glm::vec2>(monoObject);
		case NativeType::Vector3:	return *UnboxAddr<glm::vec3>(monoObject);
		case NativeType::Color:		return *UnboxAddr<glm::vec4>(monoObject);
		case NativeType::Entity:	break;
		}	

		return { };
	}

	Utils::Variant::Type ScriptMarshal::ScriptTypeToVariantType(const ManagedType& type)
	{
		TR_PROFILE_FUNCTION();
		using namespace Utils;
		switch (type.GetNativeType())
		{
		case NativeType::Bool: 	return Variant::Type::Bool;
		case NativeType::Char: 	return Variant::Type::Char;
		case NativeType::Int8: 	return Variant::Type::Int8;
		case NativeType::Int16: 	return Variant::Type::Int16;
		case NativeType::Int32: 	return Variant::Type::Int32;
		case NativeType::Int64: 	return Variant::Type::Int64;
		case NativeType::UInt8: 	return Variant::Type::UInt8;
		case NativeType::UInt16: 	return Variant::Type::UInt16;
		case NativeType::UInt32: 	return Variant::Type::UInt32;
		case NativeType::UInt64: 	return Variant::Type::UInt64;
		case NativeType::Float:	return Variant::Type::Float;
		case NativeType::Double: 	return Variant::Type::Double;
		case NativeType::String: 	return Variant::Type::String;
		case NativeType::Vector2: 	return Variant::Type::Vector2;
		case NativeType::Vector3: 	return Variant::Type::Vector3;
		case NativeType::Color:	return Variant::Type::Vector4;
		case NativeType::Entity:
		case NativeType::UUID:		return Variant::Type::UUID;
		}

		return Variant::Type::None;
	}
}
