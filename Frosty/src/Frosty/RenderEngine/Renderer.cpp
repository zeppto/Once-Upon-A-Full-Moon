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



			// Point Lights
			shaderData->Shader->UploadUniformInt("u_TotalPointLights", (int)s_SceneData->PointLights.size());
			for (size_t i = 0; i < s_SceneData->PointLights.size(); i++)
			{
				shaderData->Shader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(i) + "].Color", s_SceneData->PointLights[i].Color);
				shaderData->Shader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(i) + "].Position", s_SceneData->PointLights[i].Position);
				shaderData->Shader->UploadUniformFloatArray("u_PointLights[" + std::to_string(i) + "].Radius", s_SceneData->PointLights[i].Radius);
				shaderData->Shader->UploadUniformFloatArray("u_PointLights[" + std::to_string(i) + "].Strength", s_SceneData->PointLights[i].Strength);
			}

			// Directional Lights
			shaderData->Shader->UploadUniformInt("u_TotalDirectionalLights", (int)s_SceneData->DirectionalLights.size());
			for (size_t i = 0; i < s_SceneData->DirectionalLights.size(); i++)
			{
				shaderData->Shader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(i) + "].Color", s_SceneData->DirectionalLights[i].Color);
				shaderData->Shader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(i) + "].Direction", s_SceneData->DirectionalLights[i].Direction);
				shaderData->Shader->UploadUniformFloatArray("u_DirectionalLights[" + std::to_string(i) + "].Strength", s_SceneData->DirectionalLights[i].Strength);
			}



			//For all Materials
			for (auto& MaterialIt : shaderData->MaterialMap)
			{
				nrOfMaterials++;

				auto& materialData = shaderData->MaterialMap.at(MaterialIt.first);


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

	Renderer::GameCameraProps Renderer::GetCamera()
	{
		return s_SceneData->GameCamera;
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

	void Renderer::SubmitText(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& text)
	{
		/*shader->Bind();
		vertexArray->Bind();

		glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
		glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f);

		shader->UploadUniforMat4("projection", projection);
		shader->UploadUniformInt("text", 1);
		shader->UploadUniformFloat3("textColor", color);

		std::string::const_iterator c;
		float x = 25.0f;
		float y = 24.0f;
		float scale = 1.0f;
		glm::vec3 vec = glm::vec3(0.5f, 0.8f, 0.2f);
		for (c = text.begin(); c != text.end(); c++) {
			Character ch = Assetmanager::GetAssetmanager()->GetFontMetaData("Gabriola")->GetData()->m_characters.at(*c);
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
			vertexArray->GetVertexBuffer().front()->SetData(*verts, sizeof(verts), GL_DYNAMIC_DRAW);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, ch.textureID);

			RenderCommand::Draw2D(vertexArray);

			x += (ch.advance >> 6) * scale;
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		vertexArray->Unbind();
		vertexArray->GetVertexBuffer().front()->Unbind();*/
	}

	void Renderer::SubmitParticles(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Shader>& computeShader, const std::shared_ptr<VertexArray>& vertexArray, glm::mat4& modelMat, size_t particleCount, float maxLifetime)
	{
		//computeShader->Bind();
		//vertexArray->BindShaderStorageBuffer();

		//computeShader->UploadUniformFloat("deltaTime", Frosty::Time::DeltaTime());
		//computeShader->UploadUniformFloat("maxLifetime", maxLifetime);

		//ComputeCommand::Send(particleCount);

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
		else if (mat->UseShader->GetName() == "Texture2D" || mat->UseShader->GetName() == "BlendShader")
		{
			mat->UseShader->UploadUniformFloat2("u_TextureCoordScale", mat->TextureScale);
		}
		vertexArray->Bind();
		RenderCommand::EnableBackfaceCulling();
		RenderCommand::Draw2D(vertexArray);
	}

	//For 2D, might be temp
	void Renderer::Submit2d(Texture2D* tex, Shader* shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_Transform", transform);


		vertexArray->Bind();

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		// Might wanna do this, not sure if needed: RenderCommand::DisableBackfaceCulling();
		RenderCommand::Draw2D(vertexArray);

		glDisable(GL_BLEND);
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
		RenderCommand::EnableBackfaceCulling();
		RenderCommand::Draw2D(vertexArray);
		dt += Frosty::Time::DeltaTime();
	}

}