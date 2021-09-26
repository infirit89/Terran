#pragma once

#include "Core/Base.h"
#include "Scene.h"

namespace TerranEngine 
{
	class SceneSerializer 
	{
	public:
		SceneSerializer() = default;
		SceneSerializer(const Shared<Scene>& scene);

		void SerializeJson(const std::string& filePath);

		void DesirializeJson(const std::string& filePath);
	private:
		Shared<Scene> m_Scene;
	};
}