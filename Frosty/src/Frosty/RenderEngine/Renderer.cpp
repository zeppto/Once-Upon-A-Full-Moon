#include "fypch.hpp"
#include "Renderer.hpp"
#include "Frosty/Core/ECS.hpp"

#include <glad/glad.h>

namespace Frosty
{
	Renderer::SceneData* Renderer::s_SceneData = FY_NEW Renderer::SceneData;

	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::SetCamera(const glm::vec3& pos, const glm::mat4& viewProjection)
	{
		s_SceneData->GameCamera.CameraPosition = pos;
		s_SceneData->GameCamera.ViewProjectionMatrix = viewProjection;
	}

	void Renderer::AddLight(const glm::vec3& pos, const glm::vec3& color, float radius, float strength, glm::vec3& direction)
	{
		s_SceneData->Lights.Position = pos;
		s_SceneData->Lights.Color = color;
		s_SceneData->Lights.Radius = radius;
		s_SceneData->Lights.Strength = strength;
		s_SceneData->Lights.Direction = direction;
	}

	void Renderer::Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		mat->UseShader->Bind();
		mat->UseShader->UploadUniformMat4("u_ViewProjection", s_SceneData->GameCamera.ViewProjectionMatrix);
		mat->UseShader->UploadUniformMat4("u_Transform", transform);
		if (mat->UseShader->GetName() == "FlatColor")
		{
			mat->UseShader->UploadUniformFloat3("u_CameraPosition", s_SceneData->GameCamera.CameraPosition);
			mat->UseShader->UploadUniformFloat4("u_ObjectColor", mat->Albedo);
			mat->UseShader->UploadUniformFloat3("u_LightPosition", s_SceneData->Lights.Position);
			mat->UseShader->UploadUniformFloat3("u_LightColor", s_SceneData->Lights.Color);
			mat->UseShader->UploadUniformFloat("u_SpecularStrength", mat->SpecularStrength);
			mat->UseShader->UploadUniformInt("u_Shininess", mat->Shininess);
			mat->UseShader->UploadUniformFloat("u_Radius", s_SceneData->Lights.Radius);
			mat->UseShader->UploadUniformFloat("u_Strength", s_SceneData->Lights.Strength);

			mat->UseShader->UploadUniformFloat3("u_LightDirection", s_SceneData->Lights.Direction);

		}
		else if (mat->UseShader->GetName() == "Texture2D")
		{
			mat->UseShader->UploadUniformFloat3("u_CameraPosition", s_SceneData->GameCamera.CameraPosition);
			mat->UseShader->UploadUniformFloat3("u_LightPosition", s_SceneData->Lights.Position);
			mat->UseShader->UploadUniformFloat3("u_LightColor", s_SceneData->Lights.Color);
			mat->UseShader->UploadUniformInt("u_Shininess", mat->Shininess);
		}

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}