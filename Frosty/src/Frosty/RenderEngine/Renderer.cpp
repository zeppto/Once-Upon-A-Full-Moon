#include "fypch.hpp"
#include "Renderer.hpp"
#include "Frosty/Core/ECS.hpp"

#include <glad/glad.h>

namespace Frosty
{
	Renderer::SceneData* Renderer::s_SceneData = FY_NEW Renderer::SceneData;

	void Renderer::BeginScene(const glm::mat4& viewProjection)
	{
		s_SceneData->ViewProjectionMatrix = viewProjection;
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::AddLight(const glm::vec3& pos, const glm::vec3& color)
	{
		s_SceneData->Lights.Position = pos;
		s_SceneData->Lights.Color = color;
	}

	void Renderer::Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		mat->UseShader->Bind();
		mat->UseShader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		mat->UseShader->UploadUniformMat4("u_Transform", transform);
		if (mat->UseShader->GetName() == "FlatColor")
		{
			mat->UseShader->UploadUniformFloat4("u_ObjectColor", mat->Albedo);
			//mat->UseShader->UploadUniformFloat3("u_CameraPosition", s_SceneData->Lights.Position);
			//mat->UseShader->UploadUniformFloat3("u_LightColor", s_SceneData->Lights.Color);
		}
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	float dt = 0;

	void Renderer::animSubmit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		mat->UseShader->Bind();
		mat->UseShader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		mat->UseShader->UploadUniformMat4("u_Transform", transform);
		mat->UseShader->AssignUniformBlock("a_jointDataBlock");

		vertexArray->getAnimationHandlerPtr()->CalculateAnimMatrix(&dt);
		int MaxBones = 64;
		vertexArray->getUniformBuffer()->BindUpdate(vertexArray->getAnimationHandlerPtr()->getSkinData(), MaxBones);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		dt += 0.003;
	}

}