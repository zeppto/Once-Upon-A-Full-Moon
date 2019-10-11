#ifndef TEXTURE_HPP
#define TEXTURE_HPP

namespace Frosty
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual std::string& GetName() = 0;
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(const std::string& name, const std::string& path);
		virtual ~Texture2D();

		virtual std::string& GetName() override { return m_Name; }
		virtual const std::string& GetName() const override { return m_Name; }
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void Bind(uint32_t slot = 0) const override;

	private:
		std::string m_Name;
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;

	};
}

#endif // !TEXTURE_HPP
