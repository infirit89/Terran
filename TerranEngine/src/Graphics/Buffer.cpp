#include "trpch.h"
#include "Buffer.h"

#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace TerranEngine 
{
	/* ---- Vertex Buffer Element --- */
	uint8_t VertexBufferElement::GetSize()
	{
		switch (Type)
		{
		case ShaderDataType::Float:
		case ShaderDataType::Int:		return 4;
		case ShaderDataType::Bool:		return 1;
		}

		TR_ASSERT(false, "No other type supported!");
		return 0;
	}
	/* ------------------------------*/

	/* ---- Vertex Buffer ---- */

	Shared<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL: return CreateShared<OpenGLVertexBuffer>(size);
		}

		TR_ASSERT(false, "Invalid renderer api");
		return nullptr;
	}

	Shared<VertexBuffer> VertexBuffer::Create(const float* vertices, uint32_t size)
	{
		switch (RendererAPI::GetAPI()) 
		{
		case RendererAPI::API::OpenGL: return CreateShared<OpenGLVertexBuffer>(vertices, size);
		}

		TR_ASSERT(false, "Invalid renderer api");
		return nullptr;
	}
	/* ----------------------- */


	/* ---- Index Buffer ---- */
	Shared<IndexBuffer> IndexBuffer::Create(const int* indices, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL: return CreateShared<OpenGLIndexBuffer>(indices, size);
		}

		TR_ASSERT(false, "Invalid renderer api");
		return nullptr;
	}
	/* ----------------------*/
}
