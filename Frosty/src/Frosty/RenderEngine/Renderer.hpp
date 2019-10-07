#ifndef RENDERER_HPP
#define RENDERER_HPP
#include "Frosty/RenderEngine/VertexArray.hpp"
//#include "Frosty/Core/Camera/Camera.hpp"
#include "Frosty/RenderEngine/ForwardPlus.hpp"
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
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<LightManager> lightManager);
		static void DeleteSceneData();

		// Forward Plus
		static void InitForwardPlus(std::shared_ptr<LightManager>& lightManager);

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;			
		};
		static SceneData* m_SceneData;

		// Forward Plus
		static FrustumGrid m_FrustumGrid;
	};
}
#endif