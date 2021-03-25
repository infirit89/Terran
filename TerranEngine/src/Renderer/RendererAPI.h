#pragma once

namespace TerranEngine 
{

	struct RendererAPI 
	{
		enum class API
		{
			None = 0,
			OpenGL
		};

		static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
}
