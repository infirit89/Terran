#pragma once

#include "Graphics/Buffer.h"

namespace TerranEngine 
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const float* vertices, uint32_t size);

		virtual ~OpenGLVertexBuffer() override;

		virtual void SetData(const void* vertices, uint32_t size) override;
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual uint32_t GetBufferID() const override { return m_BufferID; }

	private:
		uint32_t m_BufferID;
		uint32_t m_Size;
	};

	class OpenGLIndexBuffer : public IndexBuffer 
	{
	public:
		OpenGLIndexBuffer(const int* indices, uint32_t size);
		virtual ~OpenGLIndexBuffer() override;

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual uint32_t GetBufferID() const override { return m_BufferID; }

	private:
		uint32_t m_BufferID;
	};
}

