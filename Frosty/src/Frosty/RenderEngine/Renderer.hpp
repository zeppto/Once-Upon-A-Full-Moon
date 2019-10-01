#ifndef RENDERER_HPP
#define RENDERER_HPP
#include "Frosty/RenderEngine/VertexArray.hpp"
#include "Frosty/Core/Camera/Camera.hpp"
#include "RenderCommand.hpp"
#include "Shader.hpp"

namespace Frosty
{
	class Renderer
	{
	public:
		static void BeginScene(const std::shared_ptr<Camera>& m_Camera);		
		static void EndScene();

		static void ShaderInit(std::shared_ptr<Shader>& shader);
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);		
		static void DeleteSceneData();
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;			
		};
		static SceneData* m_SceneData;
	};
}
#endif
