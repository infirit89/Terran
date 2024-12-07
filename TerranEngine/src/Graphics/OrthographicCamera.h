#pragma once

#include "Camera.h"

namespace TerranEngine 
{
	class OrthographicCamera final : public Camera
	{
	public: 
		OrthographicCamera();

		void SetViewport(float width, float height);

		float GetOrthographicNear() const { return m_OrthographicNear; }
		void SetOrthographicNear(float nearClip) { m_OrthographicNear = nearClip; AdjustProjection(); }

		float GetOrthographicFar() const { return m_OrthographicFar; }
		void SetOrthographicFar(float farClip) { m_OrthographicFar = farClip; AdjustProjection(); }

		void SetOrthographicSize(float size) { m_OrthographicSize = size; AdjustProjection(); }
		float GetOrthographicSize() const { return m_OrthographicSize; }

	private:
		void AdjustProjection();

		float m_AspectRatio = 0.0f;
		float m_OrthographicSize = 10.0f;
		float m_OrthographicFar = 10.0f, m_OrthographicNear = -10.0f;
	};
}