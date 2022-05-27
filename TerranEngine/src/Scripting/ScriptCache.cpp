#include "trpch.h"

#include "ScriptEngine.h"
#include "ScriptCache.h"

#include <mono/metadata/appdomain.h>

namespace TerranEngine 
{
#define TR_CLASS(className) ScriptEngine::GetClass("Terran."#className)

	void ScriptCache::CacheCoreClasses()
	{
		std::hash<std::string> hasher;

		// corlib classes
		s_CachedClasses.emplace(hasher("System.Byte"), mono_get_byte_class());
		s_CachedClasses.emplace(hasher("System.Short"), mono_get_int16_class());
		s_CachedClasses.emplace(hasher("System.Int"), mono_get_int32_class());
		s_CachedClasses.emplace(hasher("System.Long"), mono_get_int64_class());
		s_CachedClasses.emplace(hasher("System.Bool"), mono_get_boolean_class());
		s_CachedClasses.emplace(hasher("System.Float"), mono_get_single_class());
		s_CachedClasses.emplace(hasher("System.Double"), mono_get_double_class());

		// api core classes
		s_CachedClasses.emplace(TR_CLASS_ID(Vector2), TR_CLASS(Vector2));
		s_CachedClasses.emplace(TR_CLASS_ID(Vector3), TR_CLASS(Vector3));
		s_CachedClasses.emplace(TR_CLASS_ID(Entity), TR_CLASS(Entity));
		s_CachedClasses.emplace(TR_CLASS_ID(UUID), TR_CLASS(UUID));
		s_CachedClasses.emplace(TR_CLASS_ID(Component), TR_CLASS(Component));
		s_CachedClasses.emplace(TR_CLASS_ID(Rigidbody2D), TR_CLASS(Rigidbody2D));
		s_CachedClasses.emplace(TR_CLASS_ID(BoxCollider2D), TR_CLASS(BoxCollider2D));
		s_CachedClasses.emplace(TR_CLASS_ID(CircleCollider2D), TR_CLASS(CircleCollider2D));
		s_CachedClasses.emplace(TR_CLASS_ID(Scriptable), TR_CLASS(Scriptable));
		s_CachedClasses.emplace(TR_CLASS_ID(Tag), TR_CLASS(Tag));
		s_CachedClasses.emplace(TR_CLASS_ID(Transform), TR_CLASS(Transform));
	}

	void ScriptCache::ClearClassCache()
	{
		s_CachedClasses.clear();
	}

	ScriptClass* ScriptCache::GetCachedClassFromName(std::string className)
	{
		uint32_t classID = TR_CLASS_ID(className);

		if(s_CachedClasses.find(classID) != s_CachedClasses.end())
			return &s_CachedClasses.at(classID);

		return nullptr;
	}

}
