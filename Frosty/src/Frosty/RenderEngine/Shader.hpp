#ifndef SHADER_HPP
#define SHADER_HPP

namespace Frosty
{
	class Shader
	{
	public:
		Shader(const std::string& computeSrc);
		virtual ~Shader();

		void Bind() const;
		void Unbind() const;

	private:
		uint32_t m_RendererID;
	};
}
#endif