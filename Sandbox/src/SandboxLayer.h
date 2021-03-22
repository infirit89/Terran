#pragma once

#include "Terran.h"

namespace TerranEngine {

	class SandboxLayer : public Layer
	{
	public: 
		SandboxLayer() : Layer("SandboxLayer") {}

		void Update() override;
	};
}