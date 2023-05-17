#include "trpch.h"

#include "VertexArray.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace TerranEngine 
{
	Shared<VertexArray> VertexArray::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL: return CreateShared<OpenGLVertexArray>();
		}

		TR_ASSERT(false, "Invalid renderer api");
		return nullptr;
	}
}
