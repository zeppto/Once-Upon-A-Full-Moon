#include"fypch.hpp"
#include "MaterialHandler.hpp"
#include <PugiXML/pugixml.hpp>
#include "Frosty/API/AssetManager/AssetManager.hpp"
namespace Frosty
{
	bool MaterialHandler::LoadMaterial(const std::string& filePath)
	{
		// Create neccessary files (XML)
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(filePath.c_str());

		FY_CORE_ASSERT(result, "Error loading file {0}: {1}", "Materials.xml", result.description());


		// Get all nodes
		pugi::xml_node materials = doc.child("materials");

		// Retrieve and save info
		size_t amount = std::distance(materials.children().begin(), materials.children().end());
		m_Materials.reserve(amount);	

		Material material;
		std::string line = "";
		for (pugi::xml_node m = materials.child("material"); m; m = m.next_sibling("material"))
		{
			line = "";

			material.Name = m.attribute("type").as_string();
			line = m.attribute("shader").as_string();
			if(line != "")
				material.UseShader = AssetManager::GetShader(line);
			else
				material.UseShader = AssetManager::GetShader("FlatColor");
			
			pugi::xml_node color = m.child("color");
			material.Albedo.r = color.attribute("r").as_float();
			material.Albedo.g = color.attribute("g").as_float();
			material.Albedo.b = color.attribute("b").as_float();
			material.Albedo.a = color.attribute("a").as_float();

			line = m.attribute("diffuseTexture").as_string();
			if (line != "")
				material.DiffuseTexture = AssetManager::GetTexture2D(line);
			else
				material.DiffuseTexture = nullptr;
			
			line = m.attribute("specularTexture").as_string();
			if (line != "")
				material.SpecularTexture = AssetManager::GetTexture2D(line);
			else
				material.SpecularTexture = nullptr;
			
			line = m.attribute("normalTexture").as_string();
			if (line != "")
				material.NormalTexture = AssetManager::GetTexture2D(line);
			else
				material.NormalTexture = AssetManager::GetTexture2D("FlatNormal");
			
			line = m.attribute("blendMapTexture").as_string();
			if (line != "")
				material.BlendMapTexture = AssetManager::GetTexture2D(line);	
			else
				material.BlendMapTexture = nullptr;
			
			line = m.attribute("blendTexture1").as_string();
			if (line != "")
				material.BlendTexture1 = AssetManager::GetTexture2D(line);
			else
				material.BlendTexture1 = nullptr;
			
			line = m.attribute("blendTexture2").as_string();
			if (line != "")
				material.BlendTexture2 = AssetManager::GetTexture2D(line);
			else
				material.BlendTexture2 = nullptr;

			material.Flash = m.attribute("flash").as_float();
			material.SpecularStrength = m.attribute("specularStrength").as_float();
			material.Shininess = m.attribute("shininess").as_int();

			pugi::xml_node textureScale = m.child("textureScale");
			material.TextureScale.x = textureScale.attribute("x").as_float();		
			material.TextureScale.y = textureScale.attribute("y").as_float();

			material.HasTransparency = m.attribute("hasTransparency").as_bool();

			m_Materials.emplace_back(std::make_shared<Material>(material.Name, material.UseShader, material.Albedo, material.DiffuseTexture, material.SpecularTexture, material.NormalTexture, material.BlendMapTexture, material.BlendTexture1, material.BlendTexture2, material.Flash, material.SpecularStrength, material.Shininess, material.TextureScale, material.HasTransparency));
		}

		return true;
	}
	
	//const int MaterialHandler::GetNumberOfMaterials()
	//{
	//	return int(m_Materials.size());
	//}
	
	//const Material& MaterialHandler::GetMaterialAt(size_t index)
	//{
	//	FY_CORE_ASSERT(index >= m_Materials.size(), "MaterialHandler: Invalid index in GetMaterialAt().");
	//	FY_CORE_ASSERT(index < 0, "MaterialHandler: Invalid index in GetMaterialAt().");

	//	return *m_Materials[index];
	//}
	//
	//const std::vector<std::shared_ptr<Material>&>& MaterialHandler::GetAllMaterials()
	//{
	//	return m_Materials;
	//}
	
	const std::shared_ptr<Material>& MaterialHandler::GetMaterialByName(std::string name)
	{
		size_t index = -1;

		for (size_t i = 0; i < m_Materials.size() && index == -1; i++)
		{
			if (m_Materials[i]->Name == name)
			{
				index = i;
			}
		}
		
		FY_CORE_ASSERT(int(index) >= 0, "MaterialHandler: Invalid name in GetMaterialByName().");

		return m_Materials[index];
	}
}