#pragma once

#include "VertexArray.h"
#include "Core/Base.h"

namespace TerranEngine 
{
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

		static void Draw(const Shared<VertexArray>& vertexArray, int numIndices);
		static void DrawArrays(int numIndices);

	};
}