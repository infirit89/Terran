#include "trpch.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace TerranEngine 
{
	RendererAPI::API RendererAPI::s_API;
	Shared<RendererAPI> RendererAPI::Create(API api)
	{
		s_API = api;
		switch (api)
		{
		case RendererAPI::API::OpenGL: return CreateShared<OpenGLRendererAPI>();
		}

		TR_ASSERT(false, "Invalid renderer api");

		return nullptr;
	}
}