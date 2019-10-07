#ifndef SHADER_HPP
#define SHADER_HPP

namespace Frosty
{
	class Shader
	{
	public:
		Shader(const std::string vertexSrc, const std::string fragmentSrc);
		Shader(const std::string vertexSrc, const std::string geometrySrc, const std::string fragmentSrc);
		virtual ~Shader();

		void Bind()const;
		void UnBind()const;

		void UploadUniformInt(const std::string& name, const int& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniforMat4(const std::string& name, const glm::mat4& matrix);
	private:
		uint32_t m_RendererID;
	};
}
#endif // !SHADER_HPP
