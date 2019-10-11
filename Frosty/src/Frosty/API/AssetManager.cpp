#include <fypch.hpp>
#include "AssetManager.hpp"

namespace Frosty
{
	std::map<std::string, std::shared_ptr<Frosty::VertexArray>> AssetManager::s_Meshes;
	std::map<std::string, std::shared_ptr<Frosty::Shader>> AssetManager::s_Shaders;
	std::map<std::string, std::shared_ptr<Frosty::Texture2D>> AssetManager::s_Textures2D;

	void AssetManager::Init()
	{
		// Move all this code later out to front-end
		AddMesh("Clock", "assets/models/clock/clock.lu");
		AddMesh("Table", "assets/models/table/table.lu");

		s_Shaders.emplace("FlatColor", FY_NEW Shader("assets/shaders/FlatColor.glsl", "FlatColor"));
		s_Shaders.emplace("Texture2D", FY_NEW Shader("assets/shaders/Texture2D.glsl", "Texture2D"));

		LoadTexture2D("Clock_Diffuse", "assets/textures/pCube10_diffuse.png");
		LoadTexture2D("Clock_Gloss", "assets/textures/pCube10_gloss.png");
		LoadTexture2D("Clock_Normal", "assets/textures/pCube10_normal.png");
		LoadTexture2D("Table_Diffuse", "assets/textures/polySurface15_diffuse.png");

		s_Shaders["Texture2D"]->Bind();
		s_Shaders["Texture2D"]->UploadUniformInt("u_DiffuseTexture", 0);
		s_Shaders["Texture2D"]->UploadUniformInt("u_GlossTexture", 1);
		s_Shaders["Texture2D"]->UploadUniformInt("u_NormalTexture", 2);
	}

	void AssetManager::AddMesh(const std::string& name, const std::string& filepath)
	{
		// Read file
		Luna::Reader tempFile;
		FY_CORE_ASSERT(tempFile.readFile(filepath.c_str()), "Failed to load file {0}!", filepath);

		// Retrieve vertices and calculate indices
		std::vector<Luna::Vertex> lunaVertices;
		std::vector<Luna::Vertex> vertices;

		tempFile.getVertices(0, lunaVertices);
		std::vector<uint32_t> indices;
		unsigned int indexCounter = 0;
		int index = -1;
		for (unsigned int i = 0; i < lunaVertices.size(); i++)
		{
			index = -1;
			for (unsigned int j = 0; j < vertices.size() && index != -1; j++)
			{
				if (vertices[j] == lunaVertices[i])
				{
					index = j;
				}
			}

			if (index == -1)
			{
				vertices.emplace_back(lunaVertices[i]);
				indices.emplace_back(indexCounter++);
			}
			else
			{
				indices.emplace_back(index);
			}
		}


		// Vertex Array
		s_Meshes.emplace(name, VertexArray::Create());

		// Vertex Buffer
		FY_CORE_INFO("{0}", sizeof(Luna::Vertex));
		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(&vertices.front(), sizeof(Luna::Vertex) * (uint64_t)vertices.size()));

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
		indexBuffer.reset(IndexBuffer::Create(&indices.front(), indices.size()));
		s_Meshes[name]->SetIndexBuffer(indexBuffer);

	}

	void AssetManager::AddQuadMesh()
	{
		s_Meshes.emplace("Quad", VertexArray::Create());

		float vertices[4 * 5] = {
			-0.5f, -0.5f, 0.0f, 0.0, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0, 1.0f
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TextureCoords" }
		};

		vertexBuffer->SetLayout(layout);
		s_Meshes["Quad"]->AddVertexBuffer(vertexBuffer);

		uint32_t indices[2 * 3] = {
			0, 1, 2,
			2, 3, 0
		};
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		s_Meshes["Quad"]->SetIndexBuffer(indexBuffer);
	}

	void AssetManager::LoadTexture2D(const std::string& name, const std::string& filepath)
	{
		s_Textures2D.emplace(name, FY_NEW Texture2D(name, filepath));
	}
}