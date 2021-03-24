#include "SandboxLayer.h"

namespace TerranEngine 
{
	void SandboxLayer::Update()
	{
	}

	void SandboxLayer::OnEvent(Event& event)
	{
		EventDispatcher dispather(event);
		dispather.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(SandboxLayer::TestKeyEvent));
	}

	bool SandboxLayer::TestKeyEvent(KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == Key::A)
			TR_TRACE("pog");

		return false;
	}
}

