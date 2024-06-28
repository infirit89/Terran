#pragma once

#include "Asset/Asset.h"

namespace TerranEngine 
{
	class PhysicsMaterial2D : public Asset
	{
	public:
		PhysicsMaterial2D() = default;
		virtual ~PhysicsMaterial2D() override = default;

		ASSET_CLASS_TYPE(PhysicsMaterial2D)

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
	};
}

