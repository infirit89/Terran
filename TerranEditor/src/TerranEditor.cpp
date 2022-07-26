#include "EditorLayer.h"

#include "Core/EntryPoint.h"

namespace TerranEditor
{
	class TerranEditorApplication : public TerranEngine::Application 
	{
	public:
		TerranEditorApplication(const TerranEngine::ApplicationData& appData, const std::string& projectPath)
			: Application(appData)
		{
			PushLayer(new TerranEditor::EditorLayer(projectPath));
		}

		~TerranEditorApplication()
		{

		}
	};
}

TerranEngine::Application* TerranEngine::CreateApplication(int argc, char** argv)
{
    std::string projectPath = "";

    if(argc > 1)
        projectPath = argv[1];
    
    TerranEngine::ApplicationData applicationData;

    applicationData.Window.Name = "Terran";
    applicationData.Window.Width = 1080;
    applicationData.Window.Height = 790;
    applicationData.Window.VSync = true;
    applicationData.Window.Maximized = true;
    applicationData.Window.Fullscren = false;

    applicationData.ScriptCorePath = "Resources/Scripts/TerranScriptCore.dll";
    
    return new TerranEditor::TerranEditorApplication(applicationData, projectPath);
}
