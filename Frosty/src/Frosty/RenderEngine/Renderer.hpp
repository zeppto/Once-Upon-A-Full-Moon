#ifndef RENDERER_HPP
#define RENDERER_HPP
#include "RenderCommand.hpp"

#include "Frosty/RenderEngine/Shader.hpp"

namespace Frosty
{
	namespace ECS
	{
		struct CMaterial;
	}

	class Renderer
	{
	public:
		static void BeginScene(const glm::mat4& viewProjection);
		static void EndScene();

		static void Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);

		inline static void Shutdown() { delete s_SceneData; }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};
		static SceneData* s_SceneData;

	};
}
#endif // !RENDERER_HPP