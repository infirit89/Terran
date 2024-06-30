#pragma once

#include "Asset/Asset.h"

namespace TerranEngine 
{
	class ScriptFile : public Asset 
	{
	public:
		ScriptFile() = default;
		virtual ~ScriptFile() override = default;

		ASSET_CLASS_TYPE(ScriptFile)
	};
}
