#include "Core/EntryPoint.h"

#include "EditorLayer.h"

namespace TerranEngine
{
	class TerranEditor : public Application 
	{
	public:
		TerranEditor()
			: Application()
		{
			WindowData data;
			data.Name = "Terran";
			data.Width = 1080;
			data.Height = 790;
			Create(data);

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