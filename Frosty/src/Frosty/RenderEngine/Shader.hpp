#ifndef SHADER_HPP
#define SHADER_HPP

namespace Frosty
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& shaderName = "Unnamed");
		virtual ~Shader();

		void Bind()const;
		void UnBind()const;

		void UploadUniformInt(const std::string& name, int value);

		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

		const std::string& GetName() const { return m_Name; }

	private:
		std::string m_Name;			// Only used for editor
		uint32_t m_RendererID;
	};
}
#endif // !SHADER_HPP
