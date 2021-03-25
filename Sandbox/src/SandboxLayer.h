#pragma once

#include "Terran.h"

namespace TerranEngine {

	class SandboxLayer : public Layer
	{
	public: 
		SandboxLayer() : Layer("SandboxLayer") {}

		void Update(Time& time) override;

		void OnEvent(Event& event) override;
		void ImGuiRender() override;

		bool KeyPressed(KeyPressedEvent& event);
	};
}