#ifndef SHADER_HPP
#define SHADER_HPP

// TODO: Remove later on!
typedef unsigned int GLenum;

namespace Frosty
{	
	class Shader
	{
	public:
		//Shader(const std::string & filepath, const std::string& shaderName = "Unnamed");
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& shaderName = "Unnamed");
		virtual ~Shader();

		void Bind()const;
		void UnBind()const;	

		const std::string& GetName() const { return m_Name; }

		void UploadUniformInt(const std::string& name, int value);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformFloatArray(const std::string& name, float value);
		void UploadUniformFloat2Array(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3Array(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4Array(const std::string& name, const glm::vec4& value);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);	

	private:
		std::string m_Name;
		uint32_t m_RendererID;
	};

	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, const std::shared_ptr<Shader>& shader);
		void Add(const std::shared_ptr<Shader>& shader);

		std::shared_ptr<Shader> Load(const std::string& filepathVertex, const std::string& filepathFragment);
		std::shared_ptr<Shader> Load(const std::string& name, const std::string& filepathVertex, const std::string& filepathFragment);

		std::shared_ptr<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
	};
}
#endif