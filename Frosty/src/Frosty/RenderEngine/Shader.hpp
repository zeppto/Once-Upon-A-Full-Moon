#ifndef SHADER_HPP
#define SHADER_HPP

namespace Frosty
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~Shader();

		void Bind()const;
		void UnBind()const;
	private:
		uint32_t m_RendererID;
	};
}
#endif // !SHADER_HPP
