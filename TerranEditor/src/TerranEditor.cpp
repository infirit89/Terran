#include "Core/EntryPoint.h"

#include "EditorLayer.h"

namespace TerranEngine
{
	class TerranEditor : public Application 
	{
	public:
		TerranEditor() : Application("Terran", { "" })
		{
			PushLayer(new EditorLayer());
		}

		~TerranEditor()
		{

		}
	};

	Application * CreateApplication()
	{
		return new TerranEditor();
	}
}