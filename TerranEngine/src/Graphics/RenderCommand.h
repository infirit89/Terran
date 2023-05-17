#pragma once

#include "RendererAPI.h"

namespace TerranEngine 
{
	
	class RenderCommand
	{
	public:
		static void Init(RendererAPI::API api);

		static void SetClearColor(float r, float g, float b, float a);
		static void Clear();

		static void Resize(int width, int height);
		static void ToggleWireframeMode(bool state);

		static void ToggleBlending(bool state);

		static void ToggleDepthTesting(bool state);

		static void Draw(RenderMode mode, const Shared<VertexArray>& vertexArray, int indexCount);
		static void DrawArrays(RenderMode mode, int vertexCount);
		static void SetLineWidth(float lineWidth);
		static uint32_t GetAPIVersion();

		static Shared<RendererAPI> GetRendererAPI() { return s_RendererAPI; }

	private:
		static Shared<RendererAPI> s_RendererAPI;
	};
}
