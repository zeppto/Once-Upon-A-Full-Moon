#include <fypch.hpp>
#include "AssetManager.hpp"


namespace Frosty
{
	std::map<std::string, std::shared_ptr<Frosty::VertexArray>> AssetManager::s_Meshes;
	std::map<std::string, std::shared_ptr<Frosty::Shader>> AssetManager::s_Shaders;
	std::map<std::string, std::shared_ptr<Frosty::Texture2D>> AssetManager::s_Textures2D;
	std::map<std::string, std::shared_ptr<Luna::BoundingBox>> AssetManager::s_BoundingBoxes;

	void AssetManager::Init()
	{
		// Move all this code later out to front-end
		//AddMesh("Clock", "assets/models/clock/clock.lu");
		//AddMesh("Table", "assets/models/table/table.lu");

		AddMesh("Plane", "assets/primitives/plane/plane.lu");
		AddMesh("Sphere", "assets/primitives/sphere/sphere.lu");
		AddMesh("Icosahedron", "assets/primitives/icosahedron/icosahedron.lu");
		AddMesh("Donut", "assets/primitives/donut/donut.lu");
		AddMesh("Disc", "assets/primitives/disc/disc.lu");
		AddMesh("Cylinder", "assets/primitives/cylinder/cylinder.lu");
		AddMesh("Cube", "assets/primitives/cube/cube.lu");
		AddMesh("Cone", "assets/primitives/cone/cone.lu");
		AddMesh("3D", "assets/primitives/3D/3D.lu");
		AddMesh("CharTest", "assets/models/character_test/char_test.lu");
		AddMesh("Scarlet", "assets/models/Scarlet_static/Scarlet_static.lu");

		AddMesh("Tree1", "assets/models/trees/tree1.lu");
		AddMesh("Tree2", "assets/models/trees/tree2.lu");
		AddMesh("Tree3", "assets/models/trees/tree3.lu");
		AddMesh("Tree4", "assets/models/trees/tree4.lu");
		AddMesh("Tree5", "assets/models/trees/tree5.lu");
		AddMesh("Tree6", "assets/models/trees/tree6.lu");
		AddMesh("Tree7", "assets/models/trees/tree7.lu");
		AddMesh("Tree8", "assets/models/trees/tree8.lu");
		AddMesh("Tree9", "assets/models/trees/tree9.lu");
		AddMesh("Tree10", "assets/models/trees/tree10.lu");

		s_Shaders.emplace("FlatColor", FY_NEW Shader("assets/shaders/FlatColor.glsl", "FlatColor"));
		s_Shaders.emplace("Texture2D", FY_NEW Shader("assets/shaders/Texture2D.glsl", "Texture2D"));
		s_Shaders.emplace("UI", FY_NEW Shader("assets/shaders/UI.glsl", "UI"));

		LoadTexture2D("Clock Diffuse", "assets/textures/clock_diffuse.png");
		//LoadTexture2D("Clock Normal", "assets/textures/clock_normal.png");
		//LoadTexture2D("Clock Specular", "assets/textures/clock_specular.png");
		LoadTexture2D("Table Diffuse", "assets/textures/polySurface15_diffuse.png");
		LoadTexture2D("Brown Mud Diffuse", "assets/textures/brown_mud_diffuse.png");
		LoadTexture2D("Checkerboard", "assets/textures/Checkerboard.png");
		LoadTexture2D("Rusty Metal Diffuse", "assets/textures/rusty_metal_diffuse.png");
		LoadTexture2D("Rusty Metal Normal", "assets/textures/rusty_metal_normal.png");
		LoadTexture2D("Rusty Metal Specular", "assets/textures/rusty_metal_specular.png");
		LoadTexture2D("Forest Ground", "assets/textures/forest_ground.jpg");
		//LoadTexture2D("Normal Test", "assets/textures/cube_normal.png");					// TEMPORARY
		//LoadTexture2D("Cherno Logo", "assets/textures/ChernoLogo.png");					// TEMPORARY
		LoadTexture2D("Heart", "assets/textures/heart.png");
		LoadTexture2D("HeartFull", "assets/textures/heartFull.png");
		LoadTexture2D("Sword", "assets/textures/sword.jpg");
		LoadTexture2D("Bow", "assets/textures/bow.png");
		LoadTexture2D("Red", "assets/textures/red.png");
		LoadTexture2D("Scarlet_diffuse", "assets/textures/Scarlet_diffuse.tga");
		LoadTexture2D("Scarlet_normal", "assets/textures/Scarlet_normal.tga");
		LoadTexture2D("Scarlet_specular", "assets/textures/Scarlet_specular.tga");


		LoadTexture2D("Tree1", "assets/textures/testPineLeves2.tga");						// TEMPORARY
		LoadTexture2D("Tree2", "assets/textures/testPineLeves4.tga");						// TEMPORARY
		LoadTexture2D("Tree3", "assets/textures/testPineLeves5.tga");						// TEMPORARY
		LoadTexture2D("Tree4", "assets/textures/testPineLeves6.tga");						// TEMPORARY
		LoadTexture2D("Tree5", "assets/textures/testPineLeves7.tga");						// TEMPORARY
		LoadTexture2D("Tree6", "assets/textures/testPineLeves10.tga");						// TEMPORARY
		LoadTexture2D("Tree7", "assets/textures/testPineLeves11.tga");						// TEMPORARY
		LoadTexture2D("Tree8", "assets/textures/testPineLeves12.tga");						// TEMPORARY
		LoadTexture2D("Tree9", "assets/textures/testPineLeves13.tga");						// TEMPORARY
		LoadTexture2D("Tree10", "assets/textures/testPineLeves14.tga");						// TEMPORARY
		
		s_Shaders["Texture2D"]->Bind();

		// Clock
		s_Shaders["Texture2D"]->UploadUniformInt("u_DiffuseTexture", 0);
		s_Shaders["Texture2D"]->UploadUniformInt("u_NormalTexture", 1);
		s_Shaders["Texture2D"]->UploadUniformInt("u_SpecularTexture", 2);

		s_Shaders["UI"]->Bind();

		s_Shaders["UI"]->UploadUniformInt("u_DiffuseTexture", 0);
	}

	void AssetManager::AddMesh(const std::string& name, const std::string& filepath)
	{
		// Read file
		Luna::Reader tempFile;
		FY_CORE_ASSERT(tempFile.readFile(filepath.c_str()), "Failed to load file {0}!", filepath);

		// Retrieve vertices and calculate indices
		std::vector<Luna::Vertex> vertices;
		std::vector<Luna::Index> indices;
		tempFile.getVertices(0, vertices);
		tempFile.getIndices(0, indices);

		// Bounding Box
		s_BoundingBoxes.emplace(name, FY_NEW Luna::BoundingBox(tempFile.getBoundingBox(0)));

		// Vertex Array
		s_Meshes.emplace(name, VertexArray::Create());

		// Vertex Buffer
		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(&vertices.front(), sizeof(Luna::Vertex) * (uint32_t)vertices.size()));

		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TextureCoords" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Bitangent" }
		};
		vertexBuffer->SetLayout(layout);

		s_Meshes[name]->AddVertexBuffer(vertexBuffer);

		// Index Buffer
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(&indices.front(), (uint32_t)indices.size()));
		s_Meshes[name]->SetIndexBuffer(indexBuffer);
	}

	void AssetManager::LoadTexture2D(const std::string& name, const std::string& filepath)
	{
		s_Textures2D.emplace(name, FY_NEW Texture2D(name, filepath));
	}

}