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
		glCreateVertexArrays(1, &m_ArrayID);
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

	void OpenGLVertexArray::AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer, const VertexBufferLayout& layout)
	{
		for (const auto& element : layout.GetElements())
		{
			glEnableVertexArrayAttrib(m_ArrayID, m_ElementIndex);
			glVertexArrayAttribBinding(m_ArrayID, m_ElementIndex, 0);
			GLenum dataType = Utility::ShaderDataTypeToNativeType(element.Type);
			switch (element.Type)
			{
			case ShaderDataType::Int:
				glVertexArrayAttribIFormat(m_ArrayID, m_ElementIndex, element.Count, dataType, element.Offset);
				break;
			case ShaderDataType::Float:
				glVertexArrayAttribFormat(m_ArrayID, m_ElementIndex, element.Count, dataType, element.Normalised ? GL_TRUE : GL_FALSE, element.Offset);
				break;
			}
			m_ElementIndex++;
		}

		// TODO: maybe make it possible to add more buffer bindings?
		glVertexArrayVertexBuffer(m_ArrayID, 0, vertexBuffer->GetBufferID(), 0, layout.GetStride());
	}

	void OpenGLVertexArray::AddIndexBuffer(const Shared<IndexBuffer>& buffer)
	{
		glVertexArrayElementBuffer(m_ArrayID, buffer->GetBufferID());
	}
}
