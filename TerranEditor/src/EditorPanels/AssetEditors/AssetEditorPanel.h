#pragma once

#include "Core/UUID.h"

namespace TerranEditor 
{
	class AssetEditorPanel 
	{
	public:
		AssetEditorPanel() = default;
		virtual ~AssetEditorPanel() = default;

		virtual void OnRender() = 0;
		void SetOpen(bool open, const TerranEngine::UUID& id) 
		{
			m_Open = open;
			m_ID = id;
		}

	protected:
		bool m_Open = false;
		TerranEngine::UUID m_ID;
	};
}