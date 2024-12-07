#pragma once

#include "VertexArray.h"

#include "Core/Base.h"
#include "CommandQueue.h"

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

	struct RendererData final
	{
		Shared<CommandQueue> CreateResourceQueue;
		Shared<CommandQueue> RenderQueue;
		Shared<CommandQueue> FreeResourceQueue;
	};

	class Renderer final
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void ExecuteCommands();

		template<typename FuncT>
		static void SubmitCreate(FuncT&& func) 
		{
			s_RendererData->CreateResourceQueue->Submit(std::forward<FuncT>(func));
		}

		template<typename FuncT>
		static void Submit(FuncT&& func) 
		{
			s_RendererData->RenderQueue->Submit(std::forward<FuncT>(func));
		}

		template<typename FuncT>
		static void SubmitFree(FuncT&& func)
		{
			s_RendererData->FreeResourceQueue->Submit(std::forward<FuncT>(func));
		}

		static void SetClearColor(float r, float g, float b, float a);
		static void Clear();

		static void SetViewport(int width, int height);
		static void WireframeMode(bool enable);

		static void EnableBlending(bool state);

		static void EnableDepthTesting(bool state);

		static void DrawIndexed(RenderMode mode, const Shared<VertexArray>& vertexArray, int numIndices);
		static void DrawArrays(RenderMode mode, const Shared<VertexArray>& vertexArray, int numVertices);
		static void DrawInstanced(RenderMode mode, const Shared<VertexArray>& vertexArray, int instanceCount);
		static void SetLineWidth(float lineWidth);

		static uint32_t GetAPIVersion();

	private:
		static RendererData* s_RendererData;
	};
}