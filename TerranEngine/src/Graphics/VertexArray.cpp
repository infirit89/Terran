#include "trpch.h"

#include "VertexArray.h"

#include "Scripting/ScriptCache.h"

#include <glad/glad.h>

#pragma warning (push)
#pragma warning (disable : 4312)

namespace TerranEngine 
{
	VertexArray::VertexArray()
		: m_AttributeIndex(0)
	{
		glCreateVertexArrays(1, &m_Handle);
	}

	VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_Handle); }

	const void VertexArray::Bind() const { glBindVertexArray(m_Handle); }
	const void VertexArray::Unbind() const { glBindVertexArray(0); }

	void VertexArray::AddVertexBufferLayout(const VertexBufferLayout& layout)
	{
		m_Layout = layout;

		for (auto element : m_Layout.GetElements())
		{
			glEnableVertexArrayAttrib(m_Handle, m_AttributeIndex);
			switch (element.Type)
			{
			case GL_INT:
				glVertexArrayAttribIFormat(
									m_Handle,
									m_AttributeIndex,
									element.Count,
									element.Type,
									element.Offset);
				break;
			case GL_FLOAT:
				glVertexArrayAttribFormat(
								m_Handle,
								m_AttributeIndex,
								element.Count,
								element.Type,
								element.Normalised ? GL_TRUE : GL_FALSE,
								element.Offset);
				break;
			}
			glVertexArrayAttribBinding(m_Handle, m_AttributeIndex, 0);
			m_AttributeIndex++;
		}
	}

	void VertexArray::AddVertexBuffer(const Shared<VertexBuffer>& buffer)
	{
		glVertexArrayVertexBuffer(m_Handle, 0, buffer->m_Handle, 0, m_Layout.GetStride());
	}

	void VertexArray::AddIndexBuffer(const Shared<IndexBuffer>& buffer)
	{
		glVertexArrayElementBuffer(m_Handle, buffer->m_Handle);
	}
}

#pragma warning (pop)
