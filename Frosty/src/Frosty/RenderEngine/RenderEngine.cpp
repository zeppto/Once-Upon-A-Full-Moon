#include "fypch.hpp"
#include "Glad/glad.h"
#include "RenderEngine.hpp"
#include "Frosty/Core/Application.hpp"
#include "Frosty/DEFINITIONS.hpp"
#include "Frosty/API/PrefabManager/PrefabManager.h"

namespace Frosty
{	
	/*void RenderEngine::UpdateInfoFromWindow()
	{
		auto app = &Application::Get().GetWindow();
		m_WindowHeight = app->GetHeight();
		m_WindowWidth = app->GetWidth();
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

		//m_Camera = new Camera;
	}
	RenderEngine::~RenderEngine()
	{
		//delete m_Camera;
	}
	void RenderEngine::Render()
	{
		UpdateInfoFromWindow();
		


		m_TestMode = true;
		if (m_TestMode)
		{
			RenderTestTriangle();



		}
	}
	void RenderEngine::UpdateCamera()
	{
		//auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		//m_Camera->CameraPositionUpdate(window);
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices) * 3, triangleVertices, GL_STATIC_DRAW);

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

		//glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
		//glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);

		glUseProgram(m_ShaderProgramVector.at(TEST_SHAPE));

		glBindVertexArray(this->m_testTriangleVBO);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}*/
	
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

	void RenderEngine::RenderPassGUI()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, m_GUIPassFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(m_ShaderProgramVector.at(SECONDPASS));

		//glEnable(GL_DEPTH_TEST);
		//glDepthMask(GL_TRUE);
		//glDisable(GL_BLEND);

		// Render Data

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderEngine::RenderPassQuad()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		//glUseProgram(m_ShaderProgramVector.at(THIRDPASS));
		glBindVertexArray(m_QuadVbo);

		glDepthMask(GL_TRUE);
		glDisable(GL_DEPTH_TEST);

		//Textures		

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void RenderEngine::InitBuffers()
	{
		CreateFirstPassFrameBuffer();
		CreateGUIPassFramebuffer();
	}

	void RenderEngine::UpdateInfoFromWindow()
	{
		auto app = &Application::Get().GetWindow();
		m_WindowHeight = app->GetHeight();
		m_WindowWidth = app->GetWidth();
	}

	void RenderEngine::CreateAllShaderPrograms()
	{
		CreateShaderProgram(FY_SHADER_TESTSHAPE_VERTEX_SHADER, FY_SHADER_TESTSHAPE_FRAGMENT_SHADER, TEST_SHAPE);
		CreateShaderProgram(FY_SHADER_TESTLIGHT_VERTEX_SHADER, FY_SHADER_TESTLIGHT_FRAGMENT_SHADER, TEST_LIGHT);
		CreateShaderProgram(FY_SHADER_TESTTEXTURE_VERTEX_SHADER, FY_SHADER_TESTTEXTURE_FRAGMENT_SHADER, TEST_TEXTURE);
	}

	void RenderEngine::CreateFirstPassFrameBuffer()
	{
		unsigned int gBufferAttachments[2];

		glGenFramebuffers(1, &m_FirstPassFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FirstPassFramebuffer);

		// Colour 
		glGenTextures(1, &m_ColourRenderTexture);
		glBindTexture(GL_TEXTURE_2D, m_ColourRenderTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_WindowWidth, m_WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColourRenderTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			FY_CORE_INFO("Success Generation of Framebuffer for Colour Texture");
			gBufferAttachments[0] = GL_COLOR_ATTACHMENT0;
		}
		else
		{
			FY_CORE_ERROR("Fault Genereting Colour Texture to Framebuffer");
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
			gBufferAttachments[1] = GL_DEPTH_ATTACHMENT;
		}
		else
		{
			FY_CORE_ERROR("Fault Genereting Position Texture to Framebuffer");
		}

		glDrawBuffers(2, gBufferAttachments);
		glDeleteBuffers(1, &m_FirstPassFramebuffer);
	}

