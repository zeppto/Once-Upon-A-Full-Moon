#include<fypch.hpp>
#include"BoolMapGenerator.hpp"
#include "Glad/glad.h"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{


	//temp
	std::shared_ptr <Camera> BoolMapGenerator::m_Camera = nullptr;
	unsigned int BoolMapGenerator::m_VertexArray = -1;
	//


	BoolMapGenerator* BoolMapGenerator::s_Instance = nullptr;
	std::list<ModelBatch> BoolMapGenerator::s_ModelList = std::list<ModelBatch>();
	BoolMapGenerator::GeneratorSettings BoolMapGenerator::s_Settings = BoolMapGenerator::GeneratorSettings();


	//RenderData
	glm::mat4 BoolMapGenerator::s_ViewOrtho(1.0f);
	unsigned int BoolMapGenerator::s_GBuffer = -1;
	unsigned int BoolMapGenerator::s_Texture = -1;
	unsigned int BoolMapGenerator::s_RenderProgramID = -1;

	BoolMapGenerator* BoolMapGenerator::Get()
	{
		if (s_Instance == nullptr)
		{
			Init();
		}
		return s_Instance;

	}

	void BoolMapGenerator::InitiateRenderData()
	{

		s_Settings.Pos = glm::vec3((s_Settings.Width) / 2.0f, 100.0f, ((float)s_Settings.Height) / 2.0f);

		glm::vec3 tempDir = s_Settings.Pos;
		tempDir[1] = 0.0f;
		glm::mat4 tempView = glm::lookAt(s_Settings.Pos, tempDir, s_Settings.UpVec);


		glm::vec4 OrthoVec((float)s_Settings.Width / -2.0f, (float)s_Settings.Width / 2.0f, (float)s_Settings.Height / -2.0f, (float)s_Settings.Height / 2.0f);
		glm::mat4 tempOrtho = glm::ortho(OrthoVec[0], OrthoVec[1], OrthoVec[2], OrthoVec[3], 1.0f, 200.0f);

		s_ViewOrtho = tempOrtho * tempView;


		m_Camera = std::make_shared<Camera>(Camera());
		m_Camera->GetCameraTranslationData().Pos = s_Settings.Pos;
		m_Camera->GetCameraTranslationData().UpVec = glm::vec3(0.0f, 0.0f, -1.0f);
		m_Camera->GetCameraTranslationData().CamSpeed = 10.0f;
		m_Camera->GetCameraTranslationData().LookAtVec = tempDir;
		m_Camera->GetCameraData().FarPlane = 200.0f;
		m_Camera->GetCameraData().NearPlane = 0.1f;
		m_Camera->GetCameraData().FoV = glm::radians(70.0f);
		m_Camera->GetCameraData().OrthoGraphic = tempOrtho;
		m_Camera->GetCameraData().View = tempView;
		m_Camera->GetCameraData().Projection = tempOrtho;

		m_Camera->GetCameraData().AspRatio = (float)s_Settings.Width / (float)s_Settings.Height;

	}

	std::shared_ptr<Camera>& BoolMapGenerator::GetCamera()
	{
		return m_Camera;
	}

	void BoolMapGenerator::Init()
	{
		if (s_Instance != nullptr)
		{
			delete s_Instance;
		}
		s_Instance = new BoolMapGenerator;
		InitiateProgram();
	}

	void BoolMapGenerator::InitCheck()
	{
		if (s_Instance == nullptr) {
			Init();
		}
	}

	void BoolMapGenerator::InitiateGBuffer()
	{
		glGenFramebuffers(1, &s_GBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, s_GBuffer);

		// - bright color buffer
		glGenTextures(1, &s_Texture);
		glBindTexture(GL_TEXTURE_2D, s_Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (s_Settings.Width * s_Settings.Pix_Cord_Ratio), (s_Settings.Height * s_Settings.Pix_Cord_Ratio), 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_Texture, 0);

		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);

		char buff[1024];
		memset(buff, 0, 1024);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			memset(buff, 0, 1024);
			glGetProgramInfoLog(s_RenderProgramID, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ASSERT("Collision Map Generator: Failed to Link program, GLFW Error ({0}) \n)", buff);

		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	std::shared_ptr<BoolMap>& BoolMapGenerator::RenderMap()
	{
		InitiateRenderData();

		std::list<ModelBatch>::iterator modelIt = s_ModelList.begin();
		unsigned int s_RenderModelID;

		//temp
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(s_RenderProgramID);

		GLint locationVO = glGetUniformLocation(s_RenderProgramID, "u_ViewOrtho");
		GLint locationMM = glGetUniformLocation(s_RenderProgramID, "u_ModelMat");

		//glm::mat4 temp = m_OrthoGraphic * m_View;

		glUniformMatrix4fv(locationVO, 1, GL_FALSE, &s_ViewOrtho[0][0]);

		float quadVertices[] = {
			// positions       
			128.0f,  0.0f, 72.0f,
			2.0f,  0.0f, 0.0f,
			0.0f,  0.0f,  70.0f
		};

		glGenBuffers(1, &s_RenderModelID);
		glBindBuffer(GL_ARRAY_BUFFER, s_RenderModelID);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), BUFFER_OFFSET(0));
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		glBindVertexArray(s_RenderModelID);
		glUniformMatrix4fv(locationMM, 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);



		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		s_ModelList.erase(s_ModelList.begin(), s_ModelList.end());

		return std::shared_ptr<BoolMap>(nullptr);
	}


	void BoolMapGenerator::InitiateProgram()
	{
		char buff[1024];
		memset(buff, 0, 1024);
		GLint compileResult = 0;

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);

		const char* shaderTextPtr = VERTEXSRC.c_str();

		glShaderSource(vs, 1, &shaderTextPtr, nullptr);
		glCompileShader(vs);
		glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			glGetShaderInfoLog(vs, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ERROR("Collision Map Generator: Failed to compile Vertex Shader, GLFW Error ({0}) \n)", buff);
		}

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

		shaderTextPtr = FRAGMENTSRC.c_str();

		glShaderSource(fs, 1, &shaderTextPtr, nullptr);
		glCompileShader(fs);
		compileResult = GL_FALSE;
		glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			memset(buff, 0, 1024);
			glGetShaderInfoLog(fs, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ERROR("Collision Map Generator: Failed to compile Fragment Shader, GLFW Error ({0}) \n", buff);
		}

		s_RenderProgramID = glCreateProgram();
		//	GLuint tempProgram = RenderProgramID;
		glAttachShader(s_RenderProgramID, vs);

		glAttachShader(s_RenderProgramID, fs);
		glLinkProgram(s_RenderProgramID);

		compileResult = GL_FALSE;
		glGetProgramiv(s_RenderProgramID, GL_LINK_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			memset(buff, 0, 1024);
			glGetProgramInfoLog(s_RenderProgramID, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ASSERT("Collision Map Generator: Failed to Link program, GLFW Error ({0}) \n)", buff);
		}
		else
		{
			FY_CORE_INFO("Success Generation of BoolMap Program)");
		}


		glDetachShader(s_RenderProgramID, vs);
		glDetachShader(s_RenderProgramID, fs);
		glDeleteShader(vs);
		glDeleteShader(fs);
	}


}