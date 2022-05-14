#pragma once

#include "ScriptMethod.h"
#include "ScriptField.h"

#include "GCManager.h"

#include "Core/Base.h"

#include <unordered_map>

typedef struct _MonoObject MonoObject;

namespace TerranEngine 
{
	class ScriptObject 
	{
	public:
		ScriptObject() = default;
		ScriptObject(GCHandle gcHandle);

		ScriptObject(const ScriptObject& other) = default;
		~ScriptObject() = default;

		ScriptField GetPublicField(const char* fieldName);
		std::unordered_map<uint32_t, ScriptField>& GetFieldMap() { return m_PublicFields; };
		const std::vector<uint32_t>& GetFieldOrder() const { return m_FieldOrder; }
		
		inline GCHandle& GetGCHandle() { return m_GCHandle; }

	private:
		GCHandle m_GCHandle;

		std::vector<uint32_t> m_FieldOrder;
		std::unordered_map<uint32_t, ScriptField> m_PublicFields;
		friend class ScriptMethod;
	};
}
