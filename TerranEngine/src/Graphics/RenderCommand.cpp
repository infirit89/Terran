#include "trpch.h"
#include "RenderCommand.h"

namespace TerranEngine 
{
	Shared<RendererAPI> RenderCommand::s_RendererAPI;

	void RenderCommand::Init(RendererAPI::API api)
	{
		s_RendererAPI = RendererAPI::Create(api);
		ToggleBlending(true);
		ToggleDepthTesting(true);
	}

	void RenderCommand::SetClearColor(float r, float g, float b, float a)
	{
		s_RendererAPI->SetClearColor(r, g, b, a);
	}
	void RenderCommand::Clear() 
	{
		s_RendererAPI->Clear();
	}

	void RenderCommand::Resize(int width, int height) 
	{
		s_RendererAPI->Resize(width, height);
	}

	void RenderCommand::ToggleWireframeMode(bool state)
	{
		s_RendererAPI->ToggleWireframeMode(state);
	}

	void RenderCommand::ToggleBlending(bool state) 
	{
		s_RendererAPI->ToggleBlending(state);
	}

	void RenderCommand::ToggleDepthTesting(bool state)
	{
		s_RendererAPI->ToggleDepthTesting(state);
	}

	void RenderCommand::Draw(RenderMode mode, const Shared<VertexArray>& vertexArray, int indexCount)
	{
		s_RendererAPI->Draw(mode, vertexArray, indexCount);
	}

	void RenderCommand::DrawArrays(RenderMode mode, int vertexCount)
	{
		s_RendererAPI->DrawArrays(mode, vertexCount);
	}

	void RenderCommand::SetLineWidth(float lineWidth)
	{
		s_RendererAPI->SetLineWidth(lineWidth);
	}

	uint32_t RenderCommand::GetAPIVersion() 
	{
		return s_RendererAPI->GetAPIVersion();
	}
}

