#pragma once

#include "Scene.h"

#include "Core/Base.h"

#include <filesystem>

namespace TerranEngine 
{
	class SceneSerializer 
	{
	public:
		SceneSerializer() = default;
		SceneSerializer(const Shared<Scene>& scene);

		void SerializeJson(const std::filesystem::path& scenePath);

		static std::string ReadJson(const std::filesystem::path& scenePath);

		bool DesirializeJson(const std::string& data);
	private:

		Shared<Scene> m_Scene;
	};
}