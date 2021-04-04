#pragma once

#include "Events/Event.h"

#include <string>

namespace TerranEngine {

	class Layer {
	public:
		Layer(const char* name) 
			: m_Name(name)
		{
		}

		virtual ~Layer() = default;

		virtual void Update(float& time) {}
		virtual void OnEvent(Event& event) {}
		virtual void ImGuiRender() {}
		virtual void OnAttach() {}
		virtual void OnDettach() {}

	private:
		const char* m_Name;
	};
}