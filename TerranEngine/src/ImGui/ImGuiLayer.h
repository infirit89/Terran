#pragma once

#include "Core/Layer.h"

namespace TerranEngine 
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDettach() override;

		void BeginFrame();
		void EndFrame();
	};
}