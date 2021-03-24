#include "trpch.h"
#include "LayerStack.h"

namespace TerranEngine {

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers) 
		{
			layer->OnDettach();
			delete layer;
		}
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


}