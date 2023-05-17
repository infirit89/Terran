#pragma once

#include "VertexArray.h"

#include "Core/Base.h"

namespace TerranEngine 
{
	enum class RenderMode
	{
		Points = 0,
		Lines,
		LineLoop,
		LineStrip,
		Triangles,
		TriangleStrip,
		TriangleFan,
	};

	class RendererAPI 
	{
	public:
		enum class API 
		{
			None = 0,
			OpenGL
		};

		virtual ~RendererAPI() = default;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;
		virtual void Clear() = 0;
		virtual void Resize(int width, int height) = 0;
		virtual void ToggleWireframeMode(bool state) = 0;
		virtual void ToggleBlending(bool state) = 0;
		virtual void ToggleDepthTesting(bool state) = 0;
		virtual void Draw(RenderMode mode, const Shared<VertexArray>& vertexArray, int indexCount) = 0;
		virtual void DrawArrays(RenderMode mode, int vertexCount) = 0;
		virtual void SetLineWidth(float lineWidth) = 0;
		virtual uint32_t GetAPIVersion() = 0;

	public:
		static Shared<RendererAPI> Create(API api);
		static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};
}
