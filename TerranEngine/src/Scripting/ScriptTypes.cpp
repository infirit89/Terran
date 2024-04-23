#include "trpch.h"
#include "ScriptTypes.h"

#include <Coral/Type.hpp>
#include <Coral/TypeCache.hpp>

namespace TerranEngine 
{
	Coral::Type const* ScriptTypes::ScriptableType;
	Coral::Type const* ScriptTypes::RigidbodyType;
	Coral::Type const* ScriptTypes::HitInfoType;
	Coral::Type const* ScriptTypes::EntityType;
	Coral::Type const* ScriptTypes::IdType;
	Coral::Type const* ScriptTypes::SerializeFieldType;

	void ScriptTypes::Initialize()
	{
		TR_CORE_INFO(TR_LOG_SCRIPT, "Caching script types");
		ScriptableType = Coral::TypeCache::Get().GetTypeByName("Terran.Scriptable");
		TR_ASSERT(ScriptableType, "Failed to find the Terran.Scriptable type");

		IdType = Coral::TypeCache::Get().GetTypeByName("Terran.UUID");
		TR_ASSERT(IdType, "Failed to find the Terran.UUID type");

		RigidbodyType = Coral::TypeCache::Get().GetTypeByName("Terran.Rigidbody2D");
		TR_ASSERT(RigidbodyType, "Failed to find the Terran.Rigidbody2d type");

		HitInfoType = Coral::TypeCache::Get().GetTypeByName("Terran.RayCastHitInfo2D");
		TR_ASSERT(HitInfoType, "Failed to find the Terran.RayCastHitInfo2D type");

		EntityType = Coral::TypeCache::Get().GetTypeByName("Terran.Entity");
		TR_ASSERT(EntityType, "Failed to find the Terran.Entity type");

		SerializeFieldType = Coral::TypeCache::Get().GetTypeByName("Terran.SerializeFieldAttribute");
		TR_ASSERT(SerializeFieldType, "Failed to find the Terran.SerializeFieldAttribute type");
	}
}

