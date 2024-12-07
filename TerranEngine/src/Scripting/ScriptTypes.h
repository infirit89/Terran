#pragma once

namespace Coral 
{
	class Type;
}

namespace TerranEngine 
{
	class ScriptTypes final
	{
	public:
		static void Initialize();
		static Coral::Type const* ScriptableType;
		static Coral::Type const* RigidbodyType;
		static Coral::Type const* HitInfoType;
		static Coral::Type const* EntityType;
		static Coral::Type const* IdType;
		static Coral::Type const* SerializeFieldType;
	};
}
