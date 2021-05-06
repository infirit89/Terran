#pragma once

#include "VertexArray.h"
#include <memory>

namespace TerranEngine 
{
	class RenderCommand
	{
	public:
		static void SetClearColor(float r, float g, float b, float a);
		static void Clear();
		static void Resize(int width, int height);

		static void EnableBlending();

		static void Draw(const VertexArray& vertexArray, int numIndices);
	};
}