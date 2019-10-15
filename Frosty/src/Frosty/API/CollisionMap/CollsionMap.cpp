#include<fypch.hpp>
#include"CollisionMap.hpp"
#include "Glad/glad.h"


namespace Frosty
{
	
	
	CollisionMap* CollisionMap::s_Instance = nullptr;


	CollisionMap* CollisionMap::Get() 
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new CollisionMap;
		}
		return s_Instance;

	}

	void CollisionMap::Initiate()
	{
		m_Camera = std::make_shared<Camera>(Camera());
		//m_CameraData.AspRatio = Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight();
		//m_CameraData.Projection = glm::perspective(m_CameraData.FoV, m_CameraData.AspRatio, m_CameraData.NearPlane, m_CameraData.FarPlane);

		m_Camera->GetCameraTranslationData().Pos = glm::vec3(((float)MAP_WITDH_PIXELS)/(2*m_Pix_Cord_Ratio),100.0f, ((float)MAP_HEIGHT_PIXELS) / (2* m_Pix_Cord_Ratio));
		m_Camera->GetCameraTranslationData().Pos = glm::vec3(96.0f,100.0f,54.0);
		m_Camera->GetCameraTranslationData().LookAtVec = glm::vec3(0.0f, -1.0f, 0.0f);
		m_Camera->GetCameraTranslationData().UpVec = glm::vec3(0.0f, 0.0f, -1.0f);
		m_Camera->GetCameraTranslationData().CamSpeed = 10.0f;

		m_Pos = glm::vec3(((float)MAP_WITDH_PIXELS) / (2 * m_Pix_Cord_Ratio), 100.0f, ((float)MAP_HEIGHT_PIXELS) / (2 * m_Pix_Cord_Ratio));
		m_View = glm::lookAt(m_Camera->GetCameraTranslationData().Pos, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		m_OrthoGraphic  = glm::ortho((float)MAP_WITDH_PIXELS / (-2 * m_Pix_Cord_Ratio), (float)MAP_WITDH_PIXELS / (2.0f * m_Pix_Cord_Ratio), (float)MAP_HEIGHT_PIXELS / (-2.0f * m_Pix_Cord_Ratio), (float)MAP_HEIGHT_PIXELS / (2.0f * m_Pix_Cord_Ratio), 1.0f, 200.0f);

		m_Camera->GetCameraData().FarPlane = 200.0f;
		m_Camera->GetCameraData().NearPlane = 0.1f;
		m_Camera->GetCameraData().FoV = glm::radians(70.0f);
				
		m_Camera->GetCameraData().AspRatio = MAP_WITDH_PIXELS / MAP_HEIGHT_PIXELS;
		
				
		//m_Camera->GetCameraData().Projection = glm::perspective(m_Camera->GetCameraData().FoV, m_Camera->GetCameraData().AspRatio, m_Camera->GetCameraData().NearPlane, m_Camera->GetCameraData().FarPlane);
		//m_Camera->GetCameraData().OrthoGraphic = glm::ortho((float)MAP_WITDH_PIXELS/-20, (float)MAP_WITDH_PIXELS/20.0f, (float)MAP_HEIGHT_PIXELS / -20.0f,(float)MAP_HEIGHT_PIXELS/20.0f,1.0f,200.0f);
		m_Camera->GetCameraData().OrthoGraphic = glm::ortho((float)MAP_WITDH_PIXELS/-20, (float)MAP_WITDH_PIXELS/20.0f, (float)MAP_HEIGHT_PIXELS / -20.0f,(float)MAP_HEIGHT_PIXELS/20.0f,1.0f,200.0f);
		m_Camera->GetCameraData().View = glm::lookAt(m_Camera->GetCameraTranslationData().Pos,glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		m_Camera->GetCameraData().Projection = m_OrthoGraphic;


	}

	std::shared_ptr<Camera>& CollisionMap::GetCamera()
	{
		//return std::make_shared<Camera>(m_Camera);
		return m_Camera;
	}

	void CollisionMap::InitiateGBuffer()
	{
		glGenFramebuffers(1, &m_GBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer);

		// - bright color buffer
		glGenTextures(1, &m_Texture);
		glBindTexture(GL_TEXTURE_2D, m_Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, MAP_WITDH_PIXELS, MAP_HEIGHT_PIXELS, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);

		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, attachments);

		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			FY_CORE_ERROR("Framebuffer not complete!");
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void CollisionMap::InitiateShaders()
	{
		m_VertexSrc = R"(
			#version 440 core
			
			layout(location = 0) in vec3 vsInPos;
			
			uniform mat4 u_ViewOrtho;
			
			out vec3 vsOutPos;
			
			void main()
			{
				vsOutPos = vsInPos;
				gl_Position = u_ViewOrtho * vec4(vsInPos, 1.0f);
			}
		)";
		m_FragmentSrc = R"(
			#version 440 core

			in vec3 vsOutPos;

			layout(location = 0) out vec4 fsOutCol;
			
			void main()
			{
				fsOutCol = vec4{ 1.f, 0.f, 0.f, 1.f };
			}
		)";
	}

	void CollisionMap::InitiateProgram()
	{
		char buff[1024];
		memset(buff, 0, 1024);
		GLint compileResult = 0;

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);

		const char* shaderTextPtr = m_VertexSrc.c_str();

		glShaderSource(vs, 1, &shaderTextPtr, nullptr);
		glCompileShader(vs);
		glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			glGetShaderInfoLog(vs, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ERROR("Collision Map Generator Failed to compile Vertex Shader, GLFW Error ({0}) \n)", buff);
		}

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

		shaderTextPtr = m_FragmentSrc.c_str();

		glShaderSource(fs, 1, &shaderTextPtr, nullptr);
		glCompileShader(fs);
		compileResult = GL_FALSE;
		glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			memset(buff, 0, 1024);
			glGetShaderInfoLog(fs, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ERROR("Collision Map Generator Failed to compile Fragment Shader, GLFW Error ({0}) \n", buff);
		}

		m_RendererID = glCreateProgram();
		GLuint tempProgram = m_RendererID;
		glAttachShader(tempProgram, vs);

		glAttachShader(tempProgram, fs);
		glLinkProgram(tempProgram);

		compileResult = GL_FALSE;
		glGetProgramiv(tempProgram, GL_LINK_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			memset(buff, 0, 1024);
			glGetProgramInfoLog(tempProgram, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ERROR("Collision Map Generator Failed to Link program, GLFW Error ({0}) \n)", buff);
		}
		else
		{
			FY_CORE_INFO("Success Generation of Program)");
		}

		glDetachShader(tempProgram, vs);
		glDetachShader(tempProgram, fs);
		glDeleteShader(vs);
		glDeleteShader(fs);
	}

	void CollisionMap::RenderQuad()
	{

		if (QuadVAO == 0) {
			float quadVertices[] = {
				// positions       
				MAP_WITDH_PIXELS/(2*m_Pix_Cord_Ratio),  1.0f, MAP_HEIGHT_PIXELS/(2* m_Pix_Cord_Ratio),
				0.0f,  1.0f, 0.0f,
				0.0f,  1.0f, MAP_HEIGHT_PIXELS / m_Pix_Cord_Ratio
			};
			unsigned int VBO;
			glGenVertexArrays(1, &QuadVAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(QuadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		}

		glUseProgram(m_RendererID);

		// Render the Quad
		glBindVertexArray(QuadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}













}