	void RenderEngine::CreateGUIPassFramebuffer()
	{
		unsigned int gBufferAttachments[1];

		glGenFramebuffers(1, &m_GUIPassFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_GUIPassFramebuffer);

		// Colour
		glGenTextures(1, &m_GUIRenderTexture);
		glBindTexture(GL_TEXTURE_2D, m_GUIRenderTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_WindowWidth, m_WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GUIRenderTexture, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			FY_CORE_INFO("Success Generation of Texture for GUI-Framebuffer");
			gBufferAttachments[0] = GL_COLOR_ATTACHMENT0;
		}
		else
		{
			FY_CORE_ERROR("Fault Genereting Texture to GUI-Framebuffer");
		}

		glDrawBuffers(1, gBufferAttachments);
		glDeleteBuffers(1, &m_GUIRenderTexture);
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

	RenderEngine::RenderEngine()
	{
		CreateAllShaderPrograms();
		UpdateInfoFromWindow();
		InitBuffers();
		CreateTriangle();

		m_Camera = new Camera;

		////TEMP//LOAD//FILES////
		auto tempAssetsManager = Assetmanager::GetAssetmanager();
		MotherLoader::GetMotherLoader()->LoadFiles();
		MotherLoader::GetMotherLoader()->PrintLoadingAttemptInformation();

		AssetMetaData<ModelTemplate>* metaModel = tempAssetsManager->GetModeltemplateMetaData("clock");


		AssetMetaData<TextureFile>* metaTexture = tempAssetsManager->GetTextureMetaData("pCube10_diffuse");
		
		auto tempPrefabManager = PrefabManager::GetPrefabManager();
		
		tempPrefabManager->setPrefab("TestPrefab1", "clock", "Mat_0:table");
		tempPrefabManager->setPrefab("TestPrefab2", "table", "Mat_0:clock");

		glBindTexture(GL_TEXTURE_2D, 0);
		////////
	}

	RenderEngine::~RenderEngine()
	{
		delete m_Camera;
	}

	void RenderEngine::ClearColor()
	{
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	}

	void RenderEngine::Render()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


		UpdateInfoFromWindow();

		m_Transform.SetRotate(glm::vec3(0.0f, m_Rotation += 100 * Time::DeltaTime(), 0.0f));//Temp
		//RenderTriangle();
		RenderTempModels();

		
		/*RenderPrefab("TestPrefab1");
		RenderPrefab("TestPrefab2");*/

	}

	void RenderEngine::UpdateCamera()
	{
		m_Camera->CameraPositionUpdate();
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
			{ 0.5f, 0.5f, 0.0f,		 1.0f, 0.0f,	1.0f, 1.0f, 0.0f, },
			{ 0.5f, -0.5f, 0.0f,	 1.0f, 1.0f,	1.0f, 0.0f, 1.0f},
			{-0.5f, -0.5f, 0.0f,	 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,},
		};

