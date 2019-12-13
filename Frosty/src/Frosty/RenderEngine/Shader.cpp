#include "fypch.hpp"
#include "Shader.hpp"

#include <Glad/glad.h>

namespace Frosty
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex" || type == "vertex\t")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		FY_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& shaderName)
		: m_Name(shaderName)
	{
		std::string vertexSource = ReadFile(vertexSrc);
		std::string fragmentSource = ReadFile(fragmentSrc);

		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSource;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSource;

		Compile(shaderSources);
	}

	Shader::Shader(const std::string& vertexSrc, const std::string& geometrySrc, const std::string& fragmentSrc, const std::string& shaderName)
		: m_Name(shaderName)
	{
		std::string vertexSource = ReadFile(vertexSrc);
		std::string geometrySource = ReadFile(geometrySrc);
		std::string fragmentSource = ReadFile(fragmentSrc);

		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSource;
		shaderSources[GL_GEOMETRY_SHADER] = geometrySource;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSource;

		Compile(shaderSources);
	}

	Shader::Shader(const std::string& computeSrc, const std::string& shaderName)
		: m_Name(shaderName)
	{
		std::string computeSource = ReadFile(computeSrc);

		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_COMPUTE_SHADER] = computeSource;

		Compile(shaderSources);
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

	void Shader::UploadUniformInt(const std::string& name, int value)
	{
		GLuint location = GetUniformLocation(name);
		glUniform1i(location, value);
	}

	void Shader::UploadUniformFloat(const std::string& name, float value)
	{
		GLuint location = GetUniformLocation(name);
		glUniform1f(location, value);
	}

	void Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLuint location = GetUniformLocation(name);
		glUniform2f(location, value.x, value.y);
	}

	void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLuint location = GetUniformLocation(name);
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLuint location = GetUniformLocation(name);
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Shader::UploadUniformFloatArray(const std::string& name, float value)
	{
		GLuint location = GetUniformLocation(name);
		glUniform1fv(location, 1, &value);
	}

	void Shader::UploadUniformFloat2Array(const std::string& name, const glm::vec2& value)
	{
		GLuint location = GetUniformLocation(name);
		glUniform2fv(location, 1, &value[0]);
	}

	void Shader::UploadUniformFloat3Array(const std::string& name, const glm::vec3& value)
	{
		GLuint location = GetUniformLocation(name);
		glUniform3fv(location, 1, &value[0]);
	}

	void Shader::UploadUniformFloat4Array(const std::string& name, const glm::vec4& value)
	{
		GLuint location = GetUniformLocation(name);
		glUniform4fv(location, 1, &value[0]);
	}

	void Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLuint location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLuint location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformMat4Array(const std::string& name, const glm::mat4& matrix)
	{
		GLuint location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix[0]));
	}

	void Shader::AssignUniformBlock(const std::string& name)
	{
		//TODO replace "a_jointDataBlock" with name.
		unsigned int jointDataIndex = glGetUniformBlockIndex(m_RendererID, "a_jointDataBlock");
		glUniformBlockBinding(m_RendererID, jointDataIndex, 1);
	}
	
	uint32_t Shader::GetID() const
	{
		return m_RendererID;
	}
	
	std::string Shader::ReadFile(const std::string& filepath)
	{
		std::string result = "";
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);				// Go to the end of the file
			result.resize(in.tellg());				// Then check the size of the file and resize the string var
			in.seekg(0, std::ios::beg);				// Go back to the begining of the file 
			in.read(&result[0], result.size());		// Read the file into the string var
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
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		//FY_CORE_ASSERT(shaderSources.size() <= 2, "We only support two shaders for now.");
		//std::array<GLenum, 2> glShaderIDs;
		std::vector<GLenum> glShaderIDs(shaderSources.size()); //Initialized to be of correct size to avoid push_back
		int glShaderIndex = 0;

		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const char* sourceCStr = source.c_str();
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
			glShaderIDs[glShaderIndex++] = shader;
		}

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
			{
				glDeleteShader(id);
			}

			// Use the infoLog as you see fit.
			FY_CORE_ERROR("{0}", infoLog.data());
			FY_CORE_ASSERT(false, "Shader link failure!");

			// In this simple program, we'll just leave
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
		}

		m_RendererID = program;
	}

	unsigned int Shader::GetUniformLocation(const std::string& name)
	{
		//GLuint location = -1;
		//if (m_UniformCache.count(name) == 0)
		//{
		//	location = glGetUniformLocation(m_RendererID, name.c_str());
		//	m_UniformCache.emplace(name, location);
		//}
		//else
		//{
		//	location = m_UniformCache[name];
		//}
		GLuint location = glGetUniformLocation(m_RendererID, name.c_str());
		return location;
	}

	void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
	{
		FY_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepathVertex, const std::string& filepathFragment)
	{
		std::shared_ptr<Shader> shader;
		shader.reset(FY_NEW Shader(filepathVertex, filepathFragment, name));
		Add(shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
	{
		FY_CORE_ASSERT(Exists(name), "Shader not found!");

		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}
