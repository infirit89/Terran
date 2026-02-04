#pragma once

#include "Scene.h"

// #include "Core/Result.h"
#include <LibCore/Base.h>
#include <LibCore/Result.h>

#include <filesystem>

namespace Terran::World {

enum class SceneSerializationError {
    InvalidFormat,
    NotFound,
};

class SceneSerializer final {
public:
    SceneSerializer() = default;
    SceneSerializer(Terran::Core::Shared<Scene> const& scene);

    void SerializeEditor(std::filesystem::path const& scenePath);
    Core::Result<void, SceneSerializationError> DeserializeEditor(std::filesystem::path const& scenePath);

public:
    static char const* SceneFilter;

private:
    Terran::Core::Shared<Scene> m_Scene;
};

}