		glGenVertexArrays(1, &this->m_testTriangleVBO);
		glBindVertexArray(this->m_testTriangleVBO);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &this->m_testTriangleVBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->m_testTriangleVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices) * 3, triangleVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 5));
		glBindVertexArray(0);
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

	void RenderEngine::RenderTriangle()
	{
		

		glUniformMatrix4fv(0, 1, GL_FALSE, &m_Transform.GetModel()[0][0]); //Temp

		glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
		glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);
		glUniform1i(3, false); //Render without texture

		//glUseProgram(m_ShaderProgramVector.at(TEST_SHAPE));
		glBindVertexArray(this->m_testTriangleVBO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}

	void RenderEngine::RenderTestTriangle()
	{

		m_Transform.SetRotate(glm::vec3(0.0f, m_Rotation += 0.1, 0.0f));//Temp

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glUniformMatrix4fv(0, 1, GL_FALSE, &m_Transform.GetModel()[0][0]); //Temp

		glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
		glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);

		// <<<<<< TESTING LIGHTS IN SHADER	>>>>>>		~ W-_-W ~
		struct Light
		{
			Light(glm::vec3 newPosition, glm::vec4 newColor, float newStrength, glm::vec2 lin_Quad)
			{
				position = newPosition; color = newColor;  strength = newStrength; linear_Quadratic = lin_Quad;
			}
			Light(glm::vec4 newColor, float newStrength, glm::vec3 newDirection)
			{
				color = newColor;  strength = newStrength; direction = newDirection;
			}

			glm::vec3 position;
			glm::vec4 color;
			float strength;
			glm::vec2 linear_Quadratic;
			glm::vec3 direction;
		};
		// Point Lights
		std::vector<Light> lights;
		glm::vec3 pos = { 0.f, 0.f, 1.f };
		//Light l(pos, { 1.f, 1.f, 1.f, 1.f }, 1.f, { 0.07f, 0.017f });
		Light l(m_Transform.GetTranslate(), { 1.f, 1.f, 1.f, 1.f }, 1.f, { 0.07f, 0.017f });//temp
		Light l2(pos, { 1.f, 1.f, 1.f, 1.f }, 1.f, { 0.7f, 1.8f });
		lights.emplace_back(l);
		lights.emplace_back(l2);

		glUniform1ui(glGetUniformLocation(m_ShaderProgramVector[TEST_LIGHT], "nrOfPointLights"), lights.size());
		for (int i = 0; i < lights.size(); i++)
		{

			glUniform3fv(glGetUniformLocation(m_ShaderProgramVector[TEST_LIGHT], ("pointLights[" + std::to_string(i) + "].position").c_str()), 1, &lights[i].position[0]);
			glUniform4fv(glGetUniformLocation(m_ShaderProgramVector[TEST_LIGHT], ("pointLights[" + std::to_string(i) + "].color").c_str()), 1, &lights[i].color[0]);
			glUniform1fv(glGetUniformLocation(m_ShaderProgramVector[TEST_LIGHT], ("pointLights[" + std::to_string(i) + "].strength").c_str()), 1, &lights[i].strength);
			glUniform2fv(glGetUniformLocation(m_ShaderProgramVector[TEST_LIGHT], ("pointLights[" + std::to_string(i) + "].linear_Quadratic").c_str()), 1, &lights[i].linear_Quadratic[0]);
		}

		// Directional Light
		lights.clear();
		Light l3({ 0.f, 0.3f, 0.7f, 1.f }, 0.3f, { 0.f, 0.f, -1.f });
		Light l4({ 0.2f, 0.1f, 0.1f, 1.f }, 0.2f, { 0.f, -5.f, -1.f });
		lights.emplace_back(l3);
		lights.emplace_back(l4);

		glUniform1ui(glGetUniformLocation(m_ShaderProgramVector[TEST_LIGHT], "nrOfDirLights"), lights.size());
		for (int i = 0; i < lights.size(); i++)
		{
			glUniform4fv(glGetUniformLocation(m_ShaderProgramVector[TEST_LIGHT], ("dirLights[" + std::to_string(i) + "].color").c_str()), 1, &lights[i].color[0]);
			glUniform1fv(glGetUniformLocation(m_ShaderProgramVector[TEST_LIGHT], ("dirLights[" + std::to_string(i) + "].strength").c_str()), 1, &lights[i].strength);
			glUniform3fv(glGetUniformLocation(m_ShaderProgramVector[TEST_LIGHT], ("dirLights[" + std::to_string(i) + "].direction").c_str()), 1, &lights[i].direction[0]);
		}

		glUseProgram(m_ShaderProgramVector.at(TEST_LIGHT));
		// <<<<<< END OF LIGHTS TESTING	>>>>>>

		//glUseProgram(m_ShaderProgramVector.at(TEST_SHAPE));

		glBindVertexArray(this->m_testTriangleVBO);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void RenderEngine::RenderModel(const unsigned int& VBO, const unsigned int& nrOfVertices, const glm::mat4& modelMatrix, const unsigned int& textureID)
	{
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
	
		glUniformMatrix4fv(0, 1, GL_FALSE, &modelMatrix[0][0]); //Temp
		glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
		glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);
		glUniform1i(3, true); //Render with texture

		glUseProgram(m_ShaderProgramVector.at(TEST_SHAPE));
		glBindVertexArray(VBO);
		glDrawArrays(GL_TRIANGLES, 0, nrOfVertices);
		glBindVertexArray(0);
	}

	void RenderEngine::RenderPrefab(std::string prefabName)
	{
		auto tempPrefabManager = PrefabManager::GetPrefabManager();
		Prefab* tempPrefab = tempPrefabManager->GetPrefab(prefabName);

		RenderModel
		(
			tempPrefab->GetModelKey().GetKeyData().GetVBO(0),
			tempPrefab->GetModelKey().GetKeyData().GetMeshConst(0).vertexCount,
			m_Transform.GetModel(), //temp
			tempPrefab->GetMaterialKey().GetKeyData().Diffuse_Texture_MetaData_Ptr->GetData()->GetBufferID()

		);
	}

	void RenderEngine::AddToRenderList(TempRender* obj)
	{

		m_Temp_RenderList.emplace_back(obj);

	}

	void RenderEngine::RenderTempModels()
	{
		glUseProgram(m_ShaderProgramVector.at(TEST_SHAPE));
		for (int i = 0; i < m_Temp_RenderList.size(); i++)
		{

			if (m_Temp_RenderList.at(i)->m_Render)
			{

				glBindVertexArray(m_Temp_RenderList.at(i)->model_ptr->GetVBO(0));

				glUniformMatrix4fv(0, 1, GL_FALSE, &m_Temp_RenderList.at(i)->worldPosition[0][0]);
				glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
				glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);
				glUniform3fv(4, 1, &m_Temp_RenderList.at(i)->m_Render_Colour[0]);
				glUniform1i(3, true); //Render with texture

				glActiveTexture(GL_TEXTURE0);
				
				glBindTexture(GL_TEXTURE_2D, m_Temp_RenderList.at(i)->material_Ptr->Diffuse_Texture_MetaData_Ptr->GetData()->GetBufferID());
				glDrawArrays(GL_TRIANGLES, 0, m_Temp_RenderList.at(i)->model_ptr->GetMeshConst(0).vertexCount);

				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	}

}