#pragma once

#include <string>

namespace TerranEngine {

	class Layer {
	public:
		Layer(std::string name) 
			: m_Name(name)
		{
		}

		virtual void Update() {}
		virtual void OnEvent() {}
		virtual void OnAttach() {}
		virtual void OnDettach() {}

	private:
		std::string m_Name;
	};
}