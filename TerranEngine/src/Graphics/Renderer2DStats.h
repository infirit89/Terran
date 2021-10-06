#pragma once

#include <stdint.h>

namespace TerranEngine 
{
	struct Renderer2DStats 
	{
		uint32_t MaxVertices;
		uint32_t MaxIndices;

		uint32_t DrawCalls;

		uint32_t CurrentIndices;
		uint32_t QuadCount;

		uint32_t CurrentVertices() { return QuadCount * 4; }
	};
}