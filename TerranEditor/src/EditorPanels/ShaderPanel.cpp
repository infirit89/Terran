#include "ShaderPanel.h"

#include "Graphics/ShaderLibrary.h"
#include "Graphics/ShaderCompiler.h"

#include "UI/UI.h"

namespace TerranEditor 
{
	using namespace TerranEngine;

	void ShaderPanel::OnRender()
	{
		if (!m_Open)
			return;

		ImGui::Begin(GetName(), &m_Open);

		if (UI::BeginPropertyGroup("shaders")) 
		{
			for (const auto& [shaderName, shader] : ShaderLibrary::GetShaders()) 
			{
				ImGui::TableNextRow();
				if (UI::Button(shaderName, "Reload"))
					ShaderCompiler::Recompile(shader);
			}
			UI::EndPropertyGroup();
		}

		ImGui::End();
	}
}
