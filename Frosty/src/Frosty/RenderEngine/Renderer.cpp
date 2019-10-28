#include "fypch.hpp"
#include "Renderer.hpp"
#include "Frosty/Core/ECS.hpp"

#include <glad/glad.h>

namespace Frosty
{
	Renderer::SceneData* Renderer::s_SceneData = FY_NEW Renderer::SceneData;

	void Renderer::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
		s_SceneData->PointLights.clear();
		s_SceneData->DirectionalLights.clear();
	}

	void Renderer::SetCamera(const glm::vec3& pos, const glm::mat4& viewProjection)
	{
		s_SceneData->GameCamera.CameraPosition = pos;
		s_SceneData->GameCamera.ViewProjectionMatrix = viewProjection;
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

	void Renderer::Submit2D(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& tex, glm::mat4& modelMatrix)
	{
		//shader->Bind();
		//shader->UploadUniforMat4("model", modelMatrix);
		////shader->UploadUniformInt(tex, 0);

		//vertexArray->Bind();
		//RenderCommand::Draw2D(vertexArray);
		//shader->UnBind();
		//vertexArray->Unbind();
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

	void Renderer::SubmitParticles(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, glm::mat4& modelMat, size_t particleCount)
	{
		/*shader->Bind();
		vertexArray->Bind();

		shader->UploadUniforMat4("viewMat", m_SceneData->ViewMatrix);
		shader->UploadUniforMat4("projectionMat", m_SceneData->ProjectionMatrix);
		shader->UploadUniforMat4("modelMat", modelMat);

		RenderCommand::DrawParticles(vertexArray, particleCount);

		vertexArray->Unbind();
		shader->UnBind();*/
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
		vertexArray->Bind();
		RenderCommand::Draw2D(vertexArray);
	}

	float dt = 0;

	void Renderer::animSubmit(ECS::CMaterial* mat, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		mat->UseShader->Bind();
		mat->UseShader->UploadUniformMat4("u_ViewProjection", s_SceneData->GameCamera.ViewProjectionMatrix);
		mat->UseShader->UploadUniformMat4("u_Transform", transform);
		mat->UseShader->AssignUniformBlock("a_jointDataBlock");

		vertexArray->getAnimationHandlerPtr()->CalculateAnimMatrix(&dt);
		int MaxBones = 64;
		vertexArray->getUniformBuffer()->BindUpdate(vertexArray->getAnimationHandlerPtr()->getSkinData(), MaxBones);

		vertexArray->Bind();
		RenderCommand::Draw2D(vertexArray);
		dt += 0.003;
	}

}