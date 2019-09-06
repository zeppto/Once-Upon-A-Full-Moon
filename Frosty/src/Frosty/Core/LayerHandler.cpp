#include "fypch.hpp"
#include "LayerHandler.hpp"

namespace Frosty
{
	LayerHandler::LayerHandler()
	{
	}

	LayerHandler::~LayerHandler()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();		// Just added this
			delete layer;
		}
	}

	void LayerHandler::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerHandler::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerHandler::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}

	}

	void LayerHandler::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end())
			m_Layers.erase(it);
	}
}
