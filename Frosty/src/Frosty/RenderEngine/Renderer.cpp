#include "fypch.hpp"
#include "Renderer.hpp"
#include "Frosty/Core/ECS.hpp"

#include <glad/glad.h>

namespace Frosty
{
	Renderer::SceneData* Renderer::s_SceneData = FY_NEW Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
		s_SceneData->PointLights.clear();
		s_SceneData->DirectionalLights.clear();
	}

	void Renderer::SetCamera(const glm::vec3& pos, const glm::mat4& viewProjection)
	{
		s_SceneData->GameCamera.CameraPosition = pos;
		s_SceneData->GameCamera.ViewProjectionMatrix = viewProjection;
	}

	void Renderer::AddLight(const glm::vec3& color, const glm::vec3& pos, float strength, float radius)
	{
		PointLight light;
		light.Color = color;
		light.Position = pos;
		light.Strength = strength;
		light.Radius = radius;
		s_SceneData->PointLights.emplace_back(light);
	}

	void Renderer::AddLight(const glm::vec3& color, glm::vec3& direction, float strength)
	{
		glm::mat4 mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(direction.x), { 1.0f, 0.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(direction.y), { 0.0f, 1.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(direction.z), { 0.0f, 0.0f, 1.0f });
		
		DirectionalLight light;
		light.Color = color;
		light.Direction = mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0);
		light.Strength = strength;
		s_SceneData->DirectionalLights.emplace_back(light);
	}

	void Renderer::Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		mat->UseShader->Bind();
		mat->UseShader->UploadUniformMat4("u_ViewProjection", s_SceneData->GameCamera.ViewProjectionMatrix);
		mat->UseShader->UploadUniformMat4("u_Transform", transform);
		mat->UseShader->UploadUniformFloat3("u_CameraPosition", s_SceneData->GameCamera.CameraPosition);
		mat->UseShader->UploadUniformInt("u_Shininess", mat->Shininess);

		// Point Lights
		mat->UseShader->UploadUniformInt("u_TotalPointLights", (int)s_SceneData->PointLights.size());
		for (size_t i = 0; i < s_SceneData->PointLights.size(); i++)
		{
			mat->UseShader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(i) + "].Color", s_SceneData->PointLights[i].Color);
			mat->UseShader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(i) + "].Position", s_SceneData->PointLights[i].Position);
			mat->UseShader->UploadUniformFloatArray("u_PointLights[" + std::to_string(i) + "].Radius", s_SceneData->PointLights[i].Radius);
			mat->UseShader->UploadUniformFloatArray("u_PointLights[" + std::to_string(i) + "].Strength", s_SceneData->PointLights[i].Strength);
		}

		// Directional Lights
		mat->UseShader->UploadUniformInt("u_TotalDirectionalLights", (int)s_SceneData->DirectionalLights.size());
		for (size_t i = 0; i < s_SceneData->DirectionalLights.size(); i++)
		{
			mat->UseShader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(i) + "].Color", s_SceneData->DirectionalLights[i].Color);
			mat->UseShader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(i) + "].Direction", s_SceneData->DirectionalLights[i].Direction);
			mat->UseShader->UploadUniformFloatArray("u_DirectionalLights[" + std::to_string(i) + "].Strength", s_SceneData->DirectionalLights[i].Strength);
		}

		if (mat->UseShader->GetName() == "FlatColor")
		{
			mat->UseShader->UploadUniformFloat4("u_ObjectColor", mat->Albedo);
			mat->UseShader->UploadUniformFloat("u_SpecularStrength", mat->SpecularStrength);

		}
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}