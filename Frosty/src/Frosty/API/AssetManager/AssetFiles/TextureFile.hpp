#ifndef TEXTUREFILE_HPP
#define TEXTUREFILE_HPP

namespace Frosty
{

	class TextureFile
	{
	public:		//Variables

	private:	//Variables

		bool m_Loaded_In_Gpu;
		uint8_t m_Components;
		uint16_t m_Image_Width;
		uint16_t m_Image_Height;
		uint16_t  m_Buffer_ID;
		std::string m_FilePath;


	public:		//Functions
		TextureFile();
		TextureFile(const std::string& FilePath);
		virtual ~TextureFile();

		const bool& LoadedInGpu() { return m_Loaded_In_Gpu; }
		const uint8_t& GetComponents() { return m_Components; }
		const uint16_t& GetBufferID() { return m_Buffer_ID; }
		const uint16_t& GetImageWidth() { return m_Image_Height; }
		const uint16_t& GetImageHeight() { return m_Image_Height; }

		const std::string& GetFilePath() { return m_FilePath; }
		void SetFilePath(const std::string& FilePath) { m_FilePath = FilePath; } // unload?


		bool LoadToGpu();
		bool LoadToGpu(const std::string& FilePath);
		void ReleaseFromGpu();


	private:	//Functions



	};

}



#endif // !TEXTUREFILE_HPP
