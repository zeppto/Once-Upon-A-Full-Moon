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
		static void InitScene(std::shared_ptr<Shader>& shader);
		static void BeginScene(const std::shared_ptr<Camera>& m_Camera);		
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);
		static void Submit2D(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& tex, glm::mat4& modelMatrix);
		static void SubmitText(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& text);
		static void SubmitParticles(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, glm::mat4& modelMat, size_t particleCount);

		static void DeleteSceneData();
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
			glm::mat4 ViewMatrix;
			glm::mat4 ProjectionMatrix;
		};
		static SceneData* m_SceneData;
	};
}
#endif
