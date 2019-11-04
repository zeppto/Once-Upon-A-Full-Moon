#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include "Frosty/RenderEngine/VertexArray.hpp"
#include "Frosty/RenderEngine/Shader.hpp"
#include "Frosty/RenderEngine/Texture.hpp"

#include <Luna/include/Luna.h>
#include <Luna/include/Reader.h>

namespace Frosty
{
	struct AnimVert
	{
		float position[3] = { 0.0f };
		float uv[2] = { 0.0f };
		float normal[3] = { 0.0f };
		float tangent[3] = { 0.0f };
		float bitangent[3] = { 0.0f };

		float weights[4] = { 0.0f };
		int joints[4] = { 0 };

	};

	class AssetManager
	{
	protected:
		AssetManager() = default;

	public:
		virtual ~AssetManager() = default;

		static void Init();
		
		// Meshes
		inline static std::shared_ptr<VertexArray>& GetMesh(const std::string& name) { FY_CORE_ASSERT(s_Meshes.count(name), "Mesh error!\n{0} doesn't exist!", name); return s_Meshes[name]; }
		inline static std::map<std::string, std::shared_ptr<VertexArray>>& GetMeshes() { return s_Meshes; }

		// Shaders
		inline static std::shared_ptr<Shader>& GetShader(const std::string& name) { FY_CORE_ASSERT(s_Shaders.count(name), "Shader error!\n{0} doesn't exist!", name); return s_Shaders[name]; }
		inline static std::map<std::string, std::shared_ptr<Shader>>& GetShaders() { return s_Shaders; }

		// Textures
		inline static std::shared_ptr<Texture2D>& GetTexture2D(const std::string& name) { FY_CORE_ASSERT(s_Textures2D.count(name), "Texture2D error!\n{0} doesn't exist!", name); return s_Textures2D[name]; }
		inline static std::map<std::string, std::shared_ptr<Texture2D>>& GetTextures2D() { return s_Textures2D; }

		// Textures
		inline static std::shared_ptr<Luna::BoundingBox>& GetBoundingBox(const std::string& name) { return s_BoundingBoxes[name]; }
		inline static std::map<std::string, std::shared_ptr<Luna::BoundingBox>>& GetBoundingBoxes() { return s_BoundingBoxes; }

	private:
		// Meshes
		static void AddMesh(const std::string& name, const std::string& filepath);
		static void AddAnimatedMesh(const std::string& name, const std::string& filepath);
		static std::vector<AnimVert> MakeAnimVerts(Luna::Reader & tmpFile);

		// Textures
		static void LoadTexture2D(const std::string& name, const std::string& filepath);

	private:
		static std::map<std::string, std::shared_ptr<VertexArray>> s_Meshes;
		static std::map<std::string, std::shared_ptr<Shader>> s_Shaders;
		static std::map<std::string, std::shared_ptr<Texture2D>> s_Textures2D;
		static std::map<std::string, std::shared_ptr<Luna::BoundingBox>> s_BoundingBoxes;

	};
}

#endif // !ASSET_MANAGER_HPP
