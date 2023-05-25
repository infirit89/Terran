#pragma once

#include "Graphics/RendererAPI.h"

namespace TerranEngine 
{
#define TR_MINIMUM_OGL_VERSION 450

	class OpenGLRendererAPI : public RendererAPI 
	{
	public:
		OpenGLRendererAPI();
		virtual ~OpenGLRendererAPI() override = default;

		virtual void SetClearColor(float r, float g, float b, float a) override;
		virtual void Clear() override;
		virtual void Resize(int width, int height) override;
		virtual void ToggleWireframeMode(bool state) override;
		virtual void ToggleBlending(bool state) override;
		virtual void ToggleDepthTesting(bool state) override;
		virtual void Draw(RenderMode mode, const Shared<VertexArray>& vertexArray, int indexCount) override;
		virtual void DrawArrays(RenderMode mode, int vertexCount) override;
		virtual void SetLineWidth(float lineWidth) override;
		virtual uint32_t GetAPIVersion() override;
	};
}
