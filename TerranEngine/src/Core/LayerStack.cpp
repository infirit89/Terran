#include "trpch.h"
#include "LayerStack.h"

namespace TerranEngine {

	LayerStack::~LayerStack()
	{
		RemoveAllLayers();
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace_back(layer);
	}

	void LayerStack::RemoveLayer(Layer* layer)
	{
		auto iterator = std::find(m_Layers.begin(), m_Layers.end(), layer);

		layer->OnDettach();
		m_Layers.erase(iterator);
		m_LastInsertIndex--;
	}

	void LayerStack::RemoveAllLayers()
	{
		while (!m_Layers.empty()) 
		{
			Layer* layer = *(m_Layers.end() - 1);
			layer->OnDettach();
			delete layer;
			m_Layers.erase(m_Layers.end() - 1);
		}
	}
}
