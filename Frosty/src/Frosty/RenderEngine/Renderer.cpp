#include "fypch.hpp"
#include "Renderer.hpp"
#include "Frosty/Core/ECS.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Core/Application.hpp"
#include <glad/glad.h>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{
	Renderer::SceneData* Renderer::s_SceneData = FY_NEW Renderer::SceneData;
	std::unordered_map<std::string, std::shared_ptr<Renderer::ShaderData>> Renderer::s_ShaderMap;
	std::unordered_map<size_t, std::unordered_map<size_t, Frosty::ECS::CTransform*>*> Renderer::s_TransformLookUpMap;
	std::unordered_map<size_t, std::unordered_map<std::string, std::shared_ptr<Renderer::MeshData>>*> Renderer::s_MeshLookUpMap;
	std::unordered_map<size_t, std::unordered_map<size_t, std::shared_ptr<Renderer::MaterialData>>*> Renderer::s_MaterialLookUpMap;
	std::vector<Renderer::RenderPassData>  Renderer::s_RenderPas;
	FrustumGrid Renderer::s_ForwardPlus;
	int Renderer::s_TotalNrOfFrames;
	bool Renderer::s_DistanceCulling;
	bool Renderer::s_LightCulling;
	bool Renderer::s_RenderShadows;
	bool Renderer::s_RenderFromPointLight;
	unsigned int Renderer::s_ShadowMapFBO;
	unsigned int Renderer::s_ShadowMap;
	unsigned int Renderer::s_FullScreenQuad;



	void Renderer::Init()
	{
		RenderCommand::Init();

		for (uint8_t i = 0; i < 4; i++)
		{
			s_RenderPas.emplace_back(RenderPassData());
		}

		s_ForwardPlus.Initiate();
		CreateDepthMap();
		CreateFullScreenQuad();

		s_RenderShadows = true;
		s_DistanceCulling = true;
	}

	void Renderer::BeginScene()
	{
		if (s_LightCulling)
		{
			s_ForwardPlus.Update();
		}
	}

	void Renderer::RenderScene()
	{
		//For debugging
		int nrOfpasses = 0;
		int nrOfShaders = 0;
		int nrOfMaterials = 0;
		int nrOfMeshes = 0;
		int nrOfTransforms = 0;
		int nrOfDrawnedObjs = 0;
		int nrOfCulledObjs = 0;
		bool culling = false;

		//For all render passes 
		for (int i = 0; i < s_RenderPas.size(); i++)
		{
			nrOfpasses++;
			auto passData = s_RenderPas[i];

			if (i != 0)
			{
				glm::ivec4 viewP(Application::Get().GetWindow().GetViewport());
				glViewport(viewP.x, viewP.y, viewP.z, viewP.w);
			}





			//For all shaders
			for (auto& ShaderIt : passData.ShaderMap)
			{
				nrOfShaders++;

				auto& shaderData = passData.ShaderMap.at(ShaderIt.first);
				shaderData->Shader->Bind();
				//Set most uniforms here

				shaderData->Shader->UploadUniformFloat3("u_CameraPosition", s_SceneData->GameCamera.CameraPosition);

				if (s_RenderFromPointLight)
				{
					for (auto& DLightIt : s_SceneData->DirectionalLights)
					{
						shaderData->Shader->UploadUniformMat4("u_ViewProjection", s_SceneData->DirectionalLights[DLightIt.first].DirectionalLight->Cameras[0].ViewProjectionMatrix);
					}
				}
				else
				{
					shaderData->Shader->UploadUniformMat4("u_ViewProjection", s_SceneData->GameCamera.ViewProjectionMatrix);
				}







				if (shaderData->Shader->GetName() == "Texture2D")
				{
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					RenderCommand::EnableBackfaceCulling();
					culling = true;

					glActiveTexture(GL_TEXTURE10);
					glBindTexture(GL_TEXTURE_2D, s_ShadowMap);
				}
				else if (shaderData->Shader->GetName() == "Animation")
				{
					shaderData->Shader->AssignUniformBlock("a_jointDataBlock");
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					RenderCommand::EnableBackfaceCulling();
					culling = true;
				}
				else if (shaderData->Shader->GetName() == "FlatColor")
				{
					RenderCommand::EnableBackfaceCulling();
					culling = true;
				}
				else if (shaderData->Shader->GetName() == "BlendShader")
				{
					RenderCommand::EnableBackfaceCulling();
					culling = true;
					glActiveTexture(GL_TEXTURE10);
					glBindTexture(GL_TEXTURE_2D, s_ShadowMap);
				}
				else if (shaderData->Shader->GetName() == "ShadowMap")
				{


					RenderCommand::EnableBackfaceCulling();
					culling = true;

					glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
					glBindFramebuffer(GL_FRAMEBUFFER, s_ShadowMapFBO);
					glClear(GL_DEPTH_BUFFER_BIT);
					for (auto& DLightIt : s_SceneData->DirectionalLights)
					{
						shaderData->Shader->UploadUniformMat4("u_LightSpaceMatrix", s_SceneData->DirectionalLights[DLightIt.first].DirectionalLight->Cameras[0].ViewProjectionMatrix);

					}

				}
				else
				{
					culling = false;
				}

				// Point Lights
				shaderData->Shader->UploadUniformInt("u_TotalPointLights", (int)s_SceneData->PointLights.size());
				int PointLI = 0;
				for (auto& PLightIt : s_SceneData->PointLights)
				{
					shaderData->Shader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(PointLI) + "].Color", s_SceneData->PointLights[PLightIt.first].PointLight->Color);
					shaderData->Shader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(PointLI) + "].Position", s_SceneData->PointLights[PLightIt.first].Transform->Position);
					shaderData->Shader->UploadUniformFloatArray("u_PointLights[" + std::to_string(PointLI) + "].Radius", s_SceneData->PointLights[PLightIt.first].PointLight->Radius);
					shaderData->Shader->UploadUniformFloatArray("u_PointLights[" + std::to_string(PointLI) + "].Strength", s_SceneData->PointLights[PLightIt.first].PointLight->Strength);
					PointLI++;
				}

				// Directional Lights  
				shaderData->Shader->UploadUniformInt("u_TotalDirectionalLights", (int)s_SceneData->DirectionalLights.size());
				int DirectLI = 0;
				for (auto& DLightIt : s_SceneData->DirectionalLights)
				{
					shaderData->Shader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(DirectLI) + "].Color", s_SceneData->DirectionalLights[DLightIt.first].DirectionalLight->Color);
					shaderData->Shader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(DirectLI) + "].Position", s_SceneData->DirectionalLights[DLightIt.first].Transform->Position);
					shaderData->Shader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(DirectLI) + "].Direction", s_SceneData->DirectionalLights[DLightIt.first].DirectionalLight->Direction);
					shaderData->Shader->UploadUniformFloatArray("u_DirectionalLights[" + std::to_string(DirectLI) + "].Strength", s_SceneData->DirectionalLights[DLightIt.first].DirectionalLight->Strength);
					shaderData->Shader->UploadUniformMat4("u_DirectionalLights[" + std::to_string(DirectLI) + "].LightSpaceMatrix", s_SceneData->DirectionalLights[DLightIt.first].DirectionalLight->Cameras[0].ViewProjectionMatrix);

					DirectLI++;
				}

				shaderData->Shader->UploadUniformInt("u_LightCulling", s_LightCulling);
				if (s_LightCulling)
				{
					// LightIndex
					for (int i = 0; i < int(s_ForwardPlus.GetLightIndexList().size()); i++)
					{
						shaderData->Shader->UploadUniformInt(("forwardPlus.LightIndexList[" + std::to_string(i) + "]").c_str(), s_ForwardPlus.GetLightIndexList()[i]);
					}

					// CellLightInfo
					for (int i = 0; i < s_ForwardPlus.GetNrOfGrids(); i++)
					{
						shaderData->Shader->UploadUniformFloat2(("forwardPlus.CellLightInfo[" + std::to_string(i) + "]").c_str(), s_ForwardPlus.GetCellLightInfoAt(i));
					}
				}

				for (auto& MaterialIt : shaderData->MaterialMap)
				{
					nrOfMaterials++;

					auto& materialData = shaderData->MaterialMap.at(MaterialIt.first);

					shaderData->Shader->UploadUniformInt("u_Shininess", materialData->Material->Shininess);


					if (shaderData->Shader->GetName() == "FlatColor" || shaderData->Shader->GetName() == "HeatMap")
					{
						shaderData->Shader->UploadUniformFloat4("u_ObjectColor", materialData->Material->Albedo);
						shaderData->Shader->UploadUniformFloat("u_SpecularStrength", materialData->Material->SpecularStrength);

					}
					else if (shaderData->Shader->GetName() == "Animation")
					{
						shaderData->Shader->UploadUniformFloat("u_Flash", materialData->Material->Flash);
						shaderData->Shader->UploadUniformFloat2("u_TextureCoordScale", materialData->Material->TextureScale);
						shaderData->Shader->UploadUniformInt("u_RenderShadows", s_RenderShadows);

					}
					else if (shaderData->Shader->GetName() == "Texture2D" || shaderData->Shader->GetName() == "BlendShader")
					{
						shaderData->Shader->UploadUniformFloat2("u_TextureCoordScale", materialData->Material->TextureScale);
						shaderData->Shader->UploadUniformInt("u_RenderShadows", s_RenderShadows);
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
					if (materialData->Material->SpecularTexture != nullptr)
					{
						materialData->Material->SpecularTexture->Bind(2);
					}
					if (materialData->Material->BlendMapTexture != nullptr)
					{
						materialData->Material->BlendMapTexture->Bind(3);
					}
					if (materialData->Material->BlendTexture1 != nullptr)
					{
						materialData->Material->BlendTexture1->Bind(4);
					}
					if (materialData->Material->BlendTexture1 != nullptr)
					{
						materialData->Material->BlendTexture1->Bind(5);
					}




					if (materialData->Material->HasTransparency)
					{
						int test = 0;
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
							//If mesh is parented
							if (meshData->parentMatrix != nullptr)
							{
								//If mesh is bound to a specific joint during animations.
								if (meshData->holdJointTransform != nullptr)
								{
									meshData->TransformMap.at(TransformIt.first)->ModelMatrix = (*meshData->parentMatrix * *meshData->holdJointTransform * *meshData->TransformMap.at(TransformIt.first)->GetModelMatrix());
								}
								else
								{
									meshData->TransformMap.at(TransformIt.first)->ModelMatrix = *meshData->parentMatrix * meshData->TransformMap.at(TransformIt.first)->ModelMatrix;
								}
							}

							float distance = 0;														//The scale check is so the plane is not culled
							if (culling  && meshData->TransformMap.at(TransformIt.first)->Scale.x < 100 && s_DistanceCulling &&meshData->TransformMap.at(TransformIt.first)->EnableCulling)
							{
								distance = glm::distance(meshData->TransformMap.at(TransformIt.first)->Position, s_SceneData->GameCamera.CameraPosition);
							}

							if (distance < 110)
							{
								nrOfDrawnedObjs++;

								shaderData->Shader->UploadUniformMat4("u_Transform", meshData->TransformMap.at(TransformIt.first)->ModelMatrix);

								if (shaderData->Shader->GetName() == "Animation")
								{
									if (meshData->AnimMap.find(TransformIt.first) != meshData->AnimMap.end())
									{
										void* skinDataPtr = nullptr;
										int nrOfBones = 0;
										auto& controller = meshData->AnimMap.at(TransformIt.first);
										if (controller->currAnim != nullptr)
										{
											controller->currAnim->CalculateAnimMatrix(&controller->dt);
											controller->currAnim->GetSkinData(skinDataPtr, nrOfBones);
											meshData->VertexArray->GetUniformBuffer()->BindUpdate(skinDataPtr, nrOfBones);

											controller->dt += Frosty::Time::DeltaTime() * controller->animSpeed;
										}
										else
										{
											FY_FATAL("ANIMCONROLLER HAS NO CURRENTANIM");
										}
									}
								}
								if (*meshData->RenderMesh == true)
								{
									RenderCommand::Draw2D(meshData->VertexArray);
								}
							}
							else
							{
								nrOfCulledObjs++;

							}


							nrOfTransforms++;

						}




						glBindVertexArray(0);
					}

					glBindTexture(GL_TEXTURE_2D, 0);
				}

				glUseProgram(0);
				if (shaderData->Shader->GetName() == "ShadowMap")
				{
					glBindFramebuffer(GL_FRAMEBUFFER, 0);

				}
				if (shaderData->Shader->GetName() == "Texture2D" || shaderData->Shader->GetName() == "FlatColor" || shaderData->Shader->GetName() == "Animation")
				{
					glDisable(GL_BLEND);
					RenderCommand::DisableBackfaceCulling();
				}
			}


		}
		s_TotalNrOfFrames++;


		//DrawToFullScreenQuad(s_ShadowMap);

		//DrawToFullScreenQuad(AssetManager::GetTexture2D("green_square")->GetRenderID());

	}

	void Renderer::EndScene()
	{

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
			s_SceneData->PointLights.emplace(light->EntityPtr->Id, PointLight());
			s_SceneData->PointLights[int(light->EntityPtr->Id)].PointLight = light;
			s_SceneData->PointLights[int(light->EntityPtr->Id)].Transform = transform;
		}
		else if (light->Type == Frosty::ECS::CLight::LightType::Directional)
		{
			glm::mat4 mat = glm::mat4(1.0f);
			mat = glm::rotate(mat, glm::radians(light->Direction.x), { 1.0f, 0.0f, 0.0f });
			mat = glm::rotate(mat, glm::radians(light->Direction.x), { 0.0f, 1.0f, 0.0f });
			mat = glm::rotate(mat, glm::radians(light->Direction.x), { 0.0f, 0.0f, 1.0f });

			light->Direction = mat * glm::vec4(light->Direction, 0);


			s_SceneData->DirectionalLights.emplace(int(light->EntityPtr->Id), DirectionalLight());
			s_SceneData->DirectionalLights[int(light->EntityPtr->Id)].DirectionalLight = light;
			s_SceneData->DirectionalLights[int(light->EntityPtr->Id)].Transform = transform;
		}
	}

	void Renderer::UpdateLight(Frosty::ECS::CLight* light, ECS::CTransform* transform)
	{
		if (light->Type == Frosty::ECS::CLight::LightType::Point)
		{
			s_SceneData->PointLights[int(light->EntityPtr->Id)].PointLight = light;
			s_SceneData->PointLights[int(light->EntityPtr->Id)].Transform = transform;
		}
		else if (light->Type == Frosty::ECS::CLight::LightType::Directional)
		{

			s_SceneData->DirectionalLights[int(light->EntityPtr->Id)].DirectionalLight = light;
			s_SceneData->DirectionalLights[int(light->EntityPtr->Id)].Transform = transform;
		}
	}

	void Renderer::RemoveLight(const std::shared_ptr<ECS::Entity>& entity)
	{
		/*auto& world = Application::Get().GetWorld();
		auto& light = world->GetComponent<Frosty::ECS::CLight>(entity);
*/

		if (s_SceneData->PointLights.find(entity->Id) != s_SceneData->PointLights.end())
		{
			s_SceneData->PointLights.erase(entity->Id);
		}
		else if (s_SceneData->DirectionalLights.find(entity->Id) != s_SceneData->DirectionalLights.end())
		{
			s_SceneData->DirectionalLights.erase(entity->Id);
		}
	}

	void Renderer::RemoveAllLights()
	{
		for (auto& PLightIt : s_SceneData->PointLights)
		{
			//delete s_SceneData->PointLights[PLightIt.first];
			s_SceneData->PointLights.erase(PLightIt.first);
		}

		for (auto& DLightIt : s_SceneData->DirectionalLights)
		{
			//delete s_SceneData->DirectionalLights[DLightIt.first];
			s_SceneData->DirectionalLights.erase(DLightIt.first);
		}

	}

	void Renderer::ChangeLight(const std::shared_ptr<ECS::Entity>& entity)
	{
		auto& world = Application::Get().GetWorld();

		RemoveLight(entity);
		AddLight(&world->GetComponent<Frosty::ECS::CLight>(entity), &world->GetComponent<Frosty::ECS::CTransform>(entity));
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
		glDisable(GL_DEPTH_TEST);
		shader->Bind();
		vertexArray->Bind();
		auto& window = Frosty::Application::Get().GetWindow();
		float width = float(window.GetWidth());
		float height = float(window.GetHeight());
		glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height);

		shader->UploadUniformMat4("projection", projection);
		shader->UploadUniformInt("text", 0); //Make sure this number matches the active and sampled texture
		shader->UploadUniformFloat3("textColor", color);

		std::string::const_iterator c;
		float x = pos.x * window.GetWidthMultiplier();
		float y = pos.y * window.GetHeightMultiplier();
		scale = scale * window.GetWidthMultiplier();

		for (c = text.begin(); c != text.end(); c++) {
			Character ch = Frosty::AssetManager::GetTTF("Gabriola")->m_characters[*c];
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
		glEnable(GL_DEPTH_TEST);
	}

	void Renderer::SubmitSprite(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const uint32_t textureID, glm::vec4 color, glm::mat4 transform)
	{
		shader->Bind();
		vertexArray->Bind();
		auto& window = Frosty::Application::Get().GetWindow();
		float width = float(window.GetWidth());
		float height = float(window.GetHeight());
		glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height);
		shader->UploadUniformMat4("projection", projection);

		transform[3][0] *= window.GetWidthMultiplier();
		transform[3][1] *= window.GetHeightMultiplier();
		transform[0][0] *= window.GetWidthMultiplier();
		transform[1][1] *= window.GetHeightMultiplier();

		shader->UploadUniformMat4("transform", transform);
		shader->UploadUniformFloat4("color", color); //Make sure this number matches the active and sampled texture

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		vertexArray->GetVertexBuffer().front()->Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		RenderCommand::EnableBackfaceCulling();
		RenderCommand::Draw2D(vertexArray); //Will probably change later
		RenderCommand::DisableBackfaceCulling();
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		shader->UnBind();
		vertexArray->Unbind();

	}

	void Renderer::SubmitParticles(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, glm::mat4& modelMat, size_t particleCount, float maxLifetime, unsigned int renderMode)
	{
		shader->Bind();
		vertexArray->Bind();

		shader->UploadUniformMat4("viewMat", s_SceneData->GameCamera.ViewMatrix);
		shader->UploadUniformMat4("projectionMat", s_SceneData->GameCamera.ProjectionMatrix);
		shader->UploadUniformMat4("modelMat", modelMat);

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		switch (renderMode)
		{
		case 0:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case 1:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		}

		RenderCommand::DisableBackfaceCulling();
		RenderCommand::DrawParticles(vertexArray, particleCount);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		vertexArray->Unbind();
		shader->UnBind();
	}

	void Renderer::Submit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mat->UseShader->Bind();
		mat->UseShader->UploadUniformMat4("u_ViewProjection", s_SceneData->GameCamera.ViewProjectionMatrix);
		mat->UseShader->UploadUniformMat4("u_Transform", transform);
		mat->UseShader->UploadUniformFloat3("u_CameraPosition", s_SceneData->GameCamera.CameraPosition);
		mat->UseShader->UploadUniformInt("u_Shininess", mat->Shininess);

		SubmitLightUniforms(mat);
		SubmitForwardPlusUniforms(mat);

		if (mat->UseShader->GetName() == "FlatColor" || mat->UseShader->GetName() == "HeatMap")
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

		glDisable(GL_BLEND);
	}

	void Renderer::SubmitLightUniforms(ECS::CMaterial* mat)
	{
		// Point Lights
		mat->UseShader->UploadUniformInt("u_TotalPointLights", (int)s_SceneData->PointLights.size());
		int PointLI = 0;
		for (auto& pointLIt : s_SceneData->PointLights)
		{
			mat->UseShader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(PointLI) + "].Color", s_SceneData->PointLights[pointLIt.first].PointLight->Color);
			mat->UseShader->UploadUniformFloat3Array("u_PointLights[" + std::to_string(PointLI) + "].Position", s_SceneData->PointLights[pointLIt.first].Transform->Position);
			mat->UseShader->UploadUniformFloatArray("u_PointLights[" + std::to_string(PointLI) + "].Radius", s_SceneData->PointLights[pointLIt.first].PointLight->Radius);
			mat->UseShader->UploadUniformFloatArray("u_PointLights[" + std::to_string(PointLI) + "].Strength", s_SceneData->PointLights[pointLIt.first].PointLight->Strength);
			PointLI++;
		}

		// Directional Lights
		mat->UseShader->UploadUniformInt("u_TotalDirectionalLights", (int)s_SceneData->DirectionalLights.size());
		int dirLI = 0;
		for (auto& DirLIt : s_SceneData->DirectionalLights)
		{
			mat->UseShader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(dirLI) + "].Color", s_SceneData->DirectionalLights[DirLIt.first].DirectionalLight->Color);
			mat->UseShader->UploadUniformFloat3Array("u_DirectionalLights[" + std::to_string(dirLI) + "].Direction", s_SceneData->DirectionalLights[DirLIt.first].DirectionalLight->Direction);
			mat->UseShader->UploadUniformFloatArray("u_DirectionalLights[" + std::to_string(dirLI) + "].Strength", s_SceneData->DirectionalLights[DirLIt.first].DirectionalLight->Strength);
			dirLI++;
		}
	}

	void Renderer::SubmitForwardPlusUniforms(ECS::CMaterial* mat)
	{
		// LightIndex
		for (int i = 0; i < int(s_ForwardPlus.GetLightIndexList().size()); i++)
		{
			mat->UseShader->UploadUniformInt(("forwardPlus.LightIndexList[" + std::to_string(i) + "]").c_str(), s_ForwardPlus.GetLightIndexList()[i]);
		}

		// CellLightInfo
		for (int i = 0; i < s_ForwardPlus.GetNrOfGrids(); i++)
		{
			mat->UseShader->UploadUniformFloat2(("forwardPlus.CellLightInfo[" + std::to_string(i) + "]").c_str(), s_ForwardPlus.GetCellLightInfoAt(i));
		}
	}

	//For 2D, might be temp
	void Renderer::Submit2d(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		float width = 1280.0f;
		float height = 720.0f;
		glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height);

		shader->UploadUniformMat4("projection", projection);

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

	//void Renderer::AddToRenderer(ECS::CMaterial* mat, ECS::CMesh* mesh, ECS::CTransform* transform)
	//{
	//	if (mat->UseShader->GetName() != "Animation")
	//	{
	//		std::unordered_map<std::string, std::shared_ptr<ShaderData>>* ShaderMap = &s_ShaderMap;
	//		//std::unordered_map<std::string, std::shared_ptr<ShaderData>>* ShaderMap = nullptr;

	//		//// at(0) will be for dept sampling for shadow map
	//		//if (mat->HasTransparency)
	//		//{
	//		//	ShaderMap = &s_RenderPas[1];
	//		//}
	//		//else
	//		//{
	//		//	ShaderMap = &s_RenderPas[2];
	//		//	
	//		//}

	//		counter++;
	//		//Set up IDs
	//		size_t matID = transform->EntityPtr->Id; //Works but can be improved whith a real material ID
	//		std::string meshID = mesh->Mesh->GetName();
	//		size_t transformID = transform->EntityPtr->Id;

	//		//Check if the shader key is already in the map, if not add it.
	//		std::string ShaderName = mat->UseShader->GetName();
	//		if (ShaderMap->find(mat->UseShader->GetName()) == ShaderMap->end())
	//		{
	//			ShaderMap->emplace(mat->UseShader->GetName(), FY_NEW ShaderData);

	//		}
	//		auto& shaderData = ShaderMap->at(ShaderName);
	//		shaderData->Shader = mat->UseShader;

	//		//Check if the material key is already in the map, if not add it.
	//		if (shaderData->MaterialMap.find(matID) == shaderData->MaterialMap.end())
	//		{
	//			shaderData->MaterialMap.emplace(matID, FY_NEW MaterialData);
	//		}
	//		auto& materialData = shaderData->MaterialMap[matID];
	//		materialData->Material = mat;

	//		//Add the material to the MaterialLookUpMap
	//		s_MaterialLookUpMap.emplace(transformID, &shaderData->MaterialMap);

	//		//Check if the mesh key is already in the map, if not add it.
	//		if (materialData->MeshMap.find(meshID) == materialData->MeshMap.end())
	//		{
	//			materialData->MeshMap.emplace(meshID, FY_NEW  MeshData);
	//		}
	//		auto& meshData = materialData->MeshMap[meshID];
	//		meshData->VertexArray = mesh->Mesh;
	//		meshData->TransformMap.emplace(transformID, transform);
	//		if (mesh->parentMatrix != nullptr)
	//		{
	//			meshData->parentMatrix = mesh->parentMatrix;
	//			meshData->holdJointTransform = mesh->animOffset;
	//		}

	//		//Add the mesh to the MeshLookUpMap
	//		s_MeshLookUpMap.emplace(transformID, &materialData->MeshMap);

	//		//Add the tranform to the TransformLookUpMap
	//		auto& transformMap = meshData->TransformMap;
	//		s_TransformLookUpMap.emplace(transformID, &transformMap);

	//		ShaderMap;
	//	}
	//}	

	void Renderer::AddToRenderer(ECS::CMaterial* mat, ECS::CMesh* mesh, ECS::CTransform* transform, ECS::CAnimController* controller)
	{

		// at(0) will be for dept sampling for shadow map
		int RenderPassID = 0;


		if (mat->HasTransparency)
		{
			RenderPassID = 2;
		}
		else
		{
			RenderPassID = 1;
		}

		// Forward Plus Temp
		if (mat->UseShader->GetName() == "HeatMap")
		{
			RenderPassID = 3;
		}




		counter++;
		//Set up IDs
		size_t matID = transform->EntityPtr->Id; //Works but can be improved whith a real material ID
		std::string meshID = mesh->Mesh->GetName();
		size_t transformID = transform->EntityPtr->Id;


		//Check if the shader key is already in the map, if not add it.
		std::string ShaderName = mat->UseShader->GetName();
		if (s_RenderPas[RenderPassID].ShaderMap.find(mat->UseShader->GetName()) == s_RenderPas[RenderPassID].ShaderMap.end())
		{
			s_RenderPas[RenderPassID].ShaderMap.emplace(mat->UseShader->GetName(), FY_NEW ShaderData);

		}
		auto& shaderData = s_RenderPas[RenderPassID].ShaderMap.at(ShaderName);
		shaderData->Shader = mat->UseShader;



		//Check if the material key is already in the map, if not add it.
		if (shaderData->MaterialMap.find(matID) == shaderData->MaterialMap.end())
		{
			shaderData->MaterialMap.emplace(matID, FY_NEW MaterialData);
		}
		auto& materialData = shaderData->MaterialMap.at(matID);
		materialData->Material = mat;

		//Add the material to the MaterialLookUpMap
		s_MaterialLookUpMap.emplace(transformID, &shaderData->MaterialMap);

		//Check if the mesh key is already in the map, if not add it.
		if (materialData->MeshMap.find(meshID) == materialData->MeshMap.end())
		{
			materialData->MeshMap.emplace(meshID, FY_NEW  MeshData);
		}
		auto& meshData = materialData->MeshMap.at(meshID);
		meshData->VertexArray = mesh->Mesh;
		meshData->TransformMap.emplace(transformID, transform);

		meshData->RenderMesh = &mesh->RenderMesh;

		if (mesh->parentMatrix != nullptr)
		{
			meshData->parentMatrix = mesh->parentMatrix;
			meshData->holdJointTransform = mesh->animOffset;
		}

		if (controller != nullptr)
		{
			meshData->AnimMap.emplace(transformID, controller);
		}
		//Add the mesh to the MeshLookUpMap
		s_MeshLookUpMap.emplace(transformID, &materialData->MeshMap);

		//Add the tranform to the TransformLookUpMap
		auto& transformMap = meshData->TransformMap;
		s_TransformLookUpMap.emplace(transformID, &transformMap);

		//For Shadows
		if (mat->CastsShadows)
		{
			//Check if the shader key is already in the map, if not add it.
			std::string ShaderName = "ShadowMap";
			if (s_RenderPas[0].ShaderMap.find(ShaderName) == s_RenderPas[0].ShaderMap.end())
			{
				s_RenderPas[0].ShaderMap.emplace(ShaderName, FY_NEW ShaderData);

			}
			auto& shaderData2 = s_RenderPas[0].ShaderMap.at(ShaderName);
			shaderData2->Shader = AssetManager::GetShader(ShaderName);



			//Check if the material key is already in the map, if not add it.
			if (shaderData2->MaterialMap.find(matID) == shaderData2->MaterialMap.end())
			{
				shaderData2->MaterialMap.emplace(matID, materialData);
			}
		}

		s_RenderPas[RenderPassID].ShaderMap; //For debugging
	}

	void Renderer::RemoveFromRenderer(const size_t& matID, const std::string& meshName, const size_t& transformID)
	{
		if (s_TransformLookUpMap.find(transformID) != s_TransformLookUpMap.end())
		{
			s_ShaderMap;

			size_t nrOfTransforms = 0;
			size_t nrOfMeshes = 0;

			s_TransformLookUpMap.at(transformID)->at(transformID) = nullptr;
			s_TransformLookUpMap.at(transformID)->erase(transformID);

			if (s_TransformLookUpMap.at(transformID)->size() == 0)
			{
				nrOfTransforms = s_TransformLookUpMap.at(transformID)->size();
				s_MeshLookUpMap.at(transformID)->erase(meshName);
			}

			if (s_MeshLookUpMap.at(transformID)->size() == 0)
			{
				nrOfMeshes = s_MeshLookUpMap.at(transformID)->size();
				s_MaterialLookUpMap.at(transformID)->erase(matID);

				//m_MaterialLookUpMap.erase(transformID);

			}

			if (nrOfTransforms == 0)
			{
				s_MeshLookUpMap.erase(transformID);
			}

			if (nrOfMeshes == 0)
			{
				s_MaterialLookUpMap.erase(transformID);
			}


			s_TransformLookUpMap.erase(transformID);

		}

	}

	void Renderer::UpdateEntity(const size_t& matID, ECS::CMaterial* mat, const std::string& meshName, std::shared_ptr<VertexArray> vertexArray, const size_t& transformID, ECS::CTransform* transform)
	{
		if (s_TransformLookUpMap.find(transformID) != s_TransformLookUpMap.end())
		{
			s_ShaderMap;

			s_TransformLookUpMap.at(transformID)->at(transformID) = transform;
			s_MeshLookUpMap.at(transformID)->at(meshName)->VertexArray = vertexArray;
			//^^ Crashes up there on row 678 occasion after weapon pickup ^^
			s_MaterialLookUpMap.at(transformID)->at(matID)->Material = mat;
		}
	}

	void Renderer::SwapEntity(const std::shared_ptr<ECS::Entity>& EntityA, const std::shared_ptr<ECS::Entity>& EntityB)
	{
		auto& world = Application::Get().GetWorld();

		ECS::CMesh* meshA = &world->GetComponent<ECS::CMesh>(EntityA);
		ECS::CMesh* meshB = &world->GetComponent<ECS::CMesh>(EntityB);

		ECS::CMaterial* matA = &world->GetComponent<ECS::CMaterial>(EntityA);
		ECS::CMaterial* matB = &world->GetComponent<ECS::CMaterial>(EntityB);

		ECS::CAnimController* controllerA = nullptr;
		ECS::CAnimController* controllerB = nullptr;

		if (world->HasComponent<ECS::CAnimController>(EntityA))
		{
			ECS::CAnimController* controllerA = &world->GetComponent<ECS::CAnimController>(EntityA);
		}

		if (world->HasComponent<ECS::CAnimController>(EntityB))
		{
			ECS::CAnimController* controllerB = &world->GetComponent<ECS::CAnimController>(EntityB);
		}

		ChangeEntity(EntityA->Id, matA, meshB->Mesh->GetName(), meshA, EntityA->Id, &world->GetComponent<ECS::CTransform>(EntityA), controllerA);
		ChangeEntity(EntityB->Id, matB, meshA->Mesh->GetName(), meshB, EntityB->Id, &world->GetComponent<ECS::CTransform>(EntityB), controllerB);
	}

	void Renderer::ChangeEntity(const size_t& OldMatID, ECS::CMaterial* mat, const std::string& OldMeshName, ECS::CMesh* mesh, const size_t& transformID, ECS::CTransform* transform, ECS::CAnimController* newController)
	{
		//Not the best but it works

		if (s_TransformLookUpMap.find(transformID) != s_TransformLookUpMap.end())
		{
			//Remove old
			RemoveFromRenderer(OldMatID, OldMeshName, transformID);

			//Add new
			AddToRenderer(mat, mesh, transform, newController);
		}
	}

	void Renderer::UpdateCMesh(const size_t& entityID, ECS::CMesh* mesh)
	{
		if (s_MeshLookUpMap.find(entityID) != s_MeshLookUpMap.end())
		{
			auto& meshData = s_MeshLookUpMap.at(entityID)->at(mesh->Mesh->GetName());
			meshData->RenderMesh = &mesh->RenderMesh;
			meshData->parentMatrix = mesh->parentMatrix;
			meshData->holdJointTransform = mesh->animOffset;
		}
	}

	void Renderer::UpdateCMesh(const size_t& entityID, ECS::CMesh* mesh, ECS::CAnimController* ctrlPtr)
	{
		if (s_MeshLookUpMap.find(entityID) != s_MeshLookUpMap.end())
		{
			auto& meshData = s_MeshLookUpMap.at(entityID)->at(mesh->Mesh->GetName());
			meshData->RenderMesh = &mesh->RenderMesh;
			meshData->parentMatrix = mesh->parentMatrix;
			//If it already exists replace. If not emplace.
			if (meshData->AnimMap.find(entityID) != meshData->AnimMap.end())
			{
				meshData->AnimMap.at(entityID) = ctrlPtr;
			}
			else
			{
				meshData->AnimMap.emplace(entityID, ctrlPtr);
			}
		}
	}

	void Renderer::CreateDepthMap()
	{
		glGenFramebuffers(1, &s_ShadowMapFBO);

		glGenTextures(1, &s_ShadowMap);
		glBindTexture(GL_TEXTURE_2D, s_ShadowMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		int err = 1;

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			err = 0;
		}
		else
		{
			err = -1;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, s_ShadowMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, s_ShadowMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Renderer::CreateFullScreenQuad()
	{
		struct Pos2UV
		{
			float x, y;
			float u, v;
		};
		Pos2UV myQuad[6] =
		{
			-1,-1, 0, 0,
			-1,+1, 0, 1,
			+1,+1, 1, 1,
			-1,-1, 0, 0,
			+1,+1, 1, 1,
			+1,-1, 1, 0,
		};

		glGenVertexArrays(1, &s_FullScreenQuad);
		glBindVertexArray(s_FullScreenQuad);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &s_FullScreenQuad);
		glBindBuffer(GL_ARRAY_BUFFER, s_FullScreenQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(myQuad), myQuad, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Pos2UV), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Pos2UV), BUFFER_OFFSET(sizeof(float) * 2));
	}

	void Renderer::DrawToFullScreenQuad(unsigned int TextureID)
	{
		glDisable(GL_DEPTH_TEST);
		AssetManager::GetShader("DebugQuad")->Bind();
		glBindVertexArray(s_FullScreenQuad);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);
	}
}