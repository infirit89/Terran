#pragma once

#include <mono/metadata/object.h>

namespace TerranEngine 
{
	class ScriptString 
	{
	public:
		ScriptString() = default;

		ScriptString(MonoString* monoString);
		ScriptString(const char* string);

		ScriptString(const ScriptString& other) = default;
		~ScriptString();

		inline const char* GetUTF8Str() const			{ return m_StrBuffer; }
		inline MonoString* GetStringInternal() const	{ return m_MonoString; }

	private:
		bool m_MonoAllocatedBuffer = false;
		const char* m_StrBuffer = nullptr;
		MonoString* m_MonoString = nullptr;
	};
}