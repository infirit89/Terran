#pragma once

#include "Core/UUID.h"

namespace TerranEditor 
{
	class AssetEditorPanel 
	{
	public:
		AssetEditorPanel() = default;
		virtual ~AssetEditorPanel() = default;

		virtual void OnRender(const TerranEngine::UUID& assetID) = 0;
		void SetOpen(bool open) { m_Open = open; }

	protected:
		bool m_Open = true;
	};
}