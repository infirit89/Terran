#pragma once

#include "EditorPanel.h"

#include "Core/Base.h"
#include "Scene/Scene.h"

namespace TerranEditor 
{
	using namespace TerranEngine;

	class ECSPanel : public EditorPanel
	{
	public:
		ECSPanel() = default;
		~ECSPanel() = default;

		virtual void OnRender() override;

		virtual void SetSceneContext(const Shared<Scene>& context) override { m_Scene = context; }
		virtual const char* GetName() override { return "ECS Debug"; }
	};
}
