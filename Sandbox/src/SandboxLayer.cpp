#include "SandboxLayer.h"

#include <imgui.h>

namespace TerranEngine 
{
	void SandboxLayer::Update(Time& time)
	{
		TR_TRACE(1 / time.GetSeconds());
	}

	void SandboxLayer::OnEvent(Event& event)
	{
		EventDispatcher dispather(event);
		dispather.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(SandboxLayer::KeyPressed));
	}

	void SandboxLayer::ImGuiRender()
	{
	}

	bool SandboxLayer::KeyPressed(KeyPressedEvent& event)
	{
		if (event.GetRepeatCount() > 0)
			return false;

		switch (event.GetKeyCode())
		{
			case Key::Escape: 
			{
				Application::Get()->Close();
				break;
			}
		}

		return false;
	}
}

