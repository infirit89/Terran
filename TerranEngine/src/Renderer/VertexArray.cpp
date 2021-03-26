#include "trpch.h"

#include "VertexArray.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	int ConvertToBaseOpenGL(BufferElementType::Type type) 
	{
		switch (type)
		{
		case BufferElementType::Type::FLOAT:	return GL_FLOAT;
		case BufferElementType::Type::FLOAT2:	return GL_FLOAT;
		case BufferElementType::Type::FLOAT3:	return GL_FLOAT;
		case BufferElementType::Type::INT:		return GL_INT;
		case BufferElementType::Type::INT2:		return GL_INT;
		case BufferElementType::Type::INT3:		return GL_INT;
		case BufferElementType::Type::BOOL:		return GL_BOOL;
		default:
			TR_ASSERT(false, "No other type supported!");
		}
	}

	VertexArray::VertexArray()
		: m_ElementIndex(0)
	{
		glGenVertexArrays(1, &m_Vao);
		glBindVertexArray(m_Vao);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_Vao);
	}

	void VertexArray::Bind()
	{
		glBindVertexArray(m_Vao);
	}

	void VertexArray::Unbind()
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(VertexBuffer& buffer)
	{
		m_VBuffer = buffer;

		VertexBufferLayout layout = buffer.GetLayout();

		for (auto element : layout.GetElements()) 
		{
			glEnableVertexAttribArray(m_ElementIndex);
			glVertexAttribPointer(m_ElementIndex, element.GetCountOfType(), ConvertToBaseOpenGL(element.Type), element.Normalised ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.Offset);
			m_ElementIndex++;
		}
	}
}
