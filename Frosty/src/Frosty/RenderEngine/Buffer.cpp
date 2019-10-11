#include "fypch.hpp"
#include "Buffer.hpp"
#include <glad/glad.h>

namespace Frosty
{
	// Vertexbuffer --------------------------------------------------------------------

	VertexBuffer::VertexBuffer(const void * vertices, uint32_t size)
		:m_Size(size)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBuffer::SetNrOfVertices(uint32_t count)
	{
		m_NrOfVertices = count;
	}

	uint32_t VertexBuffer::GetNrOfVertices()const
	{
		return m_NrOfVertices;
	}

	uint32_t VertexBuffer::GetSize() const
	{
		return m_Size;
	}
	
	VertexBuffer * VertexBuffer::Create(const void * vertices, uint32_t size)
	{
		return new VertexBuffer(vertices, size);
	}

	// Indexbuffer --------------------------------------------------------------------

	IndexBuffer::IndexBuffer(const void* indices, uint32_t count)
		:	m_Count(count)
	{
		//glCreateBuffers(1, &m_RendererID);
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	uint32_t IndexBuffer::GetCount() const
	{
		return m_Count;
	}

	IndexBuffer * IndexBuffer::Create(const void * indices, uint32_t count)
	{
		return new IndexBuffer(indices, count);
	}	
}
