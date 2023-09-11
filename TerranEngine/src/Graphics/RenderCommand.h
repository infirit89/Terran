#pragma once

#include "VertexArray.h"

#include "Core/Base.h"

namespace TerranEngine 
{
	enum class RenderMode : uint32_t 
	{
		Points = 0,
		Lines,
		LineLoop,
		LineStrip,
		Triangles,
		TriangleStrip,
		TriangleFan,
	};

	class RenderCommand
	{
	public:
		static void Init();

		static void SetClearColor(float r, float g, float b, float a);
		static void Clear();

		static void Resize(int width, int height);
		static void WireframeMode(bool enable);

		static void EnableBlending(bool state);

		static void EnableDepthTesting(bool state);

		static void Draw(RenderMode mode, const Shared<VertexArray>& vertexArray, int numIndices);
		static void DrawArrays(RenderMode mode, int numVertices);
		static void DrawInstanced(RenderMode mode, const Shared<VertexArray>& vertexArray, int instanceCount);
		static void SetLineWidth(float lineWidth);

		static uint32_t GetAPIVersion();
	};
}