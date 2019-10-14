#ifndef LINKEDMATERIAL_HPP
#define LINKEDMATERIAL_HPP
#include "Luna/include/Luna.h"
#include "TextureFile.hpp"
#include"AssetFile.hpp"


namespace Frosty
{


	class LinkedMaterial : public AssetFile
	{
		
	public:


	private:

		Luna::Material m_LunaMaterial;

		TextureFile* m_Glow_Texture_Ptr;
		TextureFile* m_Normal_Texture_Ptr;
		TextureFile* m_Diffuse_Texture_Ptr;


	public:
		inline LinkedMaterial()
		   :m_Glow_Texture_Ptr(nullptr),
			m_Normal_Texture_Ptr(nullptr),
			m_Diffuse_Texture_Ptr(nullptr),
			m_LunaMaterial(Luna::Material()),
			AssetFile(FileMetaData()) {}



		inline LinkedMaterial(const FileMetaData& MetaData, const Luna::Material& LunMat)
			: m_Glow_Texture_Ptr(nullptr),
			m_Normal_Texture_Ptr(nullptr),
			m_Diffuse_Texture_Ptr(nullptr),
			m_LunaMaterial(LunMat),
			AssetFile(MetaData){}
		virtual ~LinkedMaterial();

		void LinkMaterial();

		inline TextureFile*& GetGlow() { return m_Glow_Texture_Ptr; }
		inline TextureFile*& GetNormal() { return m_Normal_Texture_Ptr; }
		inline TextureFile*& GetDiffuse() { return m_Diffuse_Texture_Ptr; }
		inline const Luna::Material& GetMaterial() { return m_LunaMaterial; }

		inline void SetGlow(TextureFile& TextureFile) { m_Glow_Texture_Ptr = &TextureFile; }
		inline void SetNormal(TextureFile& TextureFile) { m_Normal_Texture_Ptr = &TextureFile; }
		inline void SetDiffuse(TextureFile& TextureFile) { m_Diffuse_Texture_Ptr = &TextureFile; }
		inline void SetMaterial(const Luna::Material& Material) { m_LunaMaterial = Material; }

		virtual bool LoadToMem(const bool& Reload = false);
		virtual bool DeleteFromMem();

		virtual bool LoadToGPU();
	    virtual bool DeleteFromGPU();

	private:


		const std::string CutFileExtentionFromString(const char* in_char_ptr);



	};

}
#endif // !LINKEDMATERIAL_HPP
