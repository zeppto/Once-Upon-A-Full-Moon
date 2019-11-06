#ifndef RENDERCOMMAND_HPP
#define RENDERCOMMAND_HPP

#include "Frosty/RenderEngine/VertexArray.hpp"

namespace Frosty
{
	class RenderCommand
	{
	public:
		static void Init();

		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		//static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray);
		static void Draw2D(const std::shared_ptr<VertexArray>& vertexArray);
		static void DrawParticles(const std::shared_ptr<VertexArray>& vertexArray, size_t count);
		//static void DrawUIText();

	};
}
#endif // !RENDERCOMMAND_HPP