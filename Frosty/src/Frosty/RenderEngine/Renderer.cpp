#include "fypch.hpp"
#include "Glad/glad.h"
#include "Renderer.hpp"

namespace Frosty
{	
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;	
	FrustumGrid Renderer::m_FrustumGrid = FrustumGrid();
	
	void Renderer::InitScene(std::shared_ptr<Shader>& shader)
	{
		//shader.reset(new Shader(FY_SHADER_VERTEX_SHADER, FY_SHADER_FRAGMENT_SHADER));

		//shader.reset(Frosty::Shader::CreateShader("assets/shaders/Texture.glt"));
		shader.reset(Frosty::Shader::CreateShader("../../../Frosty/assets/shaders/TestShader.gls"));
		//shader.reset(Frosty::Shader::CreateShader("../../../Frosty/assets/shaders/TestShader2.gls"));
	}

	void Renderer::BeginScene(const std::shared_ptr<Camera>& m_Camera)
	{		
		m_SceneData->ViewProjectionMatrix = m_Camera->GetViewProjection();
		m_Camera->CameraPositionUpdate();	
		//m_FrustumGrid.Update();
	}

	void Renderer::EndScene()
	{
	}
	
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<LightManager>& lightManager)
	{
		shader->Bind();
		shader->UploadUniforMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);		

		//SubmitLightUniforms(shader, lightManager);
		//SubmitForwardPlusUniforms(shader);

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

	void Renderer::InitForwardPlus(std::shared_ptr<LightManager>& lightManager)
	{
		m_FrustumGrid.Initiate(lightManager);
		m_FrustumGrid.Update();
	}

	void Renderer::SubmitLightUniforms(const std::shared_ptr<Shader>& shader, const std::shared_ptr<LightManager>& lightManager)
	{
		// PointLights
		shader->UploadUniformUint("nrOfPointLights", lightManager->GetNrOfPointLights());
		for (int i = 0; i < lightManager->GetNrOfPointLights(); i++)
		{
			shader->UploadUniformFloat3Array(("pointLights[" + std::to_string(i) + "].Position").c_str(), lightManager->GetPointLightAt(i)->GetPosition());
			shader->UploadUniformFloat4Array(("pointLights[" + std::to_string(i) + "].Color").c_str(), lightManager->GetPointLightAt(i)->GetColor());
			shader->UploadUniformFloatArray(("pointLights[" + std::to_string(i) + "].Strength").c_str(), lightManager->GetPointLightAt(i)->GetStrength());
			shader->UploadUniformFloatArray(("pointLights[" + std::to_string(i) + "].Radius").c_str(), lightManager->GetPointLightAt(i)->GetRadius());
		}

		// DirectionalLights
		shader->UploadUniformUint("nrOfDirLights", lightManager->GetNrOfDirectionalLights());
		for (int i = 0; i < lightManager->GetNrOfDirectionalLights(); i++)
		{
			shader->UploadUniformFloat4Array(("dirLights[" + std::to_string(i) + "].Color").c_str(), lightManager->GetDirectionalLightAt(i)->GetColor());
			shader->UploadUniformFloatArray(("dirLights[" + std::to_string(i) + "].Strength").c_str(), lightManager->GetDirectionalLightAt(i)->GetStrength());
			shader->UploadUniformFloat3Array(("dirLights[" + std::to_string(i) + "].Direction").c_str(), lightManager->GetDirectionalLightAt(i)->GetDirection());
		}
	}

	void Renderer::SubmitForwardPlusUniforms(const std::shared_ptr<Shader>& shader)
	{
		// LightIndex
		for (int i = 0; i < int(m_FrustumGrid.GetLightIndexList().size()); i++)
		{
			shader->UploadUniformUint(("forwardPlus.LightIndexList[" + std::to_string(i) + "]").c_str(), m_FrustumGrid.GetLightIndexList().at(i));
		}

		// CellLightInfo
		for (int i = 0; i < m_FrustumGrid.GetNrOfGrids(); i++)
		{
			shader->UploadUniformFloat2(("forwardPlus.CellLightInfo[" + std::to_string(i) + "]").c_str(), m_FrustumGrid.GetCellLightInfoAt(i));
		}
	}
}
