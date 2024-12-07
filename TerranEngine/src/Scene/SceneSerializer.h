#pragma once

#include "Scene.h"

#include "Core/Base.h"
#include "Core/Result.h"

namespace TerranEngine 
{
	class SceneSerializer final
	{
	public:
		SceneSerializer() = default;
		SceneSerializer(const Shared<Scene>& scene);

		void SerializeEditor(const std::filesystem::path& scenePath);
		Result DeserializeEditor(const std::filesystem::path& scenePath);

	public:
		static const char* SceneFilter;

	private:
		Shared<Scene> m_Scene;
	};
}
