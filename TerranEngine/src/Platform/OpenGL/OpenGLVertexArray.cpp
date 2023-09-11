#include "trpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	namespace Utility
	{
		static GLenum ShaderDataTypeToNativeType(ShaderDataType type)
		{
			switch (type)
			{
			case ShaderDataType::Float: return GL_FLOAT;
			case ShaderDataType::Int:	return GL_INT;
			case ShaderDataType::Bool:	return GL_BOOL;
			}

			TR_ASSERT(false, "Unknown data type");
			return 0;
		}
	}

	OpenGLVertexArray::OpenGLVertexArray()
		: m_ElementIndex(0)
	{
		glGenVertexArrays(1, &m_ArrayID);
		glBindVertexArray(m_ArrayID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_ArrayID);
	}

	void OpenGLVertexArray::Bind()
	{
		glBindVertexArray(m_ArrayID);
	}

	void OpenGLVertexArray::Unbind()
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBufferLayout(const VertexBufferLayout& layout)
	{
		for (const auto& element : layout.GetElements())
		{
			glEnableVertexAttribArray(m_ElementIndex);
			GLenum dataType = Utility::ShaderDataTypeToNativeType(element.Type);
			switch (element.Type)
			{
			case ShaderDataType::Int:
				glVertexAttribIPointer(m_ElementIndex, element.Count, dataType, layout.GetStride(), (const void*)element.Offset);
				break;
			case ShaderDataType::Float:
				glVertexAttribPointer(m_ElementIndex, element.Count, dataType, element.Normalised ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.Offset);
				break;
			}
			m_ElementIndex++;
		}
	}
}
