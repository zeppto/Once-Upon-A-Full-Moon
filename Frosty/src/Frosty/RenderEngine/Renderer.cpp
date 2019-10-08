#include "fypch.hpp"
#include "Glad/glad.h"
#include "Renderer.hpp"

namespace Frosty
{	
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;	
	
	void Renderer::InitScene(std::shared_ptr<Shader>& shader)
	{
		shader.reset(new Shader(FY_SHADER_VERTEX_SHADER, FY_SHADER_FRAGMENT_SHADER));
	}

	void Renderer::BeginScene(const std::shared_ptr<Camera>& m_Camera)
	{		
		m_SceneData->ViewProjectionMatrix = m_Camera->GetViewProjection();
		m_Camera->CameraPositionUpdate();
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
	
	void Renderer::DeleteSceneData()
	{
		if (m_SceneData != nullptr)
		{
			delete m_SceneData;
		}
	}
}
