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
		struct CTransform;
		struct CLight;
		struct Entity;
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
		static void RenderScene();
		static void EndScene();

		static 	void Renderer::SetCamera(const glm::vec3& pos, const glm::mat4& view, const glm::mat4& projection);

		//static void AddLight(const int& ID, const glm::vec3& color, const glm::vec3& pos, float strength, float radius);
		static void AddLight(Frosty::ECS::CLight* light, ECS::CTransform* transform);
		static void UppdateLight(Frosty::ECS::CLight* light, ECS::CTransform* transform);
		static void RemoveLight(const std::shared_ptr<ECS::Entity>& entity);
		static void RemoveAllLights();

		static void Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
		static void AnimSubmit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
		//static void Submit2D(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& tex, glm::mat4& modelMatrix);
		static void SubmitText(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& text, glm::vec2 pos, glm::vec3 color, float scale);
		static void SubmitParticles(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, glm::mat4& modelMat, size_t particleCount, float maxLifetime);
		static void Submit2d(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
		static void SubmitHealthBar(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::vec3& translate, const glm::vec3& scale, const glm::vec3& HealthBarSpace);
		static void AddToRenderer(ECS::CMaterial* mat, std::shared_ptr<VertexArray> vertexArray, ECS::CTransform* transform);
		static void RemoveFromRenderer( const int& matID ,const std::string& meshName,const int& transformID);
		static void UppdateEntity (const int& matID,ECS::CMaterial* mat, const std::string& meshName, std::shared_ptr<VertexArray> vertexArray, const int& transformID, ECS::CTransform* transform);
		
		static void ChangeEntity (const int& OldMatID,ECS::CMaterial* mat, const std::string& OldMeshName, std::shared_ptr<VertexArray> vertexArray, const int& transformID, ECS::CTransform* transform);

		inline static void Shutdown() { delete s_SceneData; }

		inline static void SetDistanceCulling(bool& distanceCulling) { s_DistanceCulling = distanceCulling; }


	private:
	


		struct MeshData
		{
			std::shared_ptr<VertexArray> VertexArray;
			std::unordered_map<int, Frosty::ECS::CTransform*> TransformMap;
		};

		struct MaterialData
		{
			std::unordered_map<std::string, std::shared_ptr<MeshData>> MeshMap;

			Frosty::ECS::CMaterial* Material;
		};

		struct ShaderData
		{
			std::shared_ptr < Shader> Shader;
			std::unordered_map<int, std::shared_ptr<MaterialData>> MaterialMap;
		};

		static std::unordered_map<std::string, std::shared_ptr<ShaderData>> s_ShaderMap;
		static std::unordered_map<int, std::unordered_map<int, Frosty::ECS::CTransform*>*> s_TransformLookUpMap;
		static std::unordered_map<int, std::unordered_map<std::string, std::shared_ptr<MeshData>>*> s_MeshLookUpMap;
		static std::unordered_map<int, std::unordered_map<int, std::shared_ptr<MaterialData>>*> s_MaterialLookUpMap;

		struct PointLight
		{
			glm::vec3 Position;
			glm::vec3 Color;
			float Strength;
			float Radius;

			Frosty::ECS::CLight* PointLight;
			ECS::CTransform* Transform;
		};

		struct DirectionalLight
		{
			glm::vec3 Direction;
			glm::vec3 Color;
			float Strength;

			Frosty::ECS::CLight* DirectionalLight;
			ECS::CTransform* Transform;
		};

		struct SceneData
		{
			GameCameraProps GameCamera;
			//std::vector<PointLight> PointLights;
			std::unordered_map<int, PointLight>PointLights;

			//std::vector<DirectionalLight> DirectionalLights;
			std::unordered_map<int, DirectionalLight>DirectionalLights;
		};
		static SceneData* s_SceneData;

		static int s_TotalNrOfFrames;
		static bool s_DistanceCulling;
	};
}
#endif // !RENDERER_HPP