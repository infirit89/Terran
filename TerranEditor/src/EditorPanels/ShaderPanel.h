#pragma once

#include "EditorPanel.h"

namespace TerranEditor
{
	class ShaderPanel : public EditorPanel
	{
	public:
		ShaderPanel() = default;
		~ShaderPanel() = default;

		virtual void OnRender() override;
	};
}
