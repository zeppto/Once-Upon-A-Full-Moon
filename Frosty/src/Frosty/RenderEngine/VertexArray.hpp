#ifndef VERTEXARRAY_HPP
#define VERTEXARRAY_HPP
#include "Frosty/RenderEngine/Buffer.hpp"

namespace Frosty
{
	class VertexArray
	{
	public:
		VertexArray();
		virtual ~VertexArray();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, uint16_t enableAttribCount); //Choose the number of attributes you want to enable
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

		void BindShaderStorageBuffer();

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffer();
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer();

		//Temp
		inline const uint32_t& GetRenderID() const { return m_RendererID; }

		static VertexArray* Create();
	private:
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
}
#endif
