#include "fypch.hpp"
#include "Glad/glad.h"
#include "RenderEngine.hpp"

#include "Frosty/Core/Application.hpp"
#include "Frosty/DEFINITIONS.hpp"



//#include <stb_image.hpp>

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


		glUseProgram(m_ShaderProgramVector.at(THIRDPASS));
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

	void RenderEngine::CreateAllShaderPrograms()
	{
		CreateShaderProgram(FY_SHADER_TESTSHAPE_VERTEX_SHADER, FY_SHADER_TESTSHAPE_FRAGMENT_SHADER, TEST_SHAPE);
		CreateShaderProgram(FY_SHADER_TESTLIGHT_VERTEX_SHADER, FY_SHADER_TESTLIGHT_FRAGMENT_SHADER, TEST_LIGHT);
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
		auto tempManager = Assetmanager::GetAssetmanager();
		
		// Temp load
		//tempManager->LoadFile(FY_TEST_LOAD_MODEL,"RavJavel");
		//tempManager->LoadFile(FY_TEST1_LOAD_MODEL,"clock");
		//tempManager->LoadFile(FY_TEST2_LOAD_MODEL,"bord");
		MotherLoader::GetMotherLoader()->LoadFiles();
		MotherLoader::GetMotherLoader()->PrintLoadingAttemptInformation();
		//= *tempManager->GetModelTemplate();

		AssetMetaData<ModelTemplate>* metaModel = tempManager->GetModeltemplateMetaData("clock");
		std::shared_ptr<ModelTemplate> model = metaModel->GetData();


		AssetMetaData<TextureFile>* metaTexture = tempManager->GetTextureMetaData("pCube10_diffuse");

		//std::shared_ptr<TextureFile> texture = metaTexture->GetData();

		//CreateTestTextureData(texture);

		m_Transform.setTranslate(glm::vec3(1.0f, 0.0f, -2.0f));
		//m_Transform.setRotate(glm::vec3(30.0f, 30.0f, 30.0f));

		m_Camera = new Camera;
	}
	/*RenderEngine::RenderEngine()
	{
		CreateAllShaderPrograms();		
		UpdateInfoFromWindow();		
		m_Camera = new Camera;
		CreateTestModel();
		m_TestBox.SetUp();
		m_TestBox2.SetUp();
		m_TestBox3.SetUp();
		m_TestPlayer.SetColor(glm::vec3(0.0f, 0.0f, 1.0f));
		m_TestPlayer.SetPosition(glm::vec3(0.0f, 2.0f, -1.0f));
		m_TestPlayer.SetUp();
		m_TestPlayer.SetIsPlayer(true);
	}*/

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
	/*void RenderEngine::Render()
	{
		UpdateInfoFromWindow();		
*/
		//m_TestMode = true;
		//if (m_TestMode)
		//{
		//	m_TestBox.SetShaderProgram(m_ShaderProgramVector.at(TEST_SHAPE));
		//	m_TestBox2.SetShaderProgram(m_ShaderProgramVector.at(TEST_SHAPE));
		//	m_TestBox3.SetShaderProgram(m_ShaderProgramVector.at(TEST_SHAPE));
		//	m_TestPlayer.SetShaderProgram(m_ShaderProgramVector.at(TEST_SHAPE));
		//	RenderAllTestModels();
		//	m_TestBox.Update(glm::vec3(3, 0, -1), glm::vec3(0.0f), glm::vec3(1.0f));
		//	m_TestBox2.Update(glm::vec3(-2, -1, -1), glm::vec3(0.0f), glm::vec3(1.0f));
		//	m_TestBox3.Update(glm::vec3(-1, -2, -1), glm::vec3(0.0f), glm::vec3(1.0f));
		//	//m_TestPlayer.Update(glm::vec3(0.0f, 3.0f, -3.0f) + m_Camera->GetPos(), glm::vec3(0.0f), glm::vec3(1.0f));
		//	m_TestPlayer.Update();
		//	if (m_CollisionDetection.AABBIntersect(m_TestPlayer.GetHitBoxLength(),
		//		m_TestPlayer.GetHitBoxCenter(), m_TestBox.GetHitBoxLength(), m_TestBox.GetHitBoxCenter()))
		//		m_TestBox.SetShouldRender(false);
		//	else
		//		m_TestBox.SetShouldRender(true);

		//	//glm::vec3 testNewPos = m_TestPlayer.GetPos();
		//	//glm::vec3 temp = m_CollisionDetection.AABBIntersecPushback(m_TestPlayer.GetHitBoxLength(),
		//	//	m_TestPlayer.GetHitBoxCenter(), m_TestBox2.GetHitBoxLength(), m_TestBox2.GetHitBoxCenter());
		//	//testNewPos -= temp;
		//	//m_TestPlayer.SetPosition(testNewPos);

		//	m_TestPlayer.SetPosition(m_TestPlayer.GetPos() - m_CollisionDetection.AABBIntersecPushback(m_TestPlayer.GetHitBoxLength(),
		//		m_TestPlayer.GetHitBoxCenter(), m_TestBox2.GetHitBoxLength(), m_TestBox2.GetHitBoxCenter()));
		//	m_TestPlayer.SetPosition(m_TestPlayer.GetPos() - m_CollisionDetection.AABBIntersecPushback(m_TestPlayer.GetHitBoxLength(),
		//		m_TestPlayer.GetHitBoxCenter(), m_TestBox3.GetHitBoxLength(), m_TestBox3.GetHitBoxCenter()));
		//	//if (m_CollisionDetection.AABBIntersect(m_TestPlayer.GetHitBoxLength(),
		//	//	m_TestPlayer.GetHitBoxCenter(), m_TestBox2.GetHitBoxLength(), m_TestBox2.GetHitBoxCenter()))
		//	//	m_TestBox2.SetShouldRender(false);
		//	//else
		//	//	m_TestBox2.SetShouldRender(true);
		//	m_TestBox2.IsAttacked(m_TestPlayer.Attacked());

		//	m_TestBox.Render(m_Camera->GetView(), m_Camera->GetProjection());
		//	m_TestBox2.Render(m_Camera->GetView(), m_Camera->GetProjection());
		//	m_TestBox3.Render(m_Camera->GetView(), m_Camera->GetProjection());
		//	m_TestPlayer.Render(m_Camera->GetView(), m_Camera->GetProjection());


	//	}
	//}

	void RenderEngine::UpdateCamera()
	{
		m_Camera->CameraPositionUpdate();
	}

	void RenderEngine::CreateTriangle()
	{
		struct TriangleVertex
		{
			float x, y, z;
			float u, v;
			float r, g, b;
		};

		TriangleVertex triangleVertices[6] =
		{
			{ 2.f, 2.f, 0.0f,	1.0f, 0.0f,		1.0f, 1.0f, 0.0f},
			{ 2.f, -2.f, 0.0f,	1.0f, 1.0f,		1.0f, 0.0f, 1.0f},
			{-2.f, -2.f, 0.0f,	0.0f, 1.0f,		1.0f, 0.0f, 0.0f},

			{ 2.f, 2.f, 0.0f, 	1.0f, 0.0f,		1.0f, 1.0f, 0.0f},
			{-2.f, -2.f, 0.0f,  0.0f, 1.0f,		1.0f, 0.0f, 0.0f},
			{-2.f, 2.f, 0.0f,	1.0f, 1.0f,		1.0f, 0.0f, 1.0f},
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

	void RenderEngine::RenderTestTriangle()
	{

		m_Transform.setRotate(glm::vec3(0.0f, m_Rotation += 0.1, m_Rotation));//Temp

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glUniformMatrix4fv(0, 1, GL_FALSE, &m_Transform.getModel()[0][0]); //Temp

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
		Light l(m_Transform.getTranslate(), { 1.f, 1.f, 1.f, 1.f }, 1.f, { 0.07f, 0.017f });//temp
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

		//glDrawArrays(GL_TRIANGLES, 0, 6);


		if (m_RenderTestModel)
		{

			for (int i = 0; i < m_Temp_RenderList.size(); i++)
			{

				glBindVertexArray(m_Temp_RenderList.at(i)->model_ptr->GetVBO(0));
				glUniformMatrix4fv(0, 1, GL_FALSE, &m_Temp_RenderList.at(i)->worldPosition[0][0]); 
				glUniform3fv(13, 1,&m_Temp_RenderList.at(i)->m_Colour[0] );
				glActiveTexture(GL_TEXTURE0);
				if (m_Temp_RenderList.at(i)->material_Ptr->Diffuse_Texture_MetaData_Ptr != nullptr)
				{
					glBindTexture(GL_TEXTURE_2D, m_Temp_RenderList.at(i)->material_Ptr->Diffuse_Texture_MetaData_Ptr->GetData()->GetBufferID());
				}
				glDrawArrays(GL_TRIANGLES, 0, m_Temp_RenderList.at(i)->model_ptr->GetMeshConst(0).vertexCount);

				glBindTexture(GL_TEXTURE_2D,0);
				//glBindVertexArray(Assetmanager::GetAssetmanager()->GetModeltemplateMetaData("clock")->GetData()->GetVBO(0));
			//	glBindTexture(GL_TEXTURE_2D, Assetmanager::GetAssetmanager()->GetMaterialMetaData("Mat_0:clock")->GetData()->Diffuse_Texture_MetaData_Ptr->GetData()->GetBufferID());
				//glDrawArrays(GL_TRIANGLES, 0, Assetmanager::GetAssetmanager()->GetModeltemplateMetaData("clock")->GetData()->GetMeshConst(0).vertexCount);

			}


		}

	}

	//for test sean
		void RenderEngine::CreateTestModel(glm::vec3 color)
	{
		struct TriangleVertex
		{
			float x, y, z;
			float r, g, b;
			float u, v;
		};
		TriangleVertex triangleVertices[] =
		{
			//{ 0.5f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	 1.0f, 0.0f },
			//{ 0.5f, -0.5f, 0.0f,	 1.0f, 0.0f, 1.0f,	 1.0f, 1.0f},
			//{-0.5f, -0.5f, 0.0f,	 1.0f, 0.0f, 0.0f,	0.0f, 1.0f},

			-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f

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
	void RenderEngine::RenderAllTestModels()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);		
		
		glUseProgram(m_ShaderProgramVector.at(TEST_SHAPE));
		
		for (int i = 0; i < 1; i++)
		{
			glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
			glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);
			glm::mat4 world = glm::mat4(1.0f);
			world = glm::translate(world, glm::vec3(0, 0, -2));
			world = glm::scale(world, glm::vec3(10.0f, 10.0f, 0.0f));
			glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(world));
			glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
			glUniform3fv(4, 1, glm::value_ptr(color));


			glBindVertexArray(this->m_testTriangleVBO);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
		//glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);

		//glUseProgram(m_ShaderProgramVector.at(TEST_SHAPE));

		//glBindVertexArray(this->m_testTriangleVBO);

		//glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void RenderEngine::AddToRenderList(TempRender* obj)
	{

		m_Temp_RenderList.emplace_back(obj);

	}




}
