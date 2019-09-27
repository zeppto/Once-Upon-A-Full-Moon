#ifndef RENDERER_HPP
#define RENDERER_HPP
#include "Frosty/RenderEngine/VertexArray.hpp"
#include "RenderCommand.hpp"

namespace Frosty
{
	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);		
	};
}
#endif // !RENDERER_HPP
