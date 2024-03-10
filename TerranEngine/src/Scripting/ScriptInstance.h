#pragma once

#include "ScriptType.h"

#include "Scene/Entity.h"

#include "Utils/Variant.h"

#include <vector>

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

	struct ScriptArray 
	{
		void* Handle;
		int32_t Rank;
	};

	template<typename... TIndices>
	concept indices_concept = sizeof...(TIndices) > 0;

	class ScriptInstance 
	{
	public:
		ScriptInstance(Coral::Type& type, Entity entity);
		~ScriptInstance();

		template<typename TValue>
		void SetFieldValue(int32_t fieldHandle, TValue value) const
		{
			SetFieldValueInternal(fieldHandle, &value);
		}

		template<typename TReturn>
		TReturn GetFieldValue(int32_t fieldHandle) const 
		{
			TReturn value;
			GetFieldValueInternal(fieldHandle, &value);
			return value;
		}

		template<typename TValue, std::same_as<int32_t>... TIndices>
			requires indices_concept<TIndices...>
		void SetFieldArrayValue(int32_t fieldHandle, TValue value, TIndices... indices) 
		{
			constexpr size_t indicesSize = sizeof...(indices);
			int32_t indicesArray[indicesSize]{};
			AddToIndexArray<TIndices...>(indicesArray, std::forward<TIndices>(indices)..., std::make_index_sequence<indicesSize>{});
			SetFieldArrayValueInternal(fieldHandle, &value, indicesArray, indicesSize);
		}

		template<typename TReturn, std::same_as<int32_t>... TIndices>
			requires indices_concept<TIndices...>
		TReturn GetFieldArrayValue(int32_t fieldHandle, TIndices... indices)
		{
			constexpr size_t indicesSize = sizeof...(indices);
			TReturn value;
			int32_t indicesArray[indicesSize]{};
			AddToIndexArray<TIndices...>(indicesArray, std::forward<TIndices>(indices)..., std::make_index_sequence<indicesSize>{});
			GetFieldArrayValueInternal(fieldHandle, &value, indicesArray, indicesSize);
			return value;
		}

		template<std::same_as<int32_t>... TLengths>
			requires indices_concept<TLengths...>
		void ResizeFieldArray(int32_t fieldHandle, TLengths... lengths) 
		{
			constexpr size_t lengthsSize = sizeof...(lengths);
			int32_t lengthsArray[lengthsSize]{};
			AddToIndexArray<TLengths...>(lengthsArray, std::forward<TLengths>(lengths)..., std::make_index_sequence<lengthsSize>{});
			if constexpr (lengthsSize > 1)
				ResizeFieldArrayInternal(fieldHandle, lengthsArray, lengthsSize);
			else
				ResizeFieldArrayInternal(fieldHandle, lengthsArray[0]);
		}

		const ScriptField& GetScriptField(int32_t fieldHandle) const { return m_Fields.at(fieldHandle); }
		const ScriptArray& GetScriptArray(int32_t fieldHandle) const { return m_FieldArrays.at(fieldHandle); }

		void InvokeInit();
		void InvokeUpdate(float deltaTime);

	private:
		void SetFieldValueInternal(int32_t fieldHandle, void* value) const;
		void GetFieldValueInternal(int32_t fieldHandle, void* value) const;
		void SetFieldArrayValueInternal(int32_t fieldHandle, void* value, const int32_t* indices, size_t indicesSize) const;
		void GetFieldArrayValueInternal(int32_t fieldHandle, void* value, const int32_t* indices, size_t indicesSize) const;
		Utils::Variant GetFieldArrayValueInternal(int32_t fieldHandle, const int32_t* indices, size_t indicesSize) const;
		void ResizeFieldArrayInternal(int32_t fieldHandle, int32_t length);
		void ResizeFieldArrayInternal(int32_t fieldHandle, const int32_t* lengths, size_t lengthsSize);

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

	private:
		const void* m_Context;
		int32_t m_OnInitMethodHandle;
		int32_t m_OnUpdateMethodHandle;
		std::unordered_map<int32_t, ScriptField> m_Fields;
		mutable std::unordered_map<int32_t, ScriptArray> m_FieldArrays;

		friend class ScriptEngine;
	};
}
