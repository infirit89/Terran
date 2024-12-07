#pragma once

#include <glad/glad.h>

namespace TerranEngine 
{
	void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
}