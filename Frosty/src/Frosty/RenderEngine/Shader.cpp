#include "fypch.hpp"
#include "Shader.hpp"

#include <glad/glad.h>

namespace Frosty
{
	Shader::Shader(const std::string& computeSrc)
	{
		// Create an empty vertex shader handle
		GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = computeSrc.c_str();
		glShaderSource(computeShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(computeShader);

		GLint isCompiled = 0;
		glGetShaderiv(computeShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(computeShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(computeShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(computeShader);

			FY_CORE_ERROR("{0}", infoLog.data());
			FY_CORE_ASSERT(false, "Compute shader compilation failure!");

			return;
		}

		

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_RendererID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_RendererID, computeShader);

		// Link our program
		glLinkProgram(m_RendererID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)& isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_RendererID);
			// Don't leak shaders either.
			glDeleteShader(computeShader);

			FY_CORE_ERROR("{0}", infoLog.data());
			FY_CORE_ASSERT(false, "Compute shader link failure!");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_RendererID, computeShader);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}
}