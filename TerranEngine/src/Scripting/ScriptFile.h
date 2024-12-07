#pragma once

#include "Asset/Asset.h"

namespace TerranEngine 
{
	class ScriptFile final : public Asset
	{
	public:
		ScriptFile() = default;
		~ScriptFile() override = default;

		ASSET_CLASS_TYPE(ScriptFile)
	};
}
