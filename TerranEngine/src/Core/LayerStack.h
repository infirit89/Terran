#pragma once

#include "Layer.h"

#include <vector>

namespace TerranEngine {

	class LayerStack 
	{
	public:
		~LayerStack();

		void PushLayer(Layer* layer);
		void RemoveLayer(Layer* layer);

		std::vector<Layer*> GetLayers() { return m_Layers; }
	private:
		std::vector<Layer*> m_Layers;
		int m_LastInsertIndex = 0;
	};
}
