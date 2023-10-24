#pragma once

#include "EditorPanel.h"

namespace TerranEditor
{
	class ShaderPanel : public EditorPanel
	{
	public:
		ShaderPanel() = default;
		~ShaderPanel() = default;

		virtual const char* GetName() override { return "Shader Panel"; }
		virtual void OnRender() override;
	};
}
