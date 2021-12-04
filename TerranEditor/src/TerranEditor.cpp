#include "Core/EntryPoint.h"

#include "EditorLayer.h"

namespace TerranEngine
{
	class TerranEditorApp : public Application 
	{
	public:
		TerranEditorApp()
			: Application()
		{
			WindowData data;
			data.Name = "Terran";
			data.Width = 1080;
			data.Height = 790;
			Create(data);

			PushLayer(new TerranEditor::EditorLayer());
		}

		~TerranEditorApp()
		{

		}
	};

	Application* CreateApplication()
	{
		return new TerranEditorApp();
	}
}