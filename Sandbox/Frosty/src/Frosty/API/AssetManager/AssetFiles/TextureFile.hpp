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
		int m_Components{ 0 };
		int m_Image_Width{ 0 };
		int m_Image_Height{ 0 };

		unsigned char* m_ImageData{ nullptr };


	public:		//Functions
		inline TextureFile() : AssetFile(FileMetaData()) {}
		TextureFile(const FileMetaData& MetaData);
		//TextureFile(const std::string& FilePath);
		virtual ~TextureFile();

		const int& GetComponents() { return m_Components; }
		const int& GetImageWidth() { return m_Image_Height; }
		const int& GetImageHeight() { return m_Image_Height; }


		bool LoadToMem(const bool& Reload = false);
		bool DeleteFromMem();

		bool LoadToGPU();
		bool DeleteFromGPU();


	private:	//Functions



	};

}



#endif // !TEXTUREFILE_HPP
