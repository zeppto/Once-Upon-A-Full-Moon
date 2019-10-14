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
		static void BeginScene();
		static void EndScene();

		static void SetCamera(const glm::vec3& pos, const glm::mat4& viewProjection);
		static void AddLight(const glm::vec3& pos, const glm::vec3& color, float radius, float strength, glm::vec3& direction);

		static void Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);

		inline static void Shutdown() { delete s_SceneData; }

	private:
		struct GameCameraProps
		{
			glm::vec3 CameraPosition;
			glm::mat4 ViewProjectionMatrix;
		};
		struct LightProps
		{
			glm::vec3 Position;
			glm::vec3 Color;
			float Radius;
			float Strength;
			glm::vec3 Direction;
		};

		struct SceneData
		{
			GameCameraProps GameCamera;
			LightProps Lights;
		};
		static SceneData* s_SceneData;

	};
}
#endif // !RENDERER_HPP