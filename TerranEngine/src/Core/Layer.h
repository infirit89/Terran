#pragma once

#include "Time.h"

#include "Events/Event.h"

#include <string_view>

namespace TerranEngine {

	class Layer {
	public:
		Layer(std::string_view name)
			: m_Name(name) {}

		virtual ~Layer() = default;

		virtual void Update(Time& time) {}
		virtual void OnEvent(Event& event) {}
		virtual void ImGuiRender() {}
		virtual void OnAttach() {}
		virtual void OnDetach() {}

	private:
		std::string_view m_Name;
	};
}