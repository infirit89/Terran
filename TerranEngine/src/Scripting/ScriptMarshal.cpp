#include "trpch.h"
#include "ScriptMarshal.h"
#include "ScriptCache.h"
#include "ScriptClass.h"

#include <mono/metadata/appdomain.h>

namespace TerranEngine 
{
	template<typename T>
	static T Unbox(MonoObject* monoObject)
	{
		return *(T*)mono_object_unbox(monoObject);
	}

	template<typename T>
	static T* UnboxAddr(MonoObject* monoObject)
	{
		return (T*)mono_object_unbox(monoObject);
	}
	
	ScriptArray ScriptMarshal::UUIDToMonoArray(const UUID& id)
	{
		const uint8_t* idData = id.GetRaw();
		ScriptArray uuidArray = ScriptArray::Create<uint8_t>(16);

		constexpr uint32_t uuidArrayLength = 16 * sizeof(uint8_t); 
		
		memcpy((uint8_t*)uuidArray.GetElementAddress(0, sizeof(uint8_t)), idData, uuidArrayLength);

		return uuidArray;
	}

	UUID ScriptMarshal::MonoArrayToUUID(ScriptArray uuidArray)
	{
		if (uuidArray.Length() != 16)
		{
			TR_ERROR("Mono UUID array is invalid");
			return UUID({ 0 });
		}

		std::array<uint8_t, 16> idData = { 0 };

		const uint8_t* uuidArrayAddr = (uint8_t*)uuidArray.GetElementAddress(0, sizeof(uint8_t));

		memcpy(&idData, uuidArrayAddr, 16 * sizeof(uint8_t));

		return UUID(idData);
	}

	MonoString* ScriptMarshal::UTF8ToMonoString(const std::string& str)
	{
		MonoDomain* mainDomain = mono_domain_get();
		return mono_string_new(mainDomain, str.c_str());
	}

	std::string ScriptMarshal::MonoStringToUTF8(MonoString* monoStr)
	{
		MonoError error;
		char* str = mono_string_to_utf8_checked(monoStr, &error);

		if (error.error_code != MONO_ERROR_NONE) 
		{
			TR_ERROR("Mono Error ID: {0}; Message {1}", mono_error_get_error_code(&error), mono_error_get_message(&error));
			return "";
		}

		std::string result(str);
		mono_free((char*)str);

		return result;
	}

	Utils::Variant ScriptMarshal::MonoObjectToVariant(MonoObject* monoObject, const ScriptType& type)
	{
		switch (type.TypeEnum)
		{
		case ScriptType::Bool: 		return Unbox<bool>(monoObject);
		case ScriptType::Char: 		return (char)Unbox<wchar_t>(monoObject);
		case ScriptType::Int8: 		return Unbox<int8_t>(monoObject);
		case ScriptType::Int16: 	return Unbox<int16_t>(monoObject);
		case ScriptType::Int32: 	return Unbox<int32_t>(monoObject);
		case ScriptType::Int64: 	return Unbox<int64_t>(monoObject);
		case ScriptType::UInt8: 	return Unbox<uint8_t>(monoObject);
		case ScriptType::UInt16: 	return Unbox<uint16_t>(monoObject);
		case ScriptType::UInt32: 	return Unbox<uint32_t>(monoObject);
		case ScriptType::UInt64: 	return Unbox<uint64_t>(monoObject);
		case ScriptType::Float:		return Unbox<float>(monoObject);
		case ScriptType::Double: 	return Unbox<double>(monoObject);
		case ScriptType::String: 	return MonoStringToUTF8((MonoString*)monoObject);
		case ScriptType::Vector2:	return *UnboxAddr<glm::vec2>(monoObject);
		case ScriptType::Vector3:	return *UnboxAddr<glm::vec3>(monoObject);
		case ScriptType::Color:		return *UnboxAddr<glm::vec4>(monoObject);
		case ScriptType::Entity: break;
		}	

		return { };
	}

	Utils::Variant::Type ScriptMarshal::ScriptTypeToVariantType(const ScriptType& type)
	{
		using namespace Utils;
		switch (type.TypeEnum)
		{
		case ScriptType::Bool: 		return Variant::Type::Bool;
		case ScriptType::Char: 		return Variant::Type::Char;
		case ScriptType::Int8: 		return Variant::Type::Int8;
		case ScriptType::Int16: 	return Variant::Type::Int16;
		case ScriptType::Int32: 	return Variant::Type::Int32;
		case ScriptType::Int64: 	return Variant::Type::Int64;
		case ScriptType::UInt8: 	return Variant::Type::UInt8;
		case ScriptType::UInt16: 	return Variant::Type::UInt16;
		case ScriptType::UInt32: 	return Variant::Type::UInt32;
		case ScriptType::UInt64: 	return Variant::Type::UInt64;
		case ScriptType::Float:		return Variant::Type::Float;
		case ScriptType::Double: 	return Variant::Type::Double;
		case ScriptType::String: 	return Variant::Type::String;
		case ScriptType::Vector2: 	return Variant::Type::Vector2;
		case ScriptType::Vector3: 	return Variant::Type::Vector3;
		case ScriptType::Color:		return Variant::Type::Vector4;
		case ScriptType::Entity:
		case ScriptType::UUID:
			return Variant::Type::UUID;
		}

		return Variant::Type::None;
	}
}
