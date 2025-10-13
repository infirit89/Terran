#pragma once

#include "LibCore/Layer.h"
#include "LibCore/Event.h"

#include "Events/ApplicationEvent.h"

namespace TerranEngine 
{
	class ImGuiLayer final : public Terran::Core::Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() override;

		void OnAttach() override;
		void OnDetach() override;

		void OnEvent(Terran::Core::Event& event) override;

		void BeginFrame();
		void EndFrame();
		
		void SetBlockInput(bool block) { m_BlockInput = block; }
	private:
		bool OnWindowContentScaleChangedEvent(WindowContentScaleChangeEvent& cscEvent);

		bool m_BlockInput = true;
	};
}
