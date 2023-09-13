#include "trpch.h"

#include "VertexArray.h"

#include <glad/glad.h>

#pragma warning (push)
#pragma warning (disable : 4312)

namespace TerranEngine 
{
	uint32_t GetNativeType(VertexBufferElementType type) 
	{
		switch (type)
		{
		case TerranEngine::VertexBufferElementType::Float: return GL_FLOAT;
		case TerranEngine::VertexBufferElementType::Int: return GL_INT;
		}

		TR_ASSERT(false, "Unrecognized vertex buffer element type");
	}

	VertexArray::VertexArray()
		: m_AttributeIndex(0)
	{
		glCreateVertexArrays(1, &m_Handle);
	}

	VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_Handle); }

	const void VertexArray::Bind() const { glBindVertexArray(m_Handle); }
	const void VertexArray::Unbind() const { glBindVertexArray(0); }

	void VertexArray::AddVertexBuffer(const Shared<VertexBuffer>& buffer)
	{
		const auto& layout = buffer->GetLayout();

		for (auto element : layout.GetElements())
		{
			uint32_t nativeType = GetNativeType(element.Type);
			glEnableVertexArrayAttrib(m_Handle, m_AttributeIndex);
			switch (nativeType)
			{
			case GL_INT:
				glVertexArrayAttribIFormat(
					m_Handle,
					m_AttributeIndex,
					element.Count,
					nativeType,
					element.Offset);
				break;
			case GL_FLOAT:
				glVertexArrayAttribFormat(
					m_Handle,
					m_AttributeIndex,
					element.Count,
					nativeType,
					element.Normalised ? GL_TRUE : GL_FALSE,
					element.Offset);
				break;
			}
			glVertexArrayAttribBinding(m_Handle, m_AttributeIndex, 0);
			m_AttributeIndex++;
		}

		glVertexArrayVertexBuffer(m_Handle, 0, buffer->m_Handle, 0, layout.GetStride());
	}

	void VertexArray::AddIndexBuffer(const Shared<IndexBuffer>& buffer)
	{
		glVertexArrayElementBuffer(m_Handle, buffer->m_Handle);
	}
}

#pragma warning (pop)
