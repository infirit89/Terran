#include "trpch.h"
#include "Camera.h"
#include "LibCore/Base.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

glm::vec3 TerranEngine::Camera::ScreenToWorld(glm::mat4 const& viewMatrix, glm::vec3 screenPoint)
{
    Terran::Core::Shared<Scene> const& currentScene = SceneManager::GetCurrentScene();
    screenPoint.x = (2.0f * (screenPoint.x / currentScene->GetViewportWidth())) - 1.0f;
    screenPoint.y = 1.0f - (2.0f * (screenPoint.y / currentScene->GetViewportHeight()));

    return glm::inverse(viewMatrix * m_ProjectionMatrix) * glm::vec4(screenPoint, 1.0f);
}
