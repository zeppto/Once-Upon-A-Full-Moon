#ifndef RENDERCOMMAND_HPP
#define RENDERCOMMAND_HPP

#include "Frosty/RenderEngine/VertexArray.hpp"

namespace Frosty
{
	class RenderCommand
	{
	public:
		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray);
	private:
		//static Renderer* m_Renderer;
	};
}
#endif // !RENDERCOMMAND_HPP
