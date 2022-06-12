#pragma once

#include "ScriptMethod.h"
#include "ScriptField.h"

extern "C"
{
	typedef struct _MonoClass MonoClass;
}

namespace TerranEngine 
{
	class ScriptClass 
	{
	public:
		ScriptClass() = default;
		ScriptClass(MonoClass* monoClass);

		ScriptClass(const ScriptClass& other) = default;
		~ScriptClass() = default;

		ScriptMethod GetMethod(const char* methodSignature);
		ScriptField GetFieldFromToken(uint32_t fieldToken);
		ScriptField GetFieldFromName(const std::string& fieldName);
		
		inline MonoClass* GetMonoClass() const { return m_MonoClass; }
		inline const std::string&  GetName() const { return m_ClassName; }
		inline const std::string& GetNamespace() const { return m_Namespace; }
		
		ScriptClass GetParent();
		bool operator==(const ScriptClass& other) const { return m_MonoClass == other.m_MonoClass; }
		operator bool() const { return m_MonoClass != nullptr; }
		
		bool IsInstanceOf(ScriptClass* parent, bool checkInterfaces = true);

		inline std::vector<ScriptField>& GetFields() { return m_Fields; }
		
	private:
		MonoClass* m_MonoClass = nullptr;
		std::string m_ClassName;
		std::string m_Namespace;
		std::vector<ScriptField> m_Fields;

		friend class ScriptAssembly;
		friend class ScriptCache;
	};
}
