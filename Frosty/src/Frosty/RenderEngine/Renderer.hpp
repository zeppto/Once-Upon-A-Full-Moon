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
		static void Init();

		static void BeginScene();
		static void EndScene();

		static void SetCamera(const glm::vec3& pos, const glm::mat4& viewProjection);
		static void AddLight(const glm::vec3& color, const glm::vec3& pos, float strength, float radius);
		static void AddLight(const glm::vec3& color, glm::vec3& direction, float strength);

		static void Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
		static void Submit2D(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& tex, glm::mat4& modelMatrix);
		static void SubmitText(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& text);
		static void SubmitParticles(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, glm::mat4& modelMat, size_t particleCount);

		inline static void Shutdown() { delete s_SceneData; }

	private:
		struct GameCameraProps
		{
			glm::vec3 CameraPosition;
			glm::mat4 ViewProjectionMatrix;
		};

		struct PointLight
		{
			glm::vec3 Position;
			glm::vec3 Color;
			float Strength;
			float Radius;
		};

		struct DirectionalLight
		{
			glm::vec3 Direction;
			glm::vec3 Color;
			float Strength;
		};

		struct SceneData
		{
			GameCameraProps GameCamera;
			std::vector<PointLight> PointLights;
			std::vector<DirectionalLight> DirectionalLights;
		};
		static SceneData* s_SceneData;

	};
}
#endif // !RENDERER_HPP