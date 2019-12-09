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

		if (type == STATIC) 
		{
			glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		}
		else if (type == DYNAMIC) 
		{
			glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STREAM_DRAW);
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
		return FY_NEW VertexBuffer(vertices, size, type);
	}

	VertexBuffer * VertexBuffer::Create()
	{
		return FY_NEW VertexBuffer();
	}

	void VertexBuffer::SetData(void * vertices, uint32_t size, BufferType type) //Used for updating data in buffers
	{
		if (type == BufferType::STATIC)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
			glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW); //Reset
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices); //Faster and more efficient than using glBufferData
		}
		else if (type == BufferType::DYNAMIC)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
			glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STREAM_DRAW); //Reset
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices); //Faster and more efficient than using glBufferData
		}
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
		return FY_NEW IndexBuffer(indices, count);
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
		return FY_NEW UniformBuffer(MaxBones);
	}

	ShaderStorageBuffer * ShaderStorageBuffer::Create(uint32_t size, void* data)
	{
		return FY_NEW ShaderStorageBuffer(size, data);
	}

	void ShaderStorageBuffer::SetData(uint32_t size)
	{
	}

	ShaderStorageBuffer::ShaderStorageBuffer(uint32_t size, void* data)
	{
		glGenBuffers(1, &m_ssboID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
	}

	ShaderStorageBuffer::~ShaderStorageBuffer()
	{
		glDeleteBuffers(1, &m_ssboID);
	}

	void ShaderStorageBuffer::Bind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboID);
	}

	void ShaderStorageBuffer::Unbind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	uint32_t ShaderStorageBuffer::GetSize() const
	{
		return m_Size;
	}

	uint32_t ShaderStorageBuffer::GetID() const
	{
		return m_ssboID;
	}
}
