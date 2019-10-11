#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include "Frosty/RenderEngine/VertexArray.hpp"
#include "Frosty/RenderEngine/Shader.hpp"
#include "Frosty/RenderEngine/Texture.hpp"

#include <Luna/include/Luna.h>
#include <Luna/include/Reader.h>

namespace Frosty
{
	class AssetManager
	{
	protected:
		AssetManager() = default;

	public:
		virtual ~AssetManager() = default;

		static void Init();
		
		// Meshes
		inline static std::shared_ptr<VertexArray>& GetMesh(const std::string& name) { return s_Meshes[name]; }
		inline static std::map<std::string, std::shared_ptr<VertexArray>>& GetMeshes() { return s_Meshes; }

		// Shaders
		inline static std::shared_ptr<Shader>& GetShader(const std::string& name) { return s_Shaders[name]; }
		inline static std::map<std::string, std::shared_ptr<Shader>>& GetShaders() { return s_Shaders; }

		// Textures
		inline static std::shared_ptr<Texture2D>& GetTexture2D(const std::string& name) { return s_Textures2D[name]; }
		inline static std::map<std::string, std::shared_ptr<Texture2D>>& GetTextures2D() { return s_Textures2D; }

	private:
		// Meshes
		static void AddMesh(const std::string& name, const std::string& filepath);
		static void AddQuadMesh();		// Remove this later

		// Shaders
		static void AddStandardShader();
		static void AddFlatColorShader();
		static void AddTextureShader();

		// Textures
		static void LoadTexture2D(const std::string& name, const std::string& filepath);

	private:
		static std::map<std::string, std::shared_ptr<VertexArray>> s_Meshes;
		static std::map<std::string, std::shared_ptr<Shader>> s_Shaders;
		static std::map<std::string, std::shared_ptr<Texture2D>> s_Textures2D;

	};
}

#endif // !ASSET_MANAGER_HPP
