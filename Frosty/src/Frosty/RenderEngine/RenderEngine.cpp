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
	RenderEngine::RenderEngine()
	{
		CreateAllShaderPrograms();		
		UpdateInfoFromWindow();		
		m_Camera = new Camera;
		CreateTestModel();
		m_TestBox.SetUp();
		m_TestBox2.SetUp();
		m_TestPlayer.SetColor(glm::vec3(0.0f, 0.0f, 1.0f));
		m_TestPlayer.SetPosition(glm::vec3(0.0f, 2.0f, -1.0f));
		m_TestPlayer.SetUp();
		m_TestPlayer.SetIsPlayer(true);
	}
	RenderEngine::~RenderEngine()
	{		
		delete m_Camera;
	}
	void RenderEngine::Render()
	{
		UpdateInfoFromWindow();		

		m_TestMode = true;
		if (m_TestMode)
		{
			m_TestBox.SetShaderProgram(m_ShaderProgramVector.at(TEST_SHAPE));
			m_TestBox2.SetShaderProgram(m_ShaderProgramVector.at(TEST_SHAPE));
			m_TestPlayer.SetShaderProgram(m_ShaderProgramVector.at(TEST_SHAPE));
			RenderAllTestModels();
			m_TestBox.Update(glm::vec3(3, 0, -1), glm::vec3(0.0f), glm::vec3(1.0f));
			m_TestBox2.Update(glm::vec3(-2, -1, -1), glm::vec3(0.0f), glm::vec3(1.0f));
			//m_TestPlayer.Update(glm::vec3(0.0f, 3.0f, -3.0f) + m_Camera->GetPos(), glm::vec3(0.0f), glm::vec3(1.0f));
			m_TestPlayer.Update();
			if (m_CollisionDetection.AABBIntersect(m_TestPlayer.GetHitBoxLength(),
				m_TestPlayer.GetHitBoxCenter(), m_TestBox.GetHitBoxLength(), m_TestBox.GetHitBoxCenter()))
				m_TestBox.SetShouldRender(false);
			else
				m_TestBox.SetShouldRender(true);

			//glm::vec3 testNewPos = m_TestPlayer.GetPos();
			//glm::vec3 temp = m_CollisionDetection.AABBIntersecPushback(m_TestPlayer.GetHitBoxLength(),
			//	m_TestPlayer.GetHitBoxCenter(), m_TestBox2.GetHitBoxLength(), m_TestBox2.GetHitBoxCenter());
			//testNewPos -= temp;
			//m_TestPlayer.SetPosition(testNewPos);

			m_TestPlayer.SetPosition(m_TestPlayer.GetPos() - m_CollisionDetection.AABBIntersecPushback(m_TestPlayer.GetHitBoxLength(),
				m_TestPlayer.GetHitBoxCenter(), m_TestBox2.GetHitBoxLength(), m_TestBox2.GetHitBoxCenter()));
			//if (m_CollisionDetection.AABBIntersect(m_TestPlayer.GetHitBoxLength(),
			//	m_TestPlayer.GetHitBoxCenter(), m_TestBox2.GetHitBoxLength(), m_TestBox2.GetHitBoxCenter()))
			//	m_TestBox2.SetShouldRender(false);
			//else
			//	m_TestBox2.SetShouldRender(true);
			m_TestBox.Render(m_Camera->GetView(), m_Camera->GetProjection());
			m_TestBox2.Render(m_Camera->GetView(), m_Camera->GetProjection());
			m_TestPlayer.Render(m_Camera->GetView(), m_Camera->GetProjection());


		}
	}
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
		
		for (int i = 0; i < 4; i++)
		{
			glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
			glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);
			glm::mat4 world = glm::mat4(1.0f);
			world = glm::translate(world, glm::vec3(0, 0, -2));
			world = glm::scale(world, glm::vec3(10.0f, 10.0f, 0.0f));


			glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(world));


			glBindVertexArray(this->m_testTriangleVBO);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
		//glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);

		//glUseProgram(m_ShaderProgramVector.at(TEST_SHAPE));

		//glBindVertexArray(this->m_testTriangleVBO);

		//glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	void RenderEngine::RenderTestModel()
	{

	}

	void RenderEngine::UpdateCamera()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		m_Camera->CameraPositionUpdate(window);
	}
}
