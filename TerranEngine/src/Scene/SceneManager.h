#pragma once

#include "Core/Base.h"

#include "Scene.h"

namespace TerranEngine 
{
	class SceneManager 
	{
	public:
		static Shared<Scene> CurrentScene;
	};
}