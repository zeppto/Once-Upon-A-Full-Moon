#include "fypch.hpp"
#include "Glad/glad.h"
#include "Renderer.hpp"
#include "Frosty/RenderEngine/ForwardPlus.hpp"

namespace Frosty
{	
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
	FrustumGrid Renderer::m_FrustumGrid = FrustumGrid();

	void Renderer::BeginScene(const std::shared_ptr<Camera>& m_Camera)
	{		
		m_SceneData->ViewProjectionMatrix = m_Camera->GetViewProjection();		
		m_Camera->CameraPositionUpdate();	
	}

	void Renderer::EndScene()
	{
	}
	
	void Renderer::ShaderInit(std::shared_ptr<Shader>& shader)
	{
		//shader.reset(new Shader(FY_SHADER_VERTEX_SHADER, FY_SHADER_FRAGMENT_SHADER));
		shader.reset(new Shader(FY_SHADER_TESTLIGHT_VERTEX_SHADER, FY_SHADER_TESTLIGHT_FRAGMENT_SHADER));
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<LightManager> lightManager)
	{
		shader->Bind();
		shader->UploadUniforMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);	
		
		
		//shader->UploadUniformUint("nrOfPointLights", lightManager->GetNrOfPointLights());
		//for (int i = 0; i < lightManager->GetNrOfPointLights(); i++)
		//{
		//	shader->UploadUniformFloat3Array(("pointLights[" + std::to_string(i) + "].position").c_str(), lightManager->GetPointLightAt(i)->GetPosition());
		//	shader->UploadUniformFloat4Array(("pointLights[" + std::to_string(i) + "].color").c_str(), lightManager->GetPointLightAt(i)->GetColor());
		//	shader->UploadUniformFloatArray(("pointLights[" + std::to_string(i) + "].strength").c_str(), lightManager->GetPointLightAt(i)->GetStrength());
		//	shader->UploadUniformFloatArray(("pointLights[" + std::to_string(i) + "].radius").c_str(), lightManager->GetPointLightAt(i)->GetRadius());
		//}

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
