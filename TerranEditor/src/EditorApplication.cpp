#include "EditorLayer.h"

#include "Core/EntryPoint.h"

namespace TerranEditor
{
	class EditorApplication : public TerranEngine::Application 
	{
	public:
		EditorApplication(const TerranEngine::ApplicationData& appData, const std::string& projectPath)
			: Application(appData)
		{
			PushLayer(new TerranEditor::EditorLayer(projectPath));
		}

		~EditorApplication()
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
    applicationData.Window.VSync = false;
    applicationData.Window.Maximized = true;
    applicationData.Window.Fullscren = false;
    applicationData.Window.Debug = true;

    applicationData.ScriptCorePath = "Resources/Scripts/TerranScriptCore.dll";
    
    return new TerranEditor::EditorApplication(applicationData, projectPath);
}
