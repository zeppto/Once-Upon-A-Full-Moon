#ifndef RENDERER_HPP
#define RENDERER_HPP
#include "Frosty/RenderEngine/VertexArray.hpp"
#include "Frosty/Core/Camera/OrtoCamera.hpp"
#include "Frosty/Core/Camera/Camera.hpp"
#include "RenderCommand.hpp"
#include "Shader.hpp"

namespace Frosty
{
	class Renderer
	{
	public:
		//static void BeginScene(OrtoCamera& m_OrtoCamera);
		//static void BeginScene(const std::shared_ptr<Camera>& m_Camera);
		static void BeginScene(Camera* m_Camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
}
#endif
