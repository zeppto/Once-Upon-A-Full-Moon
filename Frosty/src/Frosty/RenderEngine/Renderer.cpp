#include "fypch.hpp"
#include "Renderer.hpp"
#include "Frosty/Core/ECS.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"

#include <glad/glad.h>

namespace Frosty
{
	Renderer::SceneData* Renderer::s_SceneData = FY_NEW Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
		s_SceneData->PointLights.clear();
		s_SceneData->DirectionalLights.clear();
	}

	void Renderer::SetCamera(const glm::vec3& pos, const glm::mat4& view, const glm::mat4& projection)
	{
		s_SceneData->GameCamera.CameraPosition = pos;
		s_SceneData->GameCamera.ViewMatrix = view;
		s_SceneData->GameCamera.ProjectionMatrix = projection;
		s_SceneData->GameCamera.ViewProjectionMatrix = projection * view;
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
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
		else if (mat->UseShader->GetName() == "Texture2D"|| mat->UseShader->GetName() == "BlendShader")
		{
			mat->UseShader->UploadUniformFloat2("u_TextureCoordScale", mat->TextureScale);
		}
		vertexArray->Bind();
		RenderCommand::EnableBackfaceCulling();
		RenderCommand::Draw2D(vertexArray);

		glDisable(GL_BLEND);
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