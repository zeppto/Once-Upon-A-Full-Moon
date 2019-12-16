#ifndef MATERIALHANDLER_HPP
#define MATERIALHANDLER_HPP


#include "Frosty/RenderEngine/Shader.hpp"
#include "Frosty/RenderEngine/Texture.hpp"

namespace Frosty
{
	struct Material
	{
		Material() {};
		Material(
			std::string name,
			std::shared_ptr<Shader> useShader,
			glm::vec4 albedo,
			std::shared_ptr<Texture2D> diffuseTexture,
			std::shared_ptr<Texture2D> specularTexture,
			std::shared_ptr<Texture2D> normalTexture,
			std::shared_ptr<Texture2D> blendMapTexture,
			std::shared_ptr<Texture2D> blendTexture1,
			std::shared_ptr<Texture2D> blendTexture2,
			float flash,
			float specularStrength,
			int shininess,
			glm::vec2 textureScale,
			bool hasTransparency) 
		: Name(name), UseShader(useShader), Albedo(albedo), DiffuseTexture(diffuseTexture), SpecularTexture(specularTexture), NormalTexture(normalTexture), BlendMapTexture(blendMapTexture), BlendTexture1(blendTexture1), BlendTexture2(blendTexture2), Flash(flash), SpecularStrength(specularStrength), Shininess(shininess), TextureScale(textureScale), HasTransparency(hasTransparency) {};

		static const unsigned int MAXIMUM_SHININESS = 256;

		std::string Name{ "" };
		std::shared_ptr<Shader> UseShader{ nullptr };
		glm::vec4 Albedo{ 0.f, 0.f, 1.f, 1.f };
		std::shared_ptr<Texture2D> DiffuseTexture{ nullptr };
		std::shared_ptr<Texture2D> SpecularTexture{ nullptr };
		std::shared_ptr<Texture2D> NormalTexture{ nullptr };
		std::shared_ptr<Texture2D> BlendMapTexture{ nullptr };
		std::shared_ptr<Texture2D> BlendTexture1{ nullptr };
		std::shared_ptr<Texture2D> BlendTexture2{ nullptr };
		
		float Flash{ 0.f };
		float SpecularStrength{ 0.5f };
		int Shininess{ 16 };
		glm::vec2 TextureScale{ 1.f };

		bool HasTransparency{ false };
	};

	class MaterialHandler
	{
	public:
		MaterialHandler() {};
		virtual ~MaterialHandler() {};
		MaterialHandler(const MaterialHandler& org) { FY_CORE_ASSERT(false, "Copy Constructor in MaterialHandler called."); };
		MaterialHandler& operator=(const MaterialHandler& org) { FY_CORE_ASSERT(false, "Assignment Operator in MaterialHandler called."); return *this; };

		bool LoadMaterial(const std::string& filePath);

		const int GetNumberOfMaterials();
		const Material& GetMaterialAt(size_t index);
		const std::vector<Material>& GetAllMaterials();

		// Returns a material depending on the name required
		const Material& GetMaterialByName(std::string name);

	private:
		std::vector<Material> m_Materials;
	};
}
#endif // !MATERIALHANDLER_HPP