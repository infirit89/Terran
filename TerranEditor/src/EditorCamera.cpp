#include "EditorCamera.h"

#include "Terran.h"

#include <glm/gtc/matrix_transform.hpp>

namespace TerranEditor 
{
	EditorCamera::EditorCamera() 
	{
		RecalculateView();
		RecalculateProjection();
	}

	void EditorCamera::Update(float& time)
	{
		const glm::vec2 mousePos{ Input::GetMousePos() };
		glm::vec2 mouseDelta = (mousePos - m_OrigMousePos) * 0.002f;
		m_OrigMousePos = mousePos;

		if (!m_BlockInput) 
		{
			//if (Input::IsMouseButtonPressed(MouseButton::LeftButton)) 
			//	PanCamera(mouseDelta);
		}

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

		m_AspectRatio = height / width;

		RecalculateProjection();
	}

	void EditorCamera::RecalculateProjection() 
	{
		float width = m_OrthoGraphicSize;
		float height = (m_OrthoGraphicSize * m_AspectRatio);

		m_ProjectionMatrix = glm::ortho(-width, width, -height, height, m_OrthographicNear, m_OrthographicFar);
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
			RecalculateProjection();
		}

		//RecalculateView();
	}

	void EditorCamera::PanCamera(glm::vec2 delta)
	{
		m_FocalPoint.x += -delta.x * 0.03f * m_OrthoGraphicSize;
		m_FocalPoint.y += delta.y * 0.03f * m_OrthoGraphicSize;

		RecalculateView();
	}

	bool EditorCamera::OnMouseScroll(MouseScrollEvent& e) 
	{
		CameraZoom(e.GetYOffset());

		return false;
	}

	glm::vec3 EditorCamera::CalculatePosition() 
	{
		return m_FocalPoint * -m_OrthoGraphicSize;
	}
	glm::vec2 EditorCamera::GetPanSpeed()
	{
		return glm::vec2();
	}
}