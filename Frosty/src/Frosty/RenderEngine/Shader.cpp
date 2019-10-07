#include "fypch.hpp"
#include "Shader.hpp"
#include "Glad/glad.h"

namespace Frosty
{
	Shader::Shader(const std::string vertexSrc, const std::string fragmentSrc)
	{
		char buff[1024];
		memset(buff, 0, 1024);
		GLint compileResult = 0;

		GLuint vertexID;
		GLuint fragmentID;

		//Read vertex shader
		vertexID = glCreateShader(GL_VERTEX_SHADER);

		std::ifstream shaderFile(vertexSrc);
		std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
		shaderFile.close();

		const char* shaderTextPtr = shaderText.c_str();

		glShaderSource(vertexID, 1, &shaderTextPtr, nullptr);
		glCompileShader(vertexID);

		glGetShaderiv(vertexID, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE) {

			glGetShaderInfoLog(vertexID, 1024, nullptr, buff);

			OutputDebugStringA(buff);
			FY_CORE_WARN("Could not create shader!");
		}

		//Read fragment shader
		memset(buff, 0, 1024);
		compileResult = 0;
		fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

		std::ifstream shaderFile2(fragmentSrc);
		std::string shaderText2((std::istreambuf_iterator<char>(shaderFile2)), std::istreambuf_iterator<char>());
		shaderFile2.close();

		const char* shaderTextPtr2 = shaderText2.c_str();

		glShaderSource(fragmentID, 1, &shaderTextPtr2, nullptr);
		glCompileShader(fragmentID);

		glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE) {

			glGetShaderInfoLog(fragmentID, 1024, nullptr, buff);

			OutputDebugStringA(buff);
			FY_CORE_WARN("Could not create shader!");
		}

		//Link shader program
		memset(buff, 0, 1024);
		compileResult = 0;

		m_RendererID = glCreateProgram();

		glAttachShader(m_RendererID, vertexID);
		glAttachShader(m_RendererID, fragmentID);
		glLinkProgram(m_RendererID);

		compileResult = GL_FALSE;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &compileResult);
		if (compileResult == GL_FALSE) {
			// query information about the compilation (nothing if compilation went fine!)
			memset(buff, 0, 1024);
			glGetProgramInfoLog(m_RendererID, 1024, nullptr, buff);
			// print to Visual Studio debug console output
			OutputDebugStringA(buff);
			FY_CORE_WARN("Could not link shader program");
		}

		glDetachShader(m_RendererID, vertexID);
		glDetachShader(m_RendererID, fragmentID);
		glDeleteShader(fragmentID);
		glDeleteShader(vertexID);

		//// Create an empty vertex shader handle
		//GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		//// Send the vertex shader source code to GL
		//// Note that std::string's .c_str is NULL character terminated.
		//const GLchar *source = vertexSrc.c_str();
		//glShaderSource(vertexShader, 1, &source, 0);
		//
		//// Compile the vertex shader
		//glCompileShader(vertexShader);

		//GLint isCompiled = 0;
		//glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		//if (isCompiled == GL_FALSE)
		//{
		//	GLint maxLength = 0;
		//	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		//	// The maxLength includes the NULL character
		//	std::vector<GLchar> infoLog(maxLength);
		//	glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

		//	// We don't need the shader anymore.
		//	glDeleteShader(vertexShader);

		//	// Use the infoLog as you see fit.

		//	// In this simple program, we'll just leave
		//	return;
		//}

		//// Create an empty fragment shader handle
		//GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		//// Send the fragment shader source code to GL
		//// Note that std::string's .c_str is NULL character terminated.
		//source = (const GLchar *)fragmentSrc.c_str();
		//glShaderSource(fragmentShader, 1, &source, 0);

		//// Compile the fragment shader
		//glCompileShader(fragmentShader);

		//glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		//if (isCompiled == GL_FALSE)
		//{
		//	GLint maxLength = 0;
		//	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		//	// The maxLength includes the NULL character
		//	std::vector<GLchar> infoLog(maxLength);
		//	glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

		//	// We don't need the shader anymore.
		//	glDeleteShader(fragmentShader);
		//	// Either of them. Don't leak shaders.
		//	glDeleteShader(vertexShader);

		//	// Use the infoLog as you see fit.
		//	FY_CORE_ERROR("{0}", infoLog.data());
		//	FY_CORE_ASSERT(false, "Vertex shader compilation failure!");

		//	// In this simple program, we'll just leave
		//	return;
		//}

		//// Vertex and fragment shaders are successfully compiled.
		//// Now time to link them together into a program.
		//// Get a program object.
		//m_RendererID = glCreateProgram();
		//GLuint program = m_RendererID;

		//// Attach our shaders to our program
		//glAttachShader(program, vertexShader);
		//glAttachShader(program, fragmentShader);

		//// Link our program
		//glLinkProgram(program);

		//// Note the different functions here: glGetProgram* instead of glGetShader*.
		//GLint isLinked = 0;
		//glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
		//if (isLinked == GL_FALSE)
		//{
		//	GLint maxLength = 0;
		//	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//	// The maxLength includes the NULL character
		//	std::vector<GLchar> infoLog(maxLength);
		//	glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		//	// We don't need the program anymore.
		//	glDeleteProgram(program);
		//	// Don't leak shaders either.
		//	glDeleteShader(vertexShader);
		//	glDeleteShader(fragmentShader);

		//	// Use the infoLog as you see fit.
		//	FY_CORE_ERROR("{0}", infoLog.data());
		//	FY_CORE_ASSERT(false, "Fragment shader compilation failure!");

		//	// In this simple program, we'll just leave
		//	return;
		//}

		//// Always detach shaders after a successful link.
		//glDetachShader(program, vertexShader);
		//glDetachShader(program, fragmentShader);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::UnBind() const
	{
		glUseProgram(0);
	}

	void Shader::UploadUniformInt(const std::string& name, const int& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void Shader::UploadUniformFloat4(const std::string & name, const glm::vec4 & value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Shader::UploadUniformFloat3(const std::string & name, const glm::vec3 & value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::UploadUniforMat4(const std::string & name, const glm::mat4 & matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
