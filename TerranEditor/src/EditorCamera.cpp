#include "EditorCamera.h"

#include "Terran.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace TerranEditor {

using namespace TerranEngine;
EditorCamera::EditorCamera()
{
    RecalculateProjection();
    RecalculateView();
}

void EditorCamera::Update(Terran::Core::Time& time)
{
    glm::vec2 const mousePos { Input::GetMousePos() };
    glm::vec2 mouseDelta = (mousePos - m_OrigMousePos) * 0.002f;
    m_OrigMousePos = mousePos;

    if (!m_BlockInput) {
        if (Input::IsMouseButtonDown(MouseButton::MiddleButton))
            PanCamera(mouseDelta, time);
    }

    if (m_ViewDirty)
        RecalculateView();

    m_ViewDirty = false;
}

void EditorCamera::OnEvent(Terran::Core::Event& event)
{
    Terran::Core::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseScrollEvent>(TR_EVENT_BIND_FN(EditorCamera::OnMouseScroll));
}

void EditorCamera::OnViewportResize(float width, float height)
{
    m_ViewportWidth = width;
    m_ViewportHeight = height;

    m_AspectRatio = width / height;

    RecalculateProjection();
}

void EditorCamera::SetFocalPoint(glm::vec3 const& focalPoint)
{
    m_FocalPoint = focalPoint;
    RecalculateView();
}

void EditorCamera::RecalculateProjection()
{
    float orthRight = (m_OrthoGraphicSize * m_AspectRatio * 0.5f);
    float orthoTop = (m_OrthoGraphicSize * 0.5f);

    m_ProjectionMatrix = glm::ortho(-orthRight, orthRight, -orthoTop, orthoTop, m_OrthographicNear, m_OrthographicFar);
}

void EditorCamera::RecalculateView()
{
    m_Position = CalculatePosition();
    m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position);
    m_ViewMatrix = glm::inverse(m_ViewMatrix);
}

void EditorCamera::CameraZoom(float delta)
{
    if (m_CameraType == EditorCameraType::Orthographic) {
        m_OrthoGraphicSize -= delta * m_OrthoGraphicSize * m_ZoomSpeed;
        m_OrthoGraphicSize = std::max(0.01f, m_OrthoGraphicSize);

        RecalculateProjection();
    }
}

void EditorCamera::PanCamera(glm::vec2 delta, Terran::Core::Time& time)
{
    m_FocalPoint.x += -delta.x * 0.03f * m_OrthoGraphicSize * time.GetDeltaTimeMilliseconds();
    m_FocalPoint.y += delta.y * 0.03f * m_OrthoGraphicSize * time.GetDeltaTimeMilliseconds();
    m_ViewDirty = true;
}

bool EditorCamera::OnMouseScroll(MouseScrollEvent& e)
{
    CameraZoom(e.GetYOffset());
    return false;
}

glm::vec3 EditorCamera::CalculatePosition()
{
    return m_FocalPoint;
}

}
