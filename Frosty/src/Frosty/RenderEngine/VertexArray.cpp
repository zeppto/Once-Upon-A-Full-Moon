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

	void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;		
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
	}

	VertexArray * VertexArray::Create()
	{
		return FY_NEW VertexArray();
	}
}