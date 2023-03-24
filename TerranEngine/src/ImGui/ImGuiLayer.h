#pragma once

#include "Core/Layer.h"

#include "Events/ApplicationEvent.h"

namespace TerranEngine 
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDettach() override;

		void OnEvent(Event& event) override;

		void BeginFrame();
		void EndFrame();
		
		void SetBlockInput(bool block) { m_BlockInput = block; }
	private:
		bool OnWindowContentScaleChangedEvent(WindowContentScaleChangeEvent& cscEvent);

		bool m_BlockInput = true;
	};
}