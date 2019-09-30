#include "fypch.hpp"
#include "Glad/glad.h"
#include "Renderer.hpp"

namespace Frosty
{	
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(OrtoCamera& m_OrtoCamera)
	{
		m_SceneData->ViewProjectionMatrix = m_OrtoCamera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}
	
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		shader->Bind();
		shader->UploadUniforMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
				
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}
