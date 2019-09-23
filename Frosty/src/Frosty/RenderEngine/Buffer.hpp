#ifndef BUFFER_HPP
#define BUFFER_HPP

namespace Frosty
{
	class VertexBuffer
	{
	public:
		VertexBuffer(float* vertices, uint32_t size);
		virtual ~VertexBuffer();

		void Bind()const;
		void Unbind()const;

		static VertexBuffer* Create(float* vertices, uint32_t size);
	private:
		uint32_t m_RendererID;
	};
	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~IndexBuffer();

		void Bind()const;
		void Unbind()const;

		uint32_t GetSize()const;

		static IndexBuffer* Create(uint32_t* indices, uint32_t size);
	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
	};
}
#endif // !BUFFER_HPP
