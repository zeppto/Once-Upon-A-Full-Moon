#ifndef TEXTUREFILE_HPP
#define TEXTUREFILE_HPP

#include "AssetFile.hpp"

namespace Frosty
{

	class TextureFile : public AssetFile
	{
	public:		//Variables

	private:	//Variables

	//	Need to be int for stbi loader
		int m_Components;
		int m_Image_Width;
		int m_Image_Height;

		unsigned char* m_ImageData;


	public:		//Functions
		inline TextureFile() : AssetFile(FileMetaData()) {}
		TextureFile(const FileMetaData& MetaData);
		//TextureFile(const std::string& FilePath);
		virtual ~TextureFile();

		const int& GetComponents() { return m_Components; }
		const int& GetImageWidth() { return m_Image_Height; }
		const int& GetImageHeight() { return m_Image_Height; }


		virtual bool LoadToMem(const bool& Reload = false);
		virtual bool DeleteFromMem();

		virtual bool LoadToGPU();
		virtual bool DeleteFromGPU();


	private:	//Functions



	};

}



#endif // !TEXTUREFILE_HPP
