#pragma once

#include "Core/Base.h"

#include "ScriptMethod.h"
#include "ScriptField.h"

#include <unordered_map>

namespace TerranEngine 
{
	class ScriptObject 
	{
	public:
		ScriptObject() = default;
		ScriptObject(uint32_t monoGCHandle);

		ScriptObject(const ScriptObject& other) = default;
		~ScriptObject();

		void Uninitialize();

		ScriptField GetPublicField(const char* fieldName);
		std::unordered_map<uint32_t, ScriptField>& GetFieldMap() { return m_PublicFields; };
		const std::vector<uint32_t>& GetFieldOrder() const { return m_FieldOrder; }

	private:
		void* GetNativeObject() const;

	private:
		uint32_t m_MonoGCHandle;
		
		std::vector<uint32_t> m_FieldOrder;
		std::unordered_map<uint32_t, ScriptField> m_PublicFields;
		friend class ScriptMethod;
	};
}
