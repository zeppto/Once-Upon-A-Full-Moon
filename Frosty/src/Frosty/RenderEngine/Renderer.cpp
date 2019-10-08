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
		m_FrustumGrid.Update();

		//if (m_Camera->CameraPositionUpdate())
		//{
		//	m_FrustumGrid.Update();	// Lowers the FPS dramatically, BIG PROBLEM		~ W-_-W ~
		//}
	}

	void Renderer::EndScene()
	{
	}
	
	void Renderer::ShaderInit(std::shared_ptr<Shader>& shader)
	{
		//shader.reset(new Shader(FY_SHADER_VERTEX_SHADER, FY_SHADER_FRAGMENT_SHADER));
		shader.reset(new Shader(FY_SHADER_TESTLIGHT_VERTEX_SHADER, FY_SHADER_TESTLIGHT_FRAGMENT_SHADER));
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<LightManager>& lightManager)
	{
		shader->Bind();
		shader->UploadUniforMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);	
		
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
}