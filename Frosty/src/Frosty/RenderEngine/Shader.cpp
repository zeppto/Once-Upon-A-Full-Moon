#include "fypch.hpp"
#include "Shader.hpp"
#include "Glad/glad.h"

namespace Frosty
{
		static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		FY_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	Shader::Shader(const std::string& filepath, bool temp)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	//Shader::Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	//	: m_Name(name)
	//{
	//	std::unordered_map<GLenum, std::string> sources;
	//	sources[GL_VERTEX_SHADER] = vertexSrc;
	//	sources[GL_FRAGMENT_SHADER] = fragmentSrc;
	//	Compile(sources);
	//}

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

			GLint maxLength = 0;
			glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexID);

			// Use the infoLog as you see fit.
			FY_CORE_ERROR("{0}", infoLog.data());
			FY_CORE_ASSERT(false, "Shader compilation failure!");

			// In this simple program, we'll just leave
			return;
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
	}

	Shader::Shader(const std::string vertexSrc, const std::string geometrySrc, const std::string fragmentSrc)
	{
		char buff[1024];
		memset(buff, 0, 1024);
		GLint compileResult = 0;

		GLuint vertexID;
		GLuint geometryID;
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

		//Read geometry shader
		memset(buff, 0, 1024);
		compileResult = 0;
		geometryID = glCreateShader(GL_GEOMETRY_SHADER);

		std::ifstream shaderFile2(geometrySrc);
		std::string shaderText2((std::istreambuf_iterator<char>(shaderFile2)), std::istreambuf_iterator<char>());
		shaderFile2.close();

		const char* shaderTextPtr2 = shaderText2.c_str();

		glShaderSource(geometryID, 1, &shaderTextPtr2, nullptr);
		glCompileShader(geometryID);

		glGetShaderiv(geometryID, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE) {

			glGetShaderInfoLog(geometryID, 1024, nullptr, buff);

			OutputDebugStringA(buff);
			FY_CORE_WARN("Could not create shader!");
		}

		//Read fragment shader
		memset(buff, 0, 1024);
		compileResult = 0;
		fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

		std::ifstream shaderFile3(fragmentSrc);
		std::string shaderText3((std::istreambuf_iterator<char>(shaderFile3)), std::istreambuf_iterator<char>());
		shaderFile3.close();

		const char* shaderTextPtr3 = shaderText3.c_str();

		glShaderSource(fragmentID, 1, &shaderTextPtr3, nullptr);
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
		glAttachShader(m_RendererID, geometryID);
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
		glDetachShader(m_RendererID, geometryID);
		glDetachShader(m_RendererID, fragmentID);
		glDeleteShader(fragmentID);
		glDeleteShader(geometryID);
		glDeleteShader(vertexID);
	}

	Shader::Shader(const std::string computeSrc)
	{
		char buff[1024];
		memset(buff, 0, 1024);
		GLint compileResult = 0;

		GLuint ID;

		//Read vertex shader
		ID = glCreateShader(GL_COMPUTE_SHADER);

		std::ifstream shaderFile(computeSrc);
		std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
		shaderFile.close();

		const char* shaderTextPtr = shaderText.c_str();

		glShaderSource(ID, 1, &shaderTextPtr, nullptr);
		glCompileShader(ID);

		glGetShaderiv(ID, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE) {

			glGetShaderInfoLog(ID, 1024, nullptr, buff);

			OutputDebugStringA(buff);
			FY_CORE_WARN("Could not create shader!");
		}

		//Link shader program
		memset(buff, 0, 1024);
		compileResult = 0;

		m_RendererID = glCreateProgram();

		glAttachShader(m_RendererID, ID);
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

		glDetachShader(m_RendererID, ID);
		glDeleteShader(ID);
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

	void Shader::UploadUniformInt(const std::string & name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void Shader::UploadUniformFloat(const std::string & name, const float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void Shader::UploadUniformFloat2(const std::string & name, const glm::vec2 & value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void Shader::UploadUniformFloat3(const std::string & name, const glm::vec3 & value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::UploadUniformFloat4(const std::string & name, const glm::vec4 & value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Shader::UploadUniforMat3(const std::string & name, const glm::mat3 & matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniforMat4(const std::string & name, const glm::mat4 & matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	Shader* Shader::CreateShader(const std::string& filepath)
	{
		return FY_NEW Shader(filepath, true);
	}

	Shader* Shader::CreateShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		return FY_NEW Shader(name, vertexSrc, fragmentSrc);
	}

	std::string Shader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			FY_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			FY_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			FY_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			FY_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos);

			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		FY_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				FY_CORE_ERROR("{0}", infoLog.data());
				FY_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		m_RendererID = program;

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);

			for (auto id : glShaderIDs)
				glDeleteShader(id);

			FY_CORE_ERROR("{0}", infoLog.data());
			FY_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}
	}

	// Shaderlibrary ------------------------------------------------------------

	void ShaderLibrary::Add(const std::string & name, Shader* shader)
	{
		FY_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(Shader* shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Shader * ShaderLibrary::Load(const std::string & filepath)
	{
		auto shader = Shader::CreateShader(filepath);
		Add(shader);
		return shader;
	}

	Shader * ShaderLibrary::Load(const std::string & name, const std::string & filepath)
	{
		auto shader = Shader::CreateShader(filepath);
		Add(name, shader);
		return shader;
	}

	Shader * ShaderLibrary::Get(const std::string & name)
	{
		FY_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string & name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

}
