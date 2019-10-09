#ifndef SHADER_HPP
#define SHADER_HPP
#include "Frosty/DEFINITIONS.hpp"

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Frosty
{	
	class Shader
	{
	public:	
		Shader(const std::string& filepath);
		Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~Shader();

		void Bind()const;
		void UnBind()const;	

		virtual const std::string& GetName() const { return m_Name; }

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformFloat(const std::string& name, const float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniforMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniforMat4(const std::string& name, const glm::mat4& matrix);		

		static Shader* CreateShader(const std::string& filepath);
		static Shader* CreateShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);		
	private:
		uint32_t m_RendererID;
		std::vector<unsigned int> m_ShaderProgramVector;
		std::string m_Name;
	};
}
#endif
