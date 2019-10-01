#include "fypch.hpp"
#include "Buffer.hpp"
#include <glad/glad.h>
//#include<Luna/include/Luna.h>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{
	// Vertexbuffer --------------------------------------------------------------------

	VertexBuffer::VertexBuffer(float* vertices, uint32_t nrOfFloats)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, nrOfFloats * sizeof(float), vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::VertexBuffer(std::vector<Luna::Vertex>* vertices, uint32_t nrOfVertices)
	{
		
		glGenVertexArrays(1, &m_RendererID);
		glBindVertexArray(m_RendererID);

		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, nrOfVertices * sizeof(Luna::Vertex), vertices->data(), GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Luna::Vertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Luna::Vertex), BUFFER_OFFSET(sizeof(float) * 3));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Luna::Vertex), BUFFER_OFFSET(sizeof(float) * 5));

		glBindVertexArray(0);

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

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t count)
	{
		return new VertexBuffer(vertices, count);
	}

	VertexBuffer* VertexBuffer::Create(std::vector<Luna::Vertex>* vertices, uint32_t count)
	{
		return new VertexBuffer(vertices, count);
	}


	// Indexbuffer --------------------------------------------------------------------

	IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
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

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		return new IndexBuffer(indices, count);
	}
}