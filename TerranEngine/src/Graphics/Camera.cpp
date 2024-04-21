#include "trpch.h"
#include "Camera.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

glm::vec3 TerranEngine::Camera::ScreenToWorld(const glm::mat4& viewMatrix, glm::vec3 screenPoint)
{
	const Shared<Scene>& currentScene = SceneManager::GetCurrentScene();
	screenPoint.x = (2.0f * (screenPoint.x / currentScene->GetViewportWidth())) - 1.0f;
	screenPoint.y = 1.0f - (2.0f * (screenPoint.y / currentScene->GetViewportHeight()));

	return glm::inverse(viewMatrix * m_ProjectionMatrix) * glm::vec4(screenPoint, 1.0f);
}
