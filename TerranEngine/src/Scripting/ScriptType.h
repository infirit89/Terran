#pragma once

namespace TerranEngine 
{
	enum class ScriptType 
	{
		None = 0,

		Int8,
		UInt8,
		Int16,
		UInt16,
		Int32,
		UInt32,
		Int64,
		UInt64,

		Float,
		Double,

		Bool,
		Char,


		Vector2,
		Vector3,
		Color,

		String,
		Entity
	};
}
