#pragma once

#include "LibCore/Base.h"

#include "Camera.h"
#include "Font.h"

#include <glm/glm.hpp>

namespace TerranEngine {

struct BatchRendererStats final {
    uint32_t MaxVertices = 0;
    uint32_t MaxIndices = 0;
    uint32_t VertexCount = 0;
    uint32_t IndexCount = 0;

    uint32_t DrawCalls = 0;
};

// TODO: make it static; because every other system is static
class BatchRenderer2D final {
public:
    static void Initialize(uint32_t batchSize);
    static void Shutdown();

    static void BeginFrame(Camera& camera, glm::mat4 const& transform, bool inverseView);
    static void EndFrame();

    static void AddQuad(glm::mat4& transform, glm::vec4 const& color, Terran::Core::Shared<Texture2D> texture, int entityID = -1);
    static void AddQuad(glm::mat4& transform, glm::vec4 const& color, int entityID = -1);
    static void AddQuad(glm::mat4& transform, glm::vec4 const& color, Terran::Core::Shared<Texture2D> texture, glm::vec2 textureCoordinates[4], int entityID = -1);

    static void AddCircle(glm::mat4& transform, glm::vec4 const& color, float thickness, int entityID = -1);

    static void AddLine(glm::vec3 const& startPoint, glm::vec3 const& endPoint, glm::vec4 const& color, float thickness, int entityID = -1);

    static void AddDebugLine(glm::vec3 const& startPoint, glm::vec3 const& endPoint, glm::vec4 const& color);
    static void AddDebugRect(glm::mat4 const& transform, glm::vec4 const& color);

    // TODO: use wide string
    static void AddString(glm::mat4& transform, std::string const& text, glm::vec4 const& color, Terran::Core::Shared<Font> fontAtlas, float lineSpacing = 1.0f, float lineWidth = 10.0f, int entityID = -1);

    static void ResetStats();
    static BatchRendererStats GetStats();

private:
    static void Clear();

    static bool QuadBatchHasRoom();
    static bool CircleBatchHasRoom();
    static bool LineBatchHasRoom();
    static bool TextBatchHasRoom();
};

}
