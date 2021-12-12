#include "EditorCamera.h"

#include "Terran.h"

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace TerranEditor 
{
	EditorCamera::EditorCamera(bool calculateVPMatrices) 
	{
		if (calculateVPMatrices) 
		{
			RecalculateView();
			RecalculateProjection();
		}
	}

	void EditorCamera::Update(float& time)
	{
		const glm::vec2 mousePos{ Input::GetMousePos() };
		glm::vec2 mouseDelta = (mousePos - m_OrigMousePos) * 0.002f;
		m_OrigMousePos = mousePos;

		if (!m_BlockInput) 
		{
			if (Input::IsMouseButtonPressed(MouseButton::LeftButton)) 
				PanCamera(mouseDelta);
		}

		RecalculateView();

	}

	void EditorCamera::OnEvent(Event& event) 
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<MouseScrollEvent>(TR_EVENT_BIND_FN(EditorCamera::OnMouseScroll));
	}

	void EditorCamera::OnViewportResize(float width, float height) 
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		m_AspectRatio = width / height;

		RecalculateProjection();
	}

	void EditorCamera::RecalculateProjection() 
	{
		float width = (m_OrthoGraphicSize * m_AspectRatio * 0.5f) * 2;
		float height = (m_OrthoGraphicSize * 0.5f) * 2;

		m_ProjectionMatrix = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, m_OrthographicNear, m_OrthographicFar);
	}

	void EditorCamera::RecalculateView() 
	{
		m_Position = CalculatePosition();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	void EditorCamera::CameraZoom(float delta) 
	{
		if (m_CameraType == EditorCameraType::Orthographic)
		{
			m_OrthoGraphicSize -= delta * 0.8f;
			if (m_OrthoGraphicSize < 1.0f) 
			{
				m_FocalPoint += GetForwardDirection();
				m_OrthoGraphicSize = 1.0f;
			}

			RecalculateProjection();
		}
	}

	void EditorCamera::PanCamera(glm::vec2 delta)
	{
		m_FocalPoint.x += -delta.x * 0.03f * m_OrthoGraphicSize;
		m_FocalPoint.y += delta.y * 0.03f * m_OrthoGraphicSize;
	}

	bool EditorCamera::OnMouseScroll(MouseScrollEvent& e) 
	{
		CameraZoom(e.GetYOffset());

		return false;
	}

	glm::vec3 EditorCamera::GetForwardDirection() 
	{
		return glm::rotate(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition() 
	{
		return m_FocalPoint - GetForwardDirection();
	}
	glm::vec2 EditorCamera::GetPanSpeed()
	{
		return glm::vec2();
	}
}