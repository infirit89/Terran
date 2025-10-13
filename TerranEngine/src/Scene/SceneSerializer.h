#pragma once

#include "Scene.h"

#include "Core/Result.h"
#include "LibCore/Base.h"

#include <filesystem>

namespace TerranEngine {

class SceneSerializer final {
public:
    SceneSerializer() = default;
    SceneSerializer(Terran::Core::Shared<Scene> const& scene);

    void SerializeEditor(std::filesystem::path const& scenePath);
    Result DeserializeEditor(std::filesystem::path const& scenePath);

public:
    static char const* SceneFilter;

private:
    Terran::Core::Shared<Scene> m_Scene;
};

}
