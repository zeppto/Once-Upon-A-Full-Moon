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
		AddMesh("AnimTest", "assets/models/animTestCuboid/AnimTestCuboid.lu");

		s_Shaders.emplace("FlatColor", FY_NEW Shader("assets/shaders/FlatColor.glsl", "FlatColor"));
		s_Shaders.emplace("Texture2D", FY_NEW Shader("assets/shaders/Texture2D.glsl", "Texture2D"));
		s_Shaders.emplace("AnimShader", FY_NEW Shader("assets/shaders/AnimSafe.glsl", "AnimShader"));
		s_Shaders.emplace("UI", FY_NEW Shader("assets/shaders/UI.glsl", "UI"));

		LoadTexture2D("Clock Diffuse", "assets/textures/clock_diffuse.png");
		LoadTexture2D("Clock Normal", "assets/textures/clock_normal.png");
		LoadTexture2D("Clock Specular", "assets/textures/clock_specular.png");
		LoadTexture2D("Table Diffuse", "assets/textures/polySurface15_diffuse.png");
		LoadTexture2D("Brown Mud Diffuse", "assets/textures/brown_mud_diffuse.png");
		LoadTexture2D("Checkerboard", "assets/textures/Checkerboard.png");
		LoadTexture2D("Rusty Metal Diffuse", "assets/textures/rusty_metal_diffuse.png");
		LoadTexture2D("Rusty Metal Normal", "assets/textures/rusty_metal_normal.png");
		LoadTexture2D("Rusty Metal Specular", "assets/textures/rusty_metal_specular.png");
		LoadTexture2D("Normal Test", "assets/textures/cube_normal.png");					// TEMPORARY
		LoadTexture2D("Cherno Logo", "assets/textures/ChernoLogo.png");						// TEMPORARY
		LoadTexture2D("Heart", "assets/textures/heart.png");
		LoadTexture2D("HeartFull", "assets/textures/heartFull.png");
		LoadTexture2D("Sword", "assets/textures/sword.png");

		s_Shaders["Texture2D"]->Bind();

		// Clock
		s_Shaders["Texture2D"]->UploadUniformInt("u_DiffuseTexture", 0);
		s_Shaders["Texture2D"]->UploadUniformInt("u_GlossTexture", 1);
		s_Shaders["Texture2D"]->UploadUniformInt("u_NormalTexture", 2);

		s_Shaders["AnimShader"]->Bind();
		
		s_Shaders["AnimShader"]->UploadUniformInt("u_DiffuseTexture", 0);
		s_Shaders["AnimShader"]->UploadUniformInt("u_GlossTexture", 1);
		s_Shaders["AnimShader"]->UploadUniformInt("u_NormalTexture", 2);

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

		if (tempFile.getAnimation().keyframeCount != 0)
		{
			std::vector<Luna::Joint> joints;
			tempFile.getJoints(joints);
			std::map<uint16_t, std::vector<Luna::Keyframe>> kMap;

			for (int i = 0; i < joints.size(); i++)
			{
				std::vector<Luna::Keyframe> keyFrames;
				tempFile.getKeyframes(joints[i].jointID, keyFrames);

				kMap[joints[i].jointID] = keyFrames;
			}

			s_Meshes[name]->setMeshAnims(&tempFile.getAnimation(), joints, &kMap);
			std::shared_ptr<UniformBuffer> uniformBuffer;
			uint32_t MAX_BONES = 64;
			uniformBuffer.reset(UniformBuffer::Create(joints.size(), MAX_BONES));
			s_Meshes[name]->SetUniformBuffer(uniformBuffer);

		}

	}

	void AssetManager::LoadTexture2D(const std::string& name, const std::string& filepath)
	{
		s_Textures2D.emplace(name, FY_NEW Texture2D(name, filepath));
	}

}