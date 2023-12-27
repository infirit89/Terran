#pragma once

#include "ManagedMethod.h"
#include "ScriptField.h"
#include "ManagedObject.h"

extern "C"
{
	typedef struct _MonoClass MonoClass;
}

namespace TerranEngine 
{
	class ManagedClass 
	{
	public:
		ManagedClass() = default;
		ManagedClass(MonoClass* monoClass);

		ManagedClass(const ManagedClass& other) = default;
		~ManagedClass() = default;

		ManagedMethod GetMethod(const char* methodSignature) const;
		ScriptField GetFieldFromToken(uint32_t fieldToken) const;
		ScriptField GetFieldFromName(const std::string& fieldName) const;
		
		inline MonoClass* GetMonoClass() const { return m_MonoClass; }
		inline const std::string&  GetName() const { return m_ClassName; }
		inline const std::string& GetNamespace() const { return m_Namespace; }
		inline std::string GetFullName() const { return fmt::format("{0}.{1}", m_Namespace, m_ClassName); }
		inline operator MonoClass* () const { return m_MonoClass; }
	
		ManagedClass GetParent();
		bool operator==(const ManagedClass& other) const { return m_MonoClass == other.m_MonoClass; }
		bool operator==(MonoClass* monoClass) const { return m_MonoClass == monoClass; }
		operator bool() const { return m_MonoClass != nullptr; }
		
		bool IsInstanceOf(ManagedClass* parent);
		bool IsSubclassOf(ManagedClass* parent, bool checkInterfaces = false);

		inline std::vector<ScriptField>& GetFields() { return m_Fields; }
		std::vector<ManagedMethod> GetMethods() const;

		bool IsEnum() const;

		int GetTypeToken() const;
		
		ManagedObject CreateInstance();

	private:
		MonoClass* m_MonoClass = nullptr;
		std::string m_ClassName;
		std::string m_Namespace;
		std::vector<ScriptField> m_Fields;

		friend class ScriptAssembly;
		friend class ScriptCache;
	};
}
