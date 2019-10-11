#include "fypch.hpp"
#include "Glad/glad.h"
#include "Renderer.hpp"
#include "Frosty/API/AssetManager/Assetmanager.hpp"

namespace Frosty
{	
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;	
	
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

	void Renderer::Submit2D(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::string& tex, glm::mat4& modelMatrix)
	{
		shader->Bind();
		shader->UploadUniforMat4("model", modelMatrix);
		//shader->UploadUniformInt(tex, 0);

		vertexArray->Bind();
		RenderCommand::Draw2D(vertexArray);
		shader->UnBind();
		vertexArray->Unbind();
	}

	void Renderer::SubmitText(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, std::shared_ptr<VertexBuffer>& vertexBuffer, std::string& text)
	{
		shader->Bind();
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

			vertexBuffer->Bind();
			vertexBuffer->SetData(*verts, sizeof(verts), GL_DYNAMIC_DRAW);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, ch.textureID);

			RenderCommand::Draw2D(vertexArray);

			x += (ch.advance >> 6) * scale;
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		vertexArray->Unbind();
		vertexBuffer->Unbind();
	}
	
	void Renderer::DeleteSceneData()
	{
		if (m_SceneData != nullptr)
		{
			delete m_SceneData;
		}
	}
}
