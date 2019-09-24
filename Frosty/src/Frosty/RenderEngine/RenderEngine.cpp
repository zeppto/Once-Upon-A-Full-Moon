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
		
	RenderEngine::RenderEngine()
	{
		CreateAllShaderPrograms();		
		UpdateInfoFromWindow();		
		CreateTriangle();

		m_Camera = new Camera;
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
			RenderTestTriangle();
		}
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

		TriangleVertex triangleVertices[6] =
		{
			{ 2.f, 2.f, 0.0f,	1.0f, 1.0f, 0.0f,	 1.0f, 0.0f },
			{ 2.f, -2.f, 0.0f,	1.0f, 0.0f, 1.0f,	 1.0f, 1.0f},
			{-2.f, -2.f, 0.0f,	1.0f, 0.0f, 0.0f,	 0.0f, 1.0f},

			{ 2.f, 2.f, 0.0f,	1.0f, 1.0f, 0.0f,	 1.0f, 0.0f },
			{-2.f, -2.f, 0.0f,	1.0f, 0.0f, 0.0f,	 0.0f, 1.0f },
			{- 2.f, 2.f, 0.0f,	1.0f, 0.0f, 1.0f,	 1.0f, 1.0f },
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
		
		// <<<<<< TESTING LIGHTS IN SHADER	>>>>>>		~ W-_-W ~
		struct Light
		{
			Light(glm::vec3 newPosition, glm::vec4 newColor, float newStrength, glm::vec2 lin_Quad) 
			{ position = newPosition; color = newColor;  strength = newStrength; linear_Quadratic = lin_Quad; }
			Light(glm::vec4 newColor, float newStrength, glm::vec3 newDirection)
			{ color = newColor;  strength = newStrength; direction = newDirection; }		

			glm::vec3 position;
			glm::vec4 color;
			float strength;
			glm::vec2 linear_Quadratic;
			glm::vec3 direction;
		};
		// Point Lights
		std::vector<Light> lights;
		glm::vec3 pos = { 0.f, 0.f, 1.f };
		Light l(pos, { 1.f, 1.f, 1.f, 1.f }, 1.f, {0.07f, 0.017f});
		Light l2(pos, { 1.f, 1.f, 1.f, 1.f }, 1.f, { 0.7f, 1.8f });
		lights.emplace_back(l);
		lights.emplace_back(l2);

		glUniform1ui(glGetUniformLocation(m_ShaderProgramVector[TEST_LIGHT], "nrOfPointLights"), (GLuint)lights.size());
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

		glUniform1ui(glGetUniformLocation(m_ShaderProgramVector[TEST_LIGHT], "nrOfDirLights"), (GLuint)lights.size());
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
}
