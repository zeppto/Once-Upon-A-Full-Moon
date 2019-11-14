#include<fypch.hpp>
#include"TestMap.hpp"
#include<Glad/glad.h>


namespace Frosty
{
	void NodeMap::RenderMap()
	{

		unsigned int gBuffer, texture, renderProgram;

		//Program

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
			FY_CORE_ERROR("Node Map Generator: Failed to compile Vertex Shader, GLFW Error ({0}) \n)", buff);
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
			FY_CORE_ERROR("Node Map Generator: Failed to compile Fragment Shader, GLFW Error ({0}) \n", buff);
		}

		renderProgram = glCreateProgram();
		glAttachShader(renderProgram, vs);

		glAttachShader(renderProgram, fs);
		glLinkProgram(renderProgram);

		compileResult = GL_FALSE;
		glGetProgramiv(renderProgram, GL_LINK_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			memset(buff, 0, 1024);
			glGetProgramInfoLog(renderProgram, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ASSERT("Node Map Generator: Failed to Link program, GLFW Error ({0}) \n)", buff);
		}



		glDetachShader(renderProgram, vs);
		glDetachShader(renderProgram, fs);
		glDeleteShader(vs);
		glDeleteShader(fs);




		//Buffer
		uint32_t tempTexWitdh = (m_Width * PixUnit * 2) + PixUnit * 2;
		uint32_t tempTexHeight = (m_Width * PixUnit * 2) + PixUnit * 2;

		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glViewport(0, 0, tempTexWitdh, tempTexHeight);
		// - bright color buffer
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		//glGenerateMipmap(GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, tempTexWitdh, tempTexHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		unsigned int attachments = (unsigned int)GL_COLOR_ATTACHMENT0;
		glDrawBuffers(1, &attachments);

		char buff[1024];
		memset(buff, 0, 1024);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			memset(buff, 0, 1024);
			glGetProgramInfoLog(renderProgram, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ASSERT("Collision Map Generator: Failed to Link program, GLFW Error ({0}) \n)", buff);
		}


		//RenderData
		glm::mat4 tempView = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0), glm::vec3(0.0f, 0.0f, -1.0), glm::vec3(0.0f, 1.0f, 0.0));
		glm::vec4 OrthoVec((m_Width + 1) / -2.0f, (m_Width + 1) / 2.0f, (m_Height + 1) / -2.0f, (m_Height + 1) / 2.0f);
		glm::mat4 tempOrtho = glm::ortho(OrthoVec[0], OrthoVec[1], OrthoVec[2], OrthoVec[3], 0.01f, 10.0f);
		glm::mat4 ViewOrtho = tempOrtho * tempView;



		std::vector<float> tempVert = {
			-0.5f,  0.0f, -0.5f,
			-0.5f,  0.0f,  0.5f,
			0.5f,  0.0f,  0.5f,
			0.5f,  0.0f,  -0.5f,
			0.5f,  0.0f,  0.5f,
			0.5f,  0.0f,  -0.5f
				};

		unsigned int VertID;

		glUseProgram(renderProgram);
		GLint locationVO = glGetUniformLocation(renderProgram, "u_ViewOrtho");
		GLint locationMM = glGetUniformLocation(renderProgram, "u_ModelMat");
		glUniformMatrix4fv(locationVO, 1, GL_FALSE, &ViewOrtho[0][0]);


	}
}