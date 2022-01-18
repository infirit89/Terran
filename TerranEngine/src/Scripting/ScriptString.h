#pragma once

#include <mono/metadata/object.h>

namespace TerranEngine 
{
	class ScriptString 
	{
	public:
		ScriptString(MonoString* monoString);
		ScriptString(const char* string);
		~ScriptString();

		inline const char* GetUTF8Str() const			{ return m_StrBuffer; }
		inline MonoString* GetStringInternal() const	{ return m_MonoString; }

	private:
		const char* m_StrBuffer = nullptr;
		MonoString* m_MonoString = nullptr;
	};
}