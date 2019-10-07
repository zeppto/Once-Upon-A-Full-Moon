#ifndef SHADER_HPP
#define SHADER_HPP
#include "Frosty/DEFINITIONS.hpp"

namespace Frosty
{
	enum ShaderProgramsEnum
	{
		STANDARD = 0,
		GEOMETRY = 1
	};
	class Shader
	{
	public:		
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~Shader();

		void Bind()const;
		void UnBind()const;	

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformFloat(const std::string& name, const float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniforMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniforMat4(const std::string& name, const glm::mat4& matrix);		
		
		std::string GetShaderProgramEnumString(ShaderProgramsEnum theEnum);

		static Shader* CreateShader(const std::string vertexShaderPath, const std::string fragmentShaderPath);
		static Shader* CreateShader(const std::string vertexShaderPath, const std::string geometryShaderPath, const std::string fragmentShaderPath);
	private:
		void CreateShaderProgram(const std::string vertexShaderPath, const std::string fragmentShaderPath, ShaderProgramsEnum program);
		void CreateAllShaderPrograms();
	private:
		uint32_t m_RendererID;
		std::vector<unsigned int> m_ShaderProgramVector;
	};
}
#endif
