#include "fypch.hpp"
#include "Glad/glad.h"
#include "RenderEngine.hpp"
#include "Frosty/Core/Application.hpp"
#include "Frosty/DEFINITIONS.hpp"

namespace Frosty
{
	void RenderEngine::UpdateInfoFromWindow()
	{
		auto app = &Application::Get().GetWindow();
		m_WindowHeight = app->GetHeight();
		m_WindowWidth = app->GetWidth();
	}
	void RenderEngine::RenderPassOne()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

		glBindFramebuffer(GL_FRAMEBUFFER, m_FirstPassFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(m_ShaderProgramVector.at(FIRSTPASS));

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		auto app = &Application::Get();

		// Render Data

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void RenderEngine::RenderPassTwo()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		glUseProgram(m_ShaderProgramVector.at(SECONDPASS));
		glBindVertexArray(m_QuadVbo);

		glDepthMask(GL_TRUE);
		glDisable(GL_DEPTH_TEST);

		//Textures		

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	void RenderEngine::InitBuffers()
	{
		CreateFirstPassFrameBuffer();
	}
	void RenderEngine::CreateFirstPassFrameBuffer()
	{
		unsigned int gBufferAttachments[6];

		glGenFramebuffers(1, &m_FirstPassFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FirstPassFramebuffer);

		// Position 
		glGenTextures(1, &m_PositionRenderTexture);
		glBindTexture(GL_TEXTURE_2D, m_PositionRenderTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_WindowWidth, m_WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PositionRenderTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{			
			FY_CORE_INFO("Success Generation of Framebuffer for Position Texture");
			gBufferAttachments[0] = GL_COLOR_ATTACHMENT0;
		}
		else
		{			
			FY_CORE_ERROR("Fault Genereting Position Texture to Framebuffer");
		}

		// Normal 
		glGenTextures(1, &m_NormalRenderTexture);
		glBindTexture(GL_TEXTURE_2D, m_NormalRenderTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_WindowWidth, m_WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_NormalRenderTexture, 0);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			gBufferAttachments[1] = GL_COLOR_ATTACHMENT1;
			FY_CORE_INFO("Success Generation of Framebuffer for Normal Texture");
		}
		else
		{
			FY_CORE_ERROR("Fault Genereting Normal Texture to Framebuffer");
		}

		//	Albedo 
		glGenTextures(1, &m_AlbedoRenderTexture);
		glBindTexture(GL_TEXTURE_2D, m_AlbedoRenderTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_WindowWidth, m_WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_AlbedoRenderTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			FY_CORE_INFO("Success Generation of Framebuffer for Albedo Texture");
			gBufferAttachments[2] = GL_COLOR_ATTACHMENT2;
		}
		else
		{
			FY_CORE_ERROR("Fault Genereting Position Texture to Framebuffer");
		}
		
		//	Albedo Original
		glGenTextures(1, &m_AlbedoOrginalTexture);
		glBindTexture(GL_TEXTURE_2D, m_AlbedoOrginalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_WindowWidth, m_WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_AlbedoOrginalTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			FY_CORE_INFO("Success Generation of Framebuffer for Albedo Texture");
			gBufferAttachments[3] = GL_COLOR_ATTACHMENT3;
		}
		else
		{
			FY_CORE_ERROR("Fault Genereting Position Texture to Framebuffer");
		}
		
		//	Albedo Final
		glGenTextures(1, &m_FinalTexture);
		glBindTexture(GL_TEXTURE_2D, m_FinalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_WindowWidth, m_WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_FinalTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			FY_CORE_INFO("Success Generation of Framebuffer for Final Texture");
			gBufferAttachments[4] = GL_COLOR_ATTACHMENT4;
		}
		else
		{
			FY_CORE_ERROR("Fault Genereting Final Texture to Framebuffer");
		}
		
		glGenTextures(1, &m_DepthRenderTexture);
		glBindTexture(GL_TEXTURE_2D, m_DepthRenderTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_WindowWidth, m_WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthRenderTexture, 0);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			FY_CORE_INFO("Success Generation of Framebuffer for Depth Texture");
			gBufferAttachments[5] = GL_DEPTH_ATTACHMENT;
		}
		else
		{
			FY_CORE_ERROR("Fault Genereting Position Texture to Framebuffer");
		}

		glDrawBuffers(5, gBufferAttachments);
		glDeleteBuffers(1, &m_FirstPassFramebuffer);
	}
	void RenderEngine::CreateAllShaderPrograms()
	{
		CreateShaderProgram(FY_SHADER_TESTSHAPE_VERTEX_SHADER, FY_SHADER_TESTSHAPE_FRAGMENT_SHADER, TEST_SHAPE);

	}
	void RenderEngine::CreateShaderProgram(std::string VertexShaderPath, std::string FragmentShaderPath, ShaderProgramsEnum Program)
	{
		GLuint tempProgram;

		char buff[1024];
		memset(buff, 0, 1024);
		GLint compileResult = 0;

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		std::ifstream shaderFile(VertexShaderPath);
		std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
		shaderFile.close();

		const char* shaderTextPtr = shaderText.c_str();

		glShaderSource(vs, 1, &shaderTextPtr, nullptr);
		glCompileShader(vs);
		glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			glGetShaderInfoLog(vs, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			//FY_CORE_ERROR("GLFW Error ({0}) \n Shader: ({1}) \n Program: ({2})", buff, VertexShaderPath, GetShaderProgramEnumString(Program));
		}

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		shaderFile.open(FragmentShaderPath);
		shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
		shaderFile.close();
		shaderTextPtr = shaderText.c_str();
		glShaderSource(fs, 1, &shaderTextPtr, nullptr);
		glCompileShader(fs);
		compileResult = GL_FALSE;
		glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			memset(buff, 0, 1024);
			glGetShaderInfoLog(fs, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			//FY_CORE_ERROR("GLFW Error ({0}) \n Shader: ({1}) \n Program: ({2})", buff, FragmentShaderPath, GetShaderProgramEnumString(Program));
		}

		tempProgram = glCreateProgram();
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
			//FY_CORE_ERROR("GLFW Error \n CompileProgram Fail Msg: ({0}) \n Program: ({1})", buff, GetShaderProgramEnumString(Program));
		}
		else
		{
			//FY_CORE_INFO("Success Generation of ({0}) Program)", GetShaderProgramEnumString(Program));
		}

