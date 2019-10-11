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

	void Renderer::Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		mat->UseShader->Bind();
		mat->UseShader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		mat->UseShader->UploadUniformMat4("u_Transform", transform);
		if (mat->UseShader->GetName() == "FlatColor")
		{
			mat->UseShader->UploadUniformFloat4("u_Color", mat->Albedo);
		}

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}