#ifndef VERTEXARRAY_HPP
#define VERTEXARRAY_HPP
#include "Frosty/RenderEngine/Buffer.hpp"
//TEMP INCLUDE THIS SHOULD BE IN A MESH CLASS
#include "Frosty/API/AnimationHandler/AnimationHandler.h"

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
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffer();
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer();
		//TEMP FUNCS SHOULD BE MOVED TO MESH CLASS
		void setMeshAnims(Luna::Animation* aPtr, std::vector<Luna::Joint> jVec, 
			std::map<uint16_t, std::vector<Luna::Keyframe>>* kMap);
		AnimationHandler* getAnimationHandlerPtr() { return &meshAnims; }
		void SetUniformBuffer(std::shared_ptr<UniformBuffer>& uniformBuffer);
		const std::shared_ptr<UniformBuffer>& getUniformBuffer();
		void addHardcodedVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, size_t size);
		//TEMP FUNCS END
		static VertexArray* Create();
	private:
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		//TEMP VARIABLES SHOULD BE MOVED TO MESH CLASS
		AnimationHandler meshAnims;
		Luna::Animation currentAnim;
		std::shared_ptr<UniformBuffer> m_UniformBuffer;
	};
}
#endif
