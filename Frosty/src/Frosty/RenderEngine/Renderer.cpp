#include "fypch.hpp"
#include "Renderer.hpp"
#include "Frosty/Core/ECS.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"

#include <glad/glad.h>

namespace Frosty
{
	Renderer::SceneData* Renderer::s_SceneData = FY_NEW Renderer::SceneData;
	std::unordered_map<std::string, std::shared_ptr<Renderer::ShaderData>> Renderer::m_ShaderMap;
	std::unordered_map<int, std::unordered_map<int, Frosty::ECS::CTransform*>*> Renderer::m_TransformLookUpMap;
	std::unordered_map<int, std::unordered_map<std::string, std::shared_ptr<Renderer::MeshData>>*> Renderer::m_MeshLookUpMap;
	std::unordered_map<int, std::unordered_map<int, std::shared_ptr<Renderer::MaterialData>>*> Renderer::m_MaterialLookUpMap;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::BeginScene()
	{
	}



	void Renderer::RenderScene()
	{
		//For debugging
		int nrOfShaders = 0;
		int nrOfMaterials = 0;
		int nrOfMeshes = 0;
		int nrOfTransforms = 0;

		//For all shaders
		for (auto& ShaderIt : m_ShaderMap)
		{
			nrOfShaders++;

			auto& shaderData = m_ShaderMap.at(ShaderIt.first);
			shaderData->Shader->Bind();
			//Set most uniforms here
			shaderData->Shader->UploadUniformMat4("u_ViewProjection", s_SceneData->GameCamera.ViewProjectionMatrix);
			shaderData->Shader->UploadUniformFloat3("u_CameraPosition", s_SceneData->GameCamera.CameraPosition);

			if (shaderData->Shader->GetName() == "Texture2D")
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				RenderCommand::EnableBackfaceCulling();
			}

			// Point Lights
			shaderData->Shader->UploadUniformInt("u_TotalPointLights", (int)s_SceneData->PointLights.size());
			int PointLI = 0;
			for (auto& PLightIt : s_SceneData->PointLights)
			{

				shaderData->Shader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(PointLI) + "].Color", s_SceneData->PointLights[PLightIt.first]->PointLight->Color);
				shaderData->Shader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(PointLI) + "].Position", s_SceneData->PointLights[PLightIt.first]->Transform->Position);
				shaderData->Shader->UploadUniformFloatArray("u_PointLights[" + std::to_string(PointLI) + "].Radius", s_SceneData->PointLights[PLightIt.first]->PointLight->Radius);
				shaderData->Shader->UploadUniformFloatArray("u_PointLights[" + std::to_string(PointLI) + "].Strength", s_SceneData->PointLights[PLightIt.first]->PointLight->Strength);
				PointLI++;
			}

			// Directional Lights
			shaderData->Shader->UploadUniformInt("u_TotalDirectionalLights", (int)s_SceneData->DirectionalLights.size());
			int DirectLI = 0;
			for (auto& DLightIt : s_SceneData->DirectionalLights)
			{
				shaderData->Shader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(DirectLI) + "].Color", s_SceneData->DirectionalLights[DLightIt.first]->DirectionalLight->Color);
				shaderData->Shader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(DirectLI) + "].Direction", s_SceneData->DirectionalLights[DLightIt.first]->DirectionalLight->Direction);
				shaderData->Shader->UploadUniformFloatArray("u_DirectionalLights[" + std::to_string(DirectLI) + "].Strength", s_SceneData->DirectionalLights[DLightIt.first]->DirectionalLight->Strength);
				DirectLI++;
			}

			



			//For all Materials
			for (auto& MaterialIt : shaderData->MaterialMap)
			{
				nrOfMaterials++;

				auto& materialData = shaderData->MaterialMap.at(MaterialIt.first);

				shaderData->Shader->UploadUniformInt("u_Shininess", materialData->Material->Shininess);


				if (shaderData->Shader->GetName() == "FlatColor")
				{
					shaderData->Shader->UploadUniformFloat4("u_ObjectColor", materialData->Material->Albedo);
					shaderData->Shader->UploadUniformFloat("u_SpecularStrength", materialData->Material->SpecularStrength);

				}
				else if (shaderData->Shader->GetName() == "Texture2D" || shaderData->Shader->GetName() == "BlendShader")
				{
					shaderData->Shader->UploadUniformFloat2("u_TextureCoordScale", materialData->Material->TextureScale);
				}

				//Bind all Textures
				if (materialData->Material->DiffuseTexture != nullptr)
				{
					materialData->Material->DiffuseTexture->Bind(0);
				}
				if (materialData->Material->NormalTexture != nullptr)
				{
					materialData->Material->NormalTexture->Bind(1);
				}

				//For all Meshes
				for (auto& MeshIt : materialData->MeshMap)
				{
					nrOfMeshes++;

					auto& meshData = materialData->MeshMap.at(MeshIt.first);
					meshData->VertexArray->Bind();

					//For all Transforms
					for (auto& TransformIt : meshData->TransformMap)
					{
						nrOfTransforms++;


						shaderData->Shader->UploadUniformMat4("u_Transform", *meshData->TransformMap.at(TransformIt.first)->GetModelMatrix());
						RenderCommand::Draw2D(meshData->VertexArray);
					}

					glBindVertexArray(0);
				}

				glBindTexture(GL_TEXTURE_2D, 0);
			}
			glUseProgram(0);

			if (shaderData->Shader->GetName() == "Texture2D")
			{
				glDisable(GL_BLEND);
				RenderCommand::DisableBackfaceCulling();
			}
		}
		int test = 0;
	}

	void Renderer::EndScene()
	{
		/*s_SceneData->PointLights.clear();
		s_SceneData->DirectionalLights.clear();*/
	}

	void Renderer::SetCamera(const glm::vec3& pos, const glm::mat4& view, const glm::mat4& projection)
	{
		s_SceneData->GameCamera.CameraPosition = pos;
		s_SceneData->GameCamera.ViewMatrix = view;
		s_SceneData->GameCamera.ProjectionMatrix = projection;
		s_SceneData->GameCamera.ViewProjectionMatrix = projection * view;
	}

	void Renderer::AddLight(Frosty::ECS::CLight* light, ECS::CTransform* transform)
	{
		if (light->Type == Frosty::ECS::CLight::LightType::Point)
		{
			s_SceneData->PointLights.emplace(light->EntityPtr->Id, FY_NEW PointLight);
			s_SceneData->PointLights.at(light->EntityPtr->Id)->PointLight = light;
			s_SceneData->PointLights.at(light->EntityPtr->Id)->Transform = transform;
		}
		else if (light->Type == Frosty::ECS::CLight::LightType::Directional)
		{
			glm::mat4 mat = glm::mat4(1.0f);
			mat = glm::rotate(mat, glm::radians(light->Direction.x), { 1.0f, 0.0f, 0.0f });
			mat = glm::rotate(mat, glm::radians(light->Direction.x), { 0.0f, 1.0f, 0.0f });
			mat = glm::rotate(mat, glm::radians(light->Direction.x), { 0.0f, 0.0f, 1.0f });

			light->Direction = mat * glm::vec4(light->Direction, 0);


			s_SceneData->DirectionalLights.emplace(light->EntityPtr->Id, FY_NEW DirectionalLight);
			s_SceneData->DirectionalLights.at(light->EntityPtr->Id)->DirectionalLight = light;
			s_SceneData->DirectionalLights.at(light->EntityPtr->Id)->Transform = transform;
		}
	}

	void Renderer::UppdateLight(Frosty::ECS::CLight* light, ECS::CTransform* transform)
	{
		if (light->Type == Frosty::ECS::CLight::LightType::Point)
		{
			s_SceneData->PointLights.at(light->EntityPtr->Id)->PointLight = light;
			s_SceneData->PointLights.at(light->EntityPtr->Id)->Transform = transform;
		}
		else if (light->Type == Frosty::ECS::CLight::LightType::Directional)
		{

			s_SceneData->DirectionalLights.at(light->EntityPtr->Id)->DirectionalLight = light;
			s_SceneData->DirectionalLights.at(light->EntityPtr->Id)->Transform = transform;
		}
	}

	void Renderer::RemoveLight(Frosty::ECS::CLight* light)
	{
	/*	if (light->Type == Frosty::ECS::CLight::LightType::Point)
		{
			s_SceneData->PointLights.erase(light->EntityPtr->Id);
			
		}
		else if (light->Type == Frosty::ECS::CLight::LightType::Directional)
		{
			s_SceneData->DirectionalLights.erase(light->EntityPtr->Id);
		}*/
	}

	

	/*void Renderer::AddLight(const glm::vec3& color, const glm::vec3& pos, float strength, float radius)
	{
		PointLight light;
		light.Color = color;
		light.Position = pos;
		light.Strength = strength;
		light.Radius = radius;
		s_SceneData->PointLights.emplace_back(light);
	}*/

	/*void Renderer::AddLight(const glm::vec3& color, glm::vec3& direction, float strength)
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
	}*/

	void Renderer::SubmitText(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& text, glm::vec2 pos, glm::vec3 color, float scale)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		shader->Bind();
		vertexArray->Bind();

		float width = 1280.0f;
		float height = 720.0f;
		glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height);

		shader->UploadUniformMat4("projection", projection);
		shader->UploadUniformInt("text", 0); //Make sure this number matches the active and sampled texture
		shader->UploadUniformFloat3("textColor", color);

		std::string::const_iterator c;
		float x = pos.x;
		float y = pos.y;
		for (c = text.begin(); c != text.end(); c++) {
			Character ch = Frosty::AssetManager::GetTTF("Gabriola")->m_characters.at(*c); //TODO: Switch out for actual font provided by system
			float xpos = x + ch.bearing.x * scale;
			float ypos = y - (ch.size.y - ch.bearing.y) * scale;
			float width = ch.size.x * scale;
			float height = ch.size.y * scale;

			float verts[6][4]
			{
				{ xpos,			ypos + height,	0.0f, 0.0f },
				{ xpos,			ypos,			0.0f, 1.0f },
				{ xpos + width,	ypos,			1.0f, 1.0f },

				{ xpos,			ypos + height,	0.0f, 0.0f },
				{ xpos + width, ypos,			1.0f, 1.0f },
				{ xpos + width, ypos + height,	1.0f, 0.0f }
			};

			vertexArray->GetVertexBuffer().front()->Bind();
			vertexArray->GetVertexBuffer().front()->SetData(*verts, sizeof(verts), Frosty::BufferType::DYNAMIC);


			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, ch.textureID);

			RenderCommand::DrawUIText(vertexArray); //Will probably change later

			x += (ch.advance >> 6) * scale;
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		shader->UnBind();
		vertexArray->Unbind();
		glDisable(GL_BLEND);
	}

	void Renderer::SubmitParticles(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, glm::mat4& modelMat, size_t particleCount, float maxLifetime)
	{
		shader->Bind();
		vertexArray->Bind();

		shader->UploadUniformMat4("viewMat", s_SceneData->GameCamera.ViewMatrix);
		shader->UploadUniformMat4("projectionMat", s_SceneData->GameCamera.ProjectionMatrix);
		shader->UploadUniformMat4("modelMat", modelMat);

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		RenderCommand::DisableBackfaceCulling();
		RenderCommand::DrawParticles(vertexArray, particleCount);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		vertexArray->Unbind();
		shader->UnBind();
	}

	void Renderer::Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//mat->UseShader->Bind();
		//mat->UseShader->UploadUniformMat4("u_ViewProjection", s_SceneData->GameCamera.ViewProjectionMatrix);
		//mat->UseShader->UploadUniformMat4("u_Transform", transform);
		//mat->UseShader->UploadUniformFloat3("u_CameraPosition", s_SceneData->GameCamera.CameraPosition);
		//mat->UseShader->UploadUniformInt("u_Shininess", mat->Shininess);

		//// Point Lights
		//mat->UseShader->UploadUniformInt("u_TotalPointLights", (int)s_SceneData->PointLights.size());
		//for (size_t i = 0; i < s_SceneData->PointLights.size(); i++)
		//{
		//	mat->UseShader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(i) + "].Color", s_SceneData->PointLights[i].Color);
		//	mat->UseShader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(i) + "].Position", s_SceneData->PointLights[i].Position);
		//	mat->UseShader->UploadUniformFloatArray("u_PointLights[" + std::to_string(i) + "].Radius", s_SceneData->PointLights[i].Radius);
		//	mat->UseShader->UploadUniformFloatArray("u_PointLights[" + std::to_string(i) + "].Strength", s_SceneData->PointLights[i].Strength);
		//}

		//// Directional Lights
		//mat->UseShader->UploadUniformInt("u_TotalDirectionalLights", (int)s_SceneData->DirectionalLights.size());
		//for (size_t i = 0; i < s_SceneData->DirectionalLights.size(); i++)
		//{
		//	mat->UseShader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(i) + "].Color", s_SceneData->DirectionalLights[i].Color);
		//	mat->UseShader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(i) + "].Direction", s_SceneData->DirectionalLights[i].Direction);
		//	mat->UseShader->UploadUniformFloatArray("u_DirectionalLights[" + std::to_string(i) + "].Strength", s_SceneData->DirectionalLights[i].Strength);
		//}

		//if (mat->UseShader->GetName() == "FlatColor")
		//{
		//	mat->UseShader->UploadUniformFloat4("u_ObjectColor", mat->Albedo);
		//	mat->UseShader->UploadUniformFloat("u_SpecularStrength", mat->SpecularStrength);

		//}
		//else if (mat->UseShader->GetName() == "Texture2D" || mat->UseShader->GetName() == "BlendShader")
		//{
		//	mat->UseShader->UploadUniformFloat2("u_TextureCoordScale", mat->TextureScale);
		//}
		//vertexArray->Bind();
		//RenderCommand::EnableBackfaceCulling();
		//RenderCommand::Draw2D(vertexArray);

		//glDisable(GL_BLEND);
	}

	//For 2D, might be temp
	void Renderer::Submit2d(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{

		shader->Bind();
		vertexArray->Bind();

		shader->UploadUniformMat4("u_Transform", transform);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		RenderCommand::DisableBackfaceCulling();
		RenderCommand::Draw2D(vertexArray);

		glDisable(GL_BLEND);

		vertexArray->Unbind();
		shader->UnBind();

	}

	void Renderer::SubmitHealthBar(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::vec3& translate, const glm::vec3& scale, const glm::vec3& HealthBarSpace)
	{
		shader->Bind();
		vertexArray->Bind();
		
		shader->UploadUniformFloat3("u_HealthBarSpace", HealthBarSpace);

		shader->UploadUniformFloat3("u_Translate", translate);
		shader->UploadUniformFloat3("u_Scale", scale);

		shader->UploadUniformMat4("u_ViewProjection", s_SceneData->GameCamera.ViewProjectionMatrix);
		shader->UploadUniformMat4("u_View", s_SceneData->GameCamera.ViewMatrix);

		shader->UploadUniformFloat3("u_CameraRight_worldspace", glm::vec3(s_SceneData->GameCamera.ViewMatrix[0][0], s_SceneData->GameCamera.ViewMatrix[1][0], s_SceneData->GameCamera.ViewMatrix[2][0]));
		shader->UploadUniformFloat3("u_CameraUp_worldspace", glm::vec3(s_SceneData->GameCamera.ViewMatrix[0][1], s_SceneData->GameCamera.ViewMatrix[1][1], s_SceneData->GameCamera.ViewMatrix[2][1]));

		RenderCommand::DisableBackfaceCulling();
		RenderCommand::Draw2D(vertexArray);

		vertexArray->Unbind();
		shader->UnBind();
	}

	int counter = 0;

	void Renderer::AddToRenderer(ECS::CMaterial* mat, std::shared_ptr<VertexArray> vertexArray, ECS::CTransform* transform)
	{
		counter++;
		//Set up IDs
		int matID = transform->EntityPtr->Id; //Works but can be improved whith a real material ID
		std::string meshID = vertexArray->GetName();
		int transformID = transform->EntityPtr->Id;


		//Check if the shader key is already in the map, if not add it.
		std::string ShaderName = mat->UseShader->GetName();
		if (m_ShaderMap.find(mat->UseShader->GetName()) == m_ShaderMap.end())
		{
			m_ShaderMap.emplace(mat->UseShader->GetName(), FY_NEW ShaderData);

		}
		auto& shaderData = m_ShaderMap.at(ShaderName);
		shaderData->Shader = mat->UseShader;



		//Check if the material key is already in the map, if not add it.
		if (shaderData->MaterialMap.find(matID) == shaderData->MaterialMap.end())
		{
			shaderData->MaterialMap.emplace(matID, FY_NEW MaterialData);
		}
		auto& materialData = shaderData->MaterialMap.at(matID);
		materialData->Material = mat;

		//Add the material to the MaterialLookUpMap
		m_MaterialLookUpMap.emplace(transformID, &shaderData->MaterialMap);

		//Check if the mesh key is already in the map, if not add it.
		if (materialData->MeshMap.find(meshID) == materialData->MeshMap.end())
		{
			materialData->MeshMap.emplace(meshID, FY_NEW  MeshData);
		}
		auto& meshData = materialData->MeshMap.at(meshID);
		meshData->VertexArray = vertexArray;
		meshData->TransformMap.emplace(transformID, transform);

		//Add the mesh to the MeshLookUpMap
		m_MeshLookUpMap.emplace(transformID, &materialData->MeshMap);

		//Add the tranform to the TransformLookUpMap
		auto& transformMap = meshData->TransformMap;
		m_TransformLookUpMap.emplace(transformID, &transformMap);

		m_ShaderMap;
	}

	void Renderer::RemoveFromRenderer(const int& matID, const std::string& meshName, const int& transformID)
	{
		if (m_TransformLookUpMap.find(transformID) != m_TransformLookUpMap.end())
		{
			m_ShaderMap;

			int nrOfTransforms = 0;
			int nrOfMeshes = 0;

			m_TransformLookUpMap.at(transformID)->at(transformID) = nullptr;
			m_TransformLookUpMap.at(transformID)->erase(transformID);

			if (m_TransformLookUpMap.at(transformID)->size() == 0)
			{
				nrOfTransforms = m_TransformLookUpMap.at(transformID)->size();
				m_MeshLookUpMap.at(transformID)->erase(meshName);
			}

			if (m_MeshLookUpMap.at(transformID)->size() == 0)
			{
				nrOfMeshes = m_MeshLookUpMap.at(transformID)->size();
				m_MaterialLookUpMap.at(transformID)->erase(matID);

				//m_MaterialLookUpMap.erase(transformID);

			}

			if (nrOfTransforms == 0)
			{
				m_MeshLookUpMap.erase(transformID);
			}

			if (nrOfMeshes == 0)
			{
				m_MaterialLookUpMap.erase(transformID);
			}


			m_TransformLookUpMap.erase(transformID);

		}

	}

	void Renderer::UpdateEntity(const int& matID, ECS::CMaterial* mat, const std::string& meshName, std::shared_ptr<VertexArray> vertexArray, const int& transformID, ECS::CTransform* transform)
	{
		if (m_TransformLookUpMap.find(transformID) != m_TransformLookUpMap.end())
		{
			m_ShaderMap;

			m_TransformLookUpMap.at(transformID)->at(transformID) = transform;
			m_MeshLookUpMap.at(transformID)->at(meshName)->VertexArray = vertexArray;
			m_MaterialLookUpMap.at(transformID)->at(matID)->Material = mat;
		}
	}

	



	float dt = 0;

	void Renderer::AnimSubmit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		mat->UseShader->Bind();
		mat->UseShader->UploadUniformMat4("u_ViewProjection", s_SceneData->GameCamera.ViewProjectionMatrix);
		mat->UseShader->UploadUniformMat4("u_Transform", transform);
		mat->UseShader->AssignUniformBlock("a_jointDataBlock");

		void* skinDataPtr = nullptr;
		int nrOfBones = 0;
		AssetManager::GetAnimation(vertexArray->GetCurrentAnim().animationName)->CalculateAnimMatrix(&dt);
		AssetManager::GetAnimation(vertexArray->GetCurrentAnim().animationName)->GetSkinData(skinDataPtr, nrOfBones);

		vertexArray->GetUniformBuffer()->BindUpdate(skinDataPtr, nrOfBones);

		vertexArray->Bind();
		/*RenderCommand::EnableBackfaceCulling();*/
		RenderCommand::Draw2D(vertexArray);
		dt += Frosty::Time::DeltaTime();
	}

}