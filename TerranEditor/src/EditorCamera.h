#pragma once

#include "Core/Time.h"

#include "Graphics/Camera.h"

#include "Events/MouseEvent.h"

namespace TerranEditor 
{
	enum class EditorCameraType 
	{
		Orthographic = 0,
		Perspective
	};

	class EditorCamera : public TerranEngine::Camera
	{
	public:
		EditorCamera();

		void Update(TerranEngine::Time& time);

		void OnEvent(TerranEngine::Event& event);

		void OnViewportResize(float width, float height);

		void SetCameraType(EditorCameraType type) { m_CameraType = type; }
		void SetBlockInput(bool block) { m_BlockInput = block; }
		void SetZoomSpeed(float zoomSpeed) { m_ZoomSpeed = zoomSpeed; }

		glm::mat4& GetView() { return m_ViewMatrix; }

		void SetFocalPoint(const glm::vec3& focalPoint);

	private:
		void RecalculateProjection();
		void RecalculateView();
		void CameraZoom(float delta);
		void PanCamera(glm::vec2 delta, TerranEngine::Time& time);

		bool OnMouseScroll(TerranEngine::MouseScrollEvent& e);
		
		glm::vec3 CalculatePosition();
		
	private:
		glm::vec2 m_OrigMousePos{ 0.0f, 0.0f };

		EditorCameraType m_CameraType = EditorCameraType::Orthographic;

		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		float m_AspectRatio = 0.0f;
		float m_OrthoGraphicSize = 10.0f;
		float m_OrthographicFar = 10.0f, m_OrthographicNear = -10.0f;

		float m_ZoomSpeed = 0.25f;

		float m_Pitch = 0.0f, m_Yaw = 0.0f;
		
		float m_ViewportWidth = 1080.0f, m_ViewportHeight = 780.0f;

		bool m_BlockInput = false;
		bool m_ViewDirty = false;
	};
}
