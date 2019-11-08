#include "fypch.hpp"
#include "Buffer.hpp"

#include <glad/glad.h>

namespace Frosty
{
	// Vertexbuffer --------------------------------------------------------------------

	VertexBuffer::VertexBuffer(const void* vertices, uint32_t size, BufferType type)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

		if (type == STATIC) {
			glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		}
		else if (type == DYNAMIC) {
			glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_COPY);
		}
	}

	VertexBuffer::VertexBuffer()
	{
		glGenBuffers(1, &m_RendererID);
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

	VertexBuffer * VertexBuffer::Create(const void * vertices, uint32_t size, BufferType type)
	{
		return new VertexBuffer(vertices, size, type);
	}

	VertexBuffer * VertexBuffer::Create()
	{
		return new VertexBuffer();
	}

	void VertexBuffer::SetData(float * vertices, uint32_t size, uint32_t type)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, type);
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

	uint32_t VertexBuffer::GetID() const
	{
		return m_RendererID;
	}


	// Indexbuffer --------------------------------------------------------------------
	IndexBuffer::IndexBuffer(const void* indices, uint32_t count)
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

	IndexBuffer* IndexBuffer::Create(const void* indices, uint32_t count)
	{
		return new IndexBuffer(indices, count);
	}

	// UniformBuffer --------------------------------------------------------------------
	UniformBuffer::UniformBuffer(int MaxBones)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * MaxBones , NULL, GL_DYNAMIC_DRAW);

	}
	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}
	
	void UniformBuffer::Bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
	}
	void UniformBuffer::BindUpdate(void * boneData, int len)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, len * sizeof(glm::mat4), boneData, GL_STATIC_DRAW);
	}

	void UniformBuffer::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	UniformBuffer* UniformBuffer::Create(int MaxBones)
	{
		return new UniformBuffer(MaxBones);
	}


}