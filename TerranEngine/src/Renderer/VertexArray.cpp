#include "trpch.h"

#include "VertexArray.h"

#include <glad/glad.h>

#pragma warning (push)
#pragma warning (disable : 4312)

namespace TerranEngine 
{
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

	const void VertexArray::Bind() const
	{
		glBindVertexArray(m_Vao);
	}

	const void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBufferLayout(const VertexBufferLayout& layout)
	{
		m_Layout = layout;

		for (auto element : m_Layout.GetElements())
		{
			glEnableVertexAttribArray(m_ElementIndex);
			glVertexAttribPointer(m_ElementIndex, element.Count, element.Type, element.Normalised ? GL_TRUE : GL_FALSE, m_Layout.GetStride(), (const void*)element.Offset);
			m_ElementIndex++;
		}
	}
}

#pragma warning (pop)
