#pragma once
#include "EditorPanel.h"

#include "UI/UI.h"

namespace TerranEditor 
{
	class PreferencesPanel : public EditorPanel
	{
	public:
		PreferencesPanel() = default;
		virtual ~PreferencesPanel() override = default;

		virtual void OnRender() override;
		virtual std::string_view GetName() override { return "Preferences"; }

	private:
		ImGuiStyle* m_Style = nullptr;
	};
}
