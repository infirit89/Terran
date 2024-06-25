#pragma once

#include "Scene.h"

#include "Core/Base.h"
#include "Core/Result.h"

#include <filesystem>

namespace TerranEngine 
{
	class SceneSerializer 
	{
	public:
		SceneSerializer() = default;
		SceneSerializer(const Shared<Scene>& scene);

		void SerializeEditor(const std::filesystem::path& scenePath);
		Result DesirializeEditor(const std::filesystem::path& scenePath);

	public:
		static const char* SceneFilter;

	private:
		Shared<Scene> m_Scene;
	};
}
