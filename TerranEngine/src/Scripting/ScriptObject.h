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

		ScriptField GetField(const char* fieldName);
		std::vector<ScriptField> GetFields();
		std::vector<ScriptField> GetPublicFields();

	private:
		void* GetNativeObject() const;

	private:
		uint32_t m_MonoGCHandle;

		std::unordered_map<uint32_t, ScriptField> m_Fields;
		friend class ScriptMethod;
	};
}
