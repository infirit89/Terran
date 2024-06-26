#pragma once

#include "EditorPanel.h"

namespace TerranEditor
{
	class ShaderPanel : public EditorPanel
	{
	public:
		ShaderPanel() = default;
		~ShaderPanel() = default;

		virtual std::string_view GetName() override { return "Shader Panel"; }
		virtual void OnRender() override;
	};
}
