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
		static void ChangeLight(const std::shared_ptr<ECS::Entity>& entity);



		static void Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
		static void AnimSubmit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, ECS::CAnimController * controller);
		//static void Submit2D(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& tex, glm::mat4& modelMatrix);
		static void SubmitText(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& text, glm::vec2 pos, glm::vec3 color, float scale);
		static void SubmitParticles(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, glm::mat4& modelMat, size_t particleCount, float maxLifetime);
		static void Submit2d(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
		static void SubmitHealthBar(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::vec3& translate, const glm::vec3& scale, const glm::vec3& HealthBarSpace);
		static void AddToRenderer(ECS::CMaterial* mat, ECS::CMesh * mesh, ECS::CTransform* transform, ECS::CAnimController* anim);
		static void RemoveFromRenderer( const size_t& matID ,const std::string& meshName, const size_t& transformID);
		static void UppdateEntity (const size_t& matID,ECS::CMaterial* mat, const std::string& meshName, std::shared_ptr<VertexArray> vertexArray, const size_t& transformID, ECS::CTransform* transform);
		
		static void ChangeEntity (const size_t& OldMatID,ECS::CMaterial* mat, const std::string& OldMeshName, ECS::CMesh* vertexArray, const size_t& transformID, ECS::CTransform* transform);

		inline static void Shutdown() { delete s_SceneData; }

		inline static void SetDistanceCulling(bool& distanceCulling) { s_DistanceCulling = distanceCulling; }


	private:



		struct MeshData
		{
			std::shared_ptr<VertexArray> VertexArray;
			std::unordered_map<size_t, Frosty::ECS::CTransform*> TransformMap;

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
			std::unordered_map<size_t, std::shared_ptr<MaterialData>> MaterialMap;
		};

		struct RenderPassData
		{
			static std::unordered_map<std::string, std::shared_ptr<ShaderData>> ShaderMap;
			static std::unordered_map<size_t, std::unordered_map<size_t, Frosty::ECS::CTransform*>*> TransformLookUpMap;
			static std::unordered_map<size_t, std::unordered_map<std::string, std::shared_ptr<MeshData>>*> MeshLookUpMap;
			static std::unordered_map<size_t, std::unordered_map<size_t, std::shared_ptr<MaterialData>>*> MaterialLookUpMap;
		};

		static std::unordered_map<std::string, std::shared_ptr<ShaderData>> s_ShaderMap;
		static std::unordered_map<size_t, std::unordered_map<size_t, Frosty::ECS::CTransform*>*> s_TransformLookUpMap;
		static std::unordered_map<size_t, std::unordered_map<std::string, std::shared_ptr<MeshData>>*> s_MeshLookUpMap;
		static std::unordered_map<size_t, std::unordered_map<size_t, std::shared_ptr<MaterialData>>*> s_MaterialLookUpMap;
		static std::vector<RenderPassData> s_RenderPas;

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
			std::unordered_map<size_t, PointLight>PointLights;

			//std::vector<DirectionalLight> DirectionalLights;
			std::unordered_map<size_t, DirectionalLight>DirectionalLights;
		};
		static SceneData* s_SceneData;

		static int s_TotalNrOfFrames;
		static bool s_DistanceCulling;
	};
}
#endif // !RENDERER_HPP