		glDetachShader(tempProgram, vs);
		glDetachShader(tempProgram, fs);
		glDeleteShader(vs);
		glDeleteShader(fs);

		m_ShaderProgramVector.insert(m_ShaderProgramVector.begin() + Program, tempProgram);
	}
	void RenderEngine::CreateQuad()
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

		glGenVertexArrays(1, &m_QuadVbo);
		glBindVertexArray(m_QuadVbo);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &m_QuadVbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_QuadVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(myQuad), myQuad, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Pos2UV), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Pos2UV), BUFFER_OFFSET(sizeof(float) * 2));
	}
	RenderEngine::RenderEngine()
	{
		CreateAllShaderPrograms();		
		UpdateInfoFromWindow();
		InitBuffers();
		CreateTriangle();	
		CreateQuad();

		m_Camera = new Camera;
	}
	RenderEngine::~RenderEngine()
	{		
		delete m_Camera;
	}
	void RenderEngine::Render()
	{
		UpdateInfoFromWindow();		
		//RenderPassOne();
		//RenderPassTwo();


		m_TestMode = true;
		if (m_TestMode)
		{
			RenderTestTriangle();



		}
	}
	void RenderEngine::UpdateCamera()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		m_Camera->CameraPositionUpdate(window);
	}
	void RenderEngine::CreateTriangle()
	{
		struct TriangleVertex
		{
			float x, y, z;
			float r, g, b;
			float u, v;
		};

		TriangleVertex triangleVertices[3] =
		{
			{ 0.5f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	 1.0f, 0.0f },
			{ 0.5f, -0.5f, 0.0f,	 1.0f, 0.0f, 1.0f,	 1.0f, 1.0f},
			{-0.5f, -0.5f, 0.0f,	 1.0f, 0.0f, 0.0f,	0.0f, 1.0f},
		};

		glGenVertexArrays(1, &this->m_testTriangleVBO);
		glBindVertexArray(this->m_testTriangleVBO);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &this->m_testTriangleVBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->m_testTriangleVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices)*3, triangleVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 6));
		glBindVertexArray(0);
	}	
	void RenderEngine::RenderTestTriangle()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);		
		
		glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
		glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);

		glUseProgram(m_ShaderProgramVector.at(TEST_SHAPE));

		glBindVertexArray(this->m_testTriangleVBO);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}	
}
