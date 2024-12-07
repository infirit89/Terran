#include "trpch.h"

#include "VertexArray.h"
#include "Renderer.h"

#include <glad/glad.h>

#pragma warning (push)
#pragma warning (disable : 4312)

namespace TerranEngine 
{
	namespace 
	{
		uint32_t GetNativeType(VertexBufferElementType type) 
		{
			switch (type)
			{
			case VertexBufferElementType::Float: return GL_FLOAT;
			case VertexBufferElementType::Int: return GL_INT;
			}

			TR_ASSERT(false, "Unrecognized vertex buffer element type");

			return GL_FLOAT;
		}
	}

	VertexArray::VertexArray()
		: m_Handle(0), m_AttributeIndex(0)
	{
		Renderer::SubmitCreate([this]()
		{
			glCreateVertexArrays(1, &m_Handle);
		});
	}

	VertexArray::~VertexArray() 
	{
		Release();
	}

	void VertexArray::Bind() 
	{
		Renderer::Submit([this]() 
		{
			glBindVertexArray(m_Handle);
		});
	}
	void VertexArray::Unbind() 
	{ 
		Renderer::Submit([]()
		{
			glBindVertexArray(0);
		});
	}

	void VertexArray::AddVertexBuffer(const Shared<VertexBuffer>& buffer)
	{
		const auto& layout = buffer->GetLayout();

		Renderer::SubmitCreate([this, layout, buffer]()
		{
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
		});
	}

	void VertexArray::AddIndexBuffer(const Shared<IndexBuffer>& buffer)
	{
		Renderer::SubmitCreate([this, buffer]()
		{
			glVertexArrayElementBuffer(m_Handle, buffer->m_Handle);
		});
	}
	void VertexArray::Release()
	{
		Renderer::SubmitFree([this]()
		{
			glDeleteVertexArrays(1, &m_Handle);
		});
	}
}

#pragma warning (pop)
