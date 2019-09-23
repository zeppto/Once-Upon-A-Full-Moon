#ifndef OPENGLBUFFER_HPP
#define OPENGLBUFFER_HPP
#include "Frosty/RenderEngine/Buffer.hpp"

namespace Frosty
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind()const;
		virtual void Unbind()const;
	private:
		uint32_t m_RendererID;
	};
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind()const;
		virtual void Unbind()const;

		virtual uint32_t GetSize()const { return m_Size; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
	};
}
#endif // !OPENGLBUFFER_HPP
