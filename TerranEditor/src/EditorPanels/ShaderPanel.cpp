#include "ShaderPanel.h"

#include "Graphics/ShaderLibrary.h"
#include "Graphics/ShaderCompiler.h"

#include "Utils/Debug/OptickProfiler.h"

#include "UI/UI.h"

namespace TerranEditor 
{
	using namespace TerranEngine;

	void ShaderPanel::OnRender()
	{
		TR_PROFILE_FUNCTION();
		if (!m_Open)
			return;

		ImGui::Begin(GetName().data(), &m_Open);

		auto button = [](const std::string& label, std::string_view buttonLabel) -> bool 
		{
			ImGui::PushID(label.c_str());
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", label.c_str());

			ImGui::TableSetColumnIndex(1);

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			bool changed = ImGui::Button(buttonLabel.data());
			ImGui::PopID();

			return changed;
		};

		if (UI::BeginPropertyGroup("shaders")) 
		{
			for (const auto& [shaderName, shader] : ShaderLibrary::GetShaders()) 
			{
				ImGui::TableNextRow();
				if (button(shaderName, "Reload"))
					ShaderCompiler::Recompile(shader);
			}
			UI::EndPropertyGroup();
		}

		ImGui::End();
	}
}
