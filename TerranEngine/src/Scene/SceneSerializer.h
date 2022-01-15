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

		static std::string ReadJson(const std::string& filePath);

		bool DesirializeJson(const std::string& data);
	private:

		Shared<Scene> m_Scene;
	};
}