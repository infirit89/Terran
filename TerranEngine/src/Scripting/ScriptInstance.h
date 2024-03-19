#pragma once

#include "ScriptType.h"

#include "Scene/Entity.h"

#include "Utils/Variant.h"

#include <vector>
#include <string>

namespace Coral 
{
	class Type;
	class FieldInfo;
}

namespace TerranEngine 
{
	struct ScriptField
	{
		ScriptType Type;
		std::string Name;
		bool IsArray;
	};

	struct ScriptObject 
	{
		const void* Handle;
		int32_t Padding[2];
	};

	struct ScriptArray
	{
		const void* Handle;
		int32_t Rank;
		int32_t FieldHandle;
	};

	template<typename TArg, size_t TIndex>
	void AddToIndexArrayI(int32_t* indicesArray, TArg value)
	{
		indicesArray[TIndex] = value;
	}

	template<typename... TArgs, size_t... TIndices>
	void AddToIndexArray(int32_t* indicesArray, TArgs&&... args, const std::index_sequence<TIndices...>&)
	{
		(AddToIndexArrayI<TArgs, TIndices>(indicesArray, std::forward<TArgs>(args)), ...);
	}

	template<typename... TIndices>
	concept indices_concept = sizeof...(TIndices) > 0;

	class ScriptInstance 
	{
	public:
		ScriptInstance(Coral::Type& type, Entity entity);
		~ScriptInstance();

		template<typename TValue>
		void SetFieldValue(int32_t fieldHandle, const TValue& value) const
		{
			SetFieldValueInternal(fieldHandle, (void*)&value);
		}

		template<>
		void SetFieldValue<std::string>(int32_t fieldHandle, const std::string& value) const;

		template<>
		void SetFieldValue<UUID>(int32_t fieldHandle, const UUID& value) const;

		template<>
		void SetFieldValue<Utils::Variant>(int32_t fieldHandle, const Utils::Variant& value) const;

		template<typename TReturn>
		TReturn GetFieldValue(int32_t fieldHandle) const 
		{
			TReturn value;
			GetFieldValueInternal(fieldHandle, &value);
			return value;
		}

		template<>
		std::string GetFieldValue<std::string>(int32_t fieldHandle) const;

		template<>
		UUID GetFieldValue<UUID>(int32_t fieldHandle) const;

		template<>
		Utils::Variant GetFieldValue<Utils::Variant>(int32_t fieldHandle) const;

		template<typename TValue, std::same_as<int32_t>... TIndices>
			requires indices_concept<TIndices...>
		void SetFieldArrayValue(const ScriptArray& array, const TValue& value, TIndices... indices) 
		{
			constexpr size_t indicesSize = sizeof...(indices);
			int32_t indicesArray[indicesSize]{};
			AddToIndexArray<TIndices...>(indicesArray, std::forward<TIndices>(indices)..., std::make_index_sequence<indicesSize>{});
			SetFieldArrayValue(array, value, indicesArray, indicesSize);
		}

		template<typename TValue>
		void SetFieldArrayValue(const ScriptArray& array, const TValue& value, const int32_t* indices, size_t indicesSize) 
		{
			SetFieldArrayValueInternal(array, (void*)&value, indices, indicesSize);
		}

		template<>
		void SetFieldArrayValue<std::string>(const ScriptArray& array, const std::string& value, const int32_t* indices, size_t indicesSize);

		template<>
		void SetFieldArrayValue<UUID>(const ScriptArray& array, const UUID& value, const int32_t* indices, size_t indicesSize);

		template<>
		void SetFieldArrayValue<Utils::Variant>(const ScriptArray& array, const Utils::Variant& value, const int32_t* indices, size_t indicesSize);

		template<typename TReturn, std::same_as<int32_t>... TIndices>
			requires indices_concept<TIndices...>
		TReturn GetFieldArrayValue(const ScriptArray& array, TIndices... indices)
		{
			constexpr size_t indicesSize = sizeof...(indices);
			int32_t indicesArray[indicesSize]{};
			AddToIndexArray<TIndices...>(indicesArray, std::forward<TIndices>(indices)..., std::make_index_sequence<indicesSize>{});

			return GetFieldArrayValue<TReturn>(array, indicesArray, indicesSize);
		}

		template<typename TReturn>
		TReturn GetFieldArrayValue(const ScriptArray& array, const int32_t* indices, size_t indicesSize) 
		{
			TReturn value;
			GetFieldArrayValueInternal(array, &value, indices, indicesSize);
			return value;
		}

		template<>
		std::string GetFieldArrayValue(const ScriptArray& array, const int32_t* indices, size_t indicesSize);

		template<>
		UUID GetFieldArrayValue(const ScriptArray& array, const int32_t* indices, size_t indicesSize);

		template<>
		Utils::Variant GetFieldArrayValue(const ScriptArray& array, const int32_t* indices, size_t indicesSize);

		template<std::same_as<int32_t>... TLengths>
			requires indices_concept<TLengths...>
		void ResizeFieldArray(ScriptArray& array, TLengths... lengths) 
		{
			constexpr size_t lengthsSize = sizeof...(lengths);
			int32_t lengthsArray[lengthsSize]{};
			AddToIndexArray<TLengths...>(lengthsArray, std::forward<TLengths>(lengths)..., std::make_index_sequence<lengthsSize>{});
			if constexpr (lengthsSize > 1)
				ResizeFieldArrayInternal(array, lengthsArray, lengthsSize);
			else
				ResizeFieldArrayInternal(array, lengthsArray[0]);
		}

		int32_t GetFieldArrayLength(const ScriptArray& array, int dimension = 0) const;

		const ScriptField& GetScriptField(int32_t fieldHandle) const { return m_Fields.at(fieldHandle); }
		ScriptArray& GetScriptArray(int32_t fieldHandle);

		void InvokeInit();
		void InvokeUpdate(float deltaTime);

		void CopyFieldFrom(int32_t fieldHandle, Shared<ScriptInstance> source);
		void CopyAllFieldsFrom(Shared<ScriptInstance> source);

		const void* GetHandle() const { return m_Context; }

	private:
		void SetFieldValueInternal(int32_t fieldHandle, void* value) const;
		void GetFieldValueInternal(int32_t fieldHandle, void* value) const;
		void SetFieldArrayValueInternal(const ScriptArray& array, void* value, const int32_t* indices, size_t indicesSize) const;
		void GetFieldArrayValueInternal(const ScriptArray& array, void* value, const int32_t* indices, size_t indicesSize) const;
		void ResizeFieldArrayInternal(ScriptArray& array, int32_t length);
		void ResizeFieldArrayInternal(ScriptArray& array, const int32_t* lengths, size_t lengthsSize);

	private:
		const void* m_Context;
		int32_t m_OnInitMethodHandle;
		int32_t m_OnUpdateMethodHandle;
		std::unordered_map<int32_t, ScriptField> m_Fields;
		mutable std::unordered_map<int32_t, ScriptObject> m_FieldObjects;

		friend class ScriptEngine;
	};
}
