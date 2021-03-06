#ifndef VERTEXARRAY_HPP
#define VERTEXARRAY_HPP
#include "Frosty/RenderEngine/Buffer.hpp"
#include "Luna/include/Luna.h"

namespace Frosty
{
	class VertexArray
	{
	public:
		VertexArray();
		virtual ~VertexArray();
		bool operator==(const VertexArray& org) { return m_RendererID == org.m_RendererID; }


		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, uint16_t enableAttribCount); //Choose the number of attributes you want to enable
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

		//temp
		inline const std::string& GetName() { return m_LoadName; }
		//temp
		inline void SetName(const std::string& str) { m_LoadName = str; }

		void BindShaderStorageBuffer();

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffer();
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer();
		//TEMP FUNCS SHOULD BE MOVED AFTER ECS INTEGRATION
		void SetUniformBuffer(std::shared_ptr<UniformBuffer>& uniformBuffer);
		const std::shared_ptr<UniformBuffer>& GetUniformBuffer();
		void AddHardcodedVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, int size);
		void SetCurrentAnim(Luna::Animation &anim);
		Luna::Animation GetCurrentAnim();
		//TEMP FUNCS END
		static VertexArray* Create();

	private:
		//temp
		std::string m_LoadName;

		uint32_t m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		//TEMP VARIABLES SHOULD BE MOVED AFTER ECS INTEGRATION
		Luna::Animation currentAnim;
		std::shared_ptr<UniformBuffer> m_UniformBuffer;
	};
}
#endif
