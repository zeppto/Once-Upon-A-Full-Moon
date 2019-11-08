#include "fypch.hpp"
#include "Frosty/RenderEngine/VertexArray.hpp"

#include <glad/glad.h>

namespace Frosty
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType Type)
	{
		switch (Type)
		{
			case Frosty::ShaderDataType::None:		return GL_FLOAT;
			case Frosty::ShaderDataType::Float:		return GL_FLOAT;
			case Frosty::ShaderDataType::Float2:	return GL_FLOAT;
			case Frosty::ShaderDataType::Float3:	return GL_FLOAT;
			case Frosty::ShaderDataType::Float4:	return GL_FLOAT;
			case Frosty::ShaderDataType::Mat3:		return GL_FLOAT;
			case Frosty::ShaderDataType::Mat4:		return GL_FLOAT;
			case Frosty::ShaderDataType::Int:		return GL_INT;
			case Frosty::ShaderDataType::Int2:		return GL_INT;
			case Frosty::ShaderDataType::Int3:		return GL_INT;
			case Frosty::ShaderDataType::Int4:		return GL_INT;
			case Frosty::ShaderDataType::Bool:		return GL_BOOL;
		}

		FY_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &m_RendererID);		
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		FY_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");
	
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;		
		for (const auto& element : vertexBuffer->GetLayout())
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, element.GetElementSize(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				vertexBuffer->GetLayout().GetStride(),
				(const void*)element.Offset);
			index++;
		}

		m_VertexBuffer.push_back(vertexBuffer);
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, uint16_t enableAttribCount)
	{
		FY_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		for (uint16_t i = 0; i < enableAttribCount; i++) {
			BufferElement element = vertexBuffer->GetLayout().GetElements()[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, element.GetElementSize(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				vertexBuffer->GetLayout().GetElements()[i].Normalized ? GL_TRUE : GL_FALSE,
				vertexBuffer->GetLayout().GetStride(),
				(const void*)element.Offset);
		}

		m_VertexBuffer.push_back(vertexBuffer);
	}

	void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;		
	}

	void VertexArray::BindShaderStorageBuffer()
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_VertexBuffer[0]->GetID());
	}

	const std::vector<std::shared_ptr<VertexBuffer>>& VertexArray::GetVertexBuffer() 
	{
		return m_VertexBuffer;
	}

	const std::shared_ptr<IndexBuffer>& VertexArray::GetIndexBuffer()
	{
		return m_IndexBuffer;
	}

	void VertexArray::setMeshAnims(Luna::Animation* aPtr, std::vector<Luna::Joint> jVec, std::map<uint16_t, std::vector<Luna::Keyframe>>* kMap)
	{
		currentAnim = *aPtr;
		meshAnims.setAnimPtr(&currentAnim);
		meshAnims.setJointVec(jVec);
		meshAnims.setKeyframeMap(kMap);
	}

	void VertexArray::SetUniformBuffer(std::shared_ptr<UniformBuffer>& uniformBuffer)
	{
		glBindVertexArray(m_RendererID);
		uniformBuffer->Bind();

		m_UniformBuffer = uniformBuffer;
	}

	const std::shared_ptr<UniformBuffer>& VertexArray::getUniformBuffer()
	{
		return m_UniformBuffer;
	}

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
	void VertexArray::addHardcodedVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, size_t size)
	{
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		// this activates the first and second attributes of this VAO
		// think of "attributes" as inputs to the Vertex Shader
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);

		glVertexAttribPointer(
			0,							// location in shader
			3,							// how many elements of type (see next argument)
			GL_FLOAT,					// type of each element
			GL_FALSE,					// integers will be normalized to [-1,1] or [0,1] when read...
			size,		// distance between two vertices in memory (stride)
			BUFFER_OFFSET(0)			// offset of FIRST vertex in the list.
		);

		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			size,
			BUFFER_OFFSET(sizeof(float) * 3)
		);

		glVertexAttribPointer(
			2,
			3,
			GL_FLOAT,
			GL_FALSE,
			size,
			BUFFER_OFFSET(sizeof(float) * 5)
		);

		glVertexAttribPointer(
			3,
			3,
			GL_FLOAT,
			GL_FALSE,
			size,
			BUFFER_OFFSET(sizeof(float) * 8)
		);

		glVertexAttribPointer(
			4,
			3,
			GL_FLOAT,
			GL_FALSE,
			size,
			BUFFER_OFFSET(sizeof(float) * 11)
		);

		glVertexAttribPointer(
			5,
			4,
			GL_FLOAT,
			GL_FALSE,
			size,
			BUFFER_OFFSET(sizeof(float) * 14)
		);
		glVertexAttribIPointer(
			6,
			4,
			GL_INT,
			size,
			BUFFER_OFFSET(sizeof(float) * 14 + 4 * sizeof(int))
		);

		m_VertexBuffer.push_back(vertexBuffer);
	}

	VertexArray * VertexArray::Create()
	{
		return FY_NEW VertexArray();
	}
}