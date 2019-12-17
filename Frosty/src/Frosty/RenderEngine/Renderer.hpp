#ifndef RENDERER_HPP
#define RENDERER_HPP
#include "RenderCommand.hpp"
#include "ComputeCommand.hpp"
#include "Texture.hpp"
#include "Frosty/RenderEngine/Shader.hpp"
#include "ForwardPlus.hpp"

namespace Frosty
{
	namespace ECS
	{
		struct CMaterial;
		struct CAnimController;
		struct CTransform;
		struct CLight;
		struct Entity;
		struct CMesh;
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

		struct PointLight
		{
			Frosty::ECS::CLight* PointLight;
			ECS::CTransform* Transform;
		};

		struct DirectionalLight
		{
			Frosty::ECS::CLight* DirectionalLight;
			ECS::CTransform* Transform;
		};

		static void Init();

		static void BeginScene();
		static void RenderScene();
		static void EndScene();

		static 	void Renderer::SetCamera(const glm::vec3& pos, const glm::mat4& view, const glm::mat4& projection);

		//static void AddLight(const int& ID, const glm::vec3& color, const glm::vec3& pos, float strength, float radius);
		static void AddLight(Frosty::ECS::CLight* light, ECS::CTransform* transform);
		static void UpdateLight(Frosty::ECS::CLight* light, ECS::CTransform* transform);
		static void RemoveLight(const std::shared_ptr<ECS::Entity>& entity);
		static void RemoveAllLights();
		static void ChangeLight(const std::shared_ptr<ECS::Entity>& entity);



		static void Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
		static void SubmitLightUniforms(ECS::CMaterial* mat);
		static void SubmitForwardPlusUniforms(ECS::CMaterial* mat);
		static void AnimSubmit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, ECS::CAnimController* controller);
		//static void Submit2D(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& tex, glm::mat4& modelMatrix);
		static void SubmitText(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& text, glm::vec2 pos, glm::vec3 color, float scale);
		static void SubmitSprite(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const uint32_t textureID, glm::vec4 color, glm::mat4 transform);
		static void SubmitParticles(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, glm::mat4& modelMat, size_t particleCount, float maxLifetime, unsigned int renderMode);
		static void Submit2d(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
		static void SubmitHealthBar(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::vec3& translate, const glm::vec3& scale, const glm::vec3& HealthBarSpace);
		static void AddToRenderer(ECS::CMaterial* mat, ECS::CMesh* mesh, ECS::CTransform* transform, ECS::CAnimController* controller);
		static void RemoveFromRenderer(const std::string& matID, const std::string& meshName, const size_t& transformID);
		static void UpdateEntity(const std::string& matID, ECS::CMaterial* mat, const std::string& meshName, std::shared_ptr<VertexArray> vertexArray, const size_t& transformID, ECS::CTransform* transform);

		static void SwapEntity(const std::shared_ptr<ECS::Entity>& EntityA, const std::shared_ptr<ECS::Entity>& EntityB);

		//Suggestion: Overload this for each individual trait. That way we need not replace everything.
		static void ChangeEntity(const std::string& OldMatID, ECS::CMaterial* mat, const std::string& OldMeshName, ECS::CMesh* mesh, const size_t& transformID, ECS::CTransform* transform, ECS::CAnimController* controller);
		static void UpdateCMesh(const size_t& entityID, ECS::CMesh* mesh);
		static void UpdateCMesh(const size_t& entityID, ECS::CMesh* mesh, ECS::CAnimController * ctrlPtr);

		inline static void Shutdown() { delete s_SceneData; }

		inline static void SetDistanceCulling(bool& distanceCulling) { s_DistanceCulling = distanceCulling; }
		inline static void SetLightCulling(bool& lightCulling) { s_LightCulling = lightCulling; }
		inline static void SetHeatMap(bool& heatMap) { s_HeatMap = heatMap; }

		inline static const GameCameraProps& GetGameCamera() { return s_SceneData->GameCamera; }
		inline static const std::unordered_map<size_t, PointLight>& GetPointLights() { return s_SceneData->PointLights; }

	private:



		struct MeshData
		{
			std::shared_ptr<VertexArray> VertexArray;
			std::unordered_map<size_t, Frosty::ECS::CTransform*> TransformMap;
			std::unordered_map<size_t, Frosty::ECS::CAnimController*> AnimMap;

			glm::mat4* parentMatrix = nullptr;
			const glm::mat4* holdJointTransform = nullptr;
		};

		struct MaterialData
		{
			std::unordered_map<std::string, std::shared_ptr<MeshData>> MeshMap;

			Frosty::ECS::CMaterial* Material;
		};

		struct ShaderData
		{
			std::shared_ptr < Shader> Shader;
			std::unordered_map<std::string, std::shared_ptr<MaterialData>> MaterialMap;
		};

		struct RenderPassData
		{
			std::unordered_map<std::string, std::shared_ptr<ShaderData>> ShaderMap;
			std::unordered_map<size_t, std::unordered_map<size_t, Frosty::ECS::CTransform*>*> TransformLookUpMap;
			std::unordered_map<size_t, std::unordered_map<std::string, std::shared_ptr<MeshData>>*> MeshLookUpMap;
			std::unordered_map<size_t, std::unordered_map<std::string, std::shared_ptr<MaterialData>>*> MaterialLookUpMap;
		};

		static std::unordered_map<std::string, std::shared_ptr<ShaderData>> s_ShaderMap;
		static std::unordered_map<size_t, std::unordered_map<size_t, Frosty::ECS::CTransform*>*> s_TransformLookUpMap;
		static std::unordered_map<size_t, std::unordered_map<std::string, std::shared_ptr<MeshData>>*> s_MeshLookUpMap;
		static std::unordered_map<size_t, std::unordered_map<std::string, std::shared_ptr<MaterialData>>*> s_MaterialLookUpMap;
		static std::vector<RenderPassData> s_RenderPas;

		struct SceneData
		{
			GameCameraProps GameCamera;
			//std::vector<PointLight> PointLights;
			std::unordered_map<size_t, PointLight>PointLights;

			//std::vector<DirectionalLight> DirectionalLights;
			std::unordered_map<size_t, DirectionalLight>DirectionalLights;
		};
		static SceneData* s_SceneData;

		static int s_TotalNrOfFrames;
		static bool s_DistanceCulling;
		static bool s_LightCulling;
		static bool s_HeatMap;

		static FrustumGrid s_ForwardPlus;
	};
}
#endif // !RENDERER_HPP