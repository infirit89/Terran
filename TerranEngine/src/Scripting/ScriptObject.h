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
		~ScriptObject();

		Shared<ScriptField> GetField(const char* fieldName);
		std::vector<Shared<ScriptField>> GetFields();
		std::vector<Shared<ScriptField>> GetPublicFields();

	private:
		inline void* GetNativeObject() const;

	private:
		uint32_t m_MonoGCHandle;

		std::unordered_map<uint32_t, Shared<ScriptField>> m_Fields;
		friend class ScriptMethod;
	};
}
