#ifndef RENDERER_HPP
#define RENDERER_HPP
#include "RenderCommand.hpp"
#include "ComputeCommand.hpp"
#include "Texture.hpp"
#include "Frosty/RenderEngine/Shader.hpp"

namespace Frosty
{
	namespace ECS
	{
		struct CMaterial;
	}

	class Renderer
	{
	protected:


	public:
		struct GameCameraProps
		{
			glm::vec3 CameraPosition;
			glm::mat4 ViewMatrix;
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewProjectionMatrix;
		};

		static void Init();

		static void BeginScene();
		static void EndScene();

		static 	void Renderer::SetCamera(const glm::vec3& pos, const glm::mat4& view, const glm::mat4& projection);
		static GameCameraProps GetCamera();

		static void AddLight(const glm::vec3& color, const glm::vec3& pos, float strength, float radius);
		static void AddLight(const glm::vec3& color, glm::vec3& direction, float strength);

		static void Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
		static void AnimSubmit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
		//static void Submit2D(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& tex, glm::mat4& modelMatrix);
		static void SubmitText(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& text, glm::vec2 pos);
		static void SubmitParticles(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, glm::mat4& modelMat, size_t particleCount, float maxLifetime);
		static void Submit2d(Texture2D* tex, const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);


		inline static void Shutdown() { delete s_SceneData; }

	private:
	

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