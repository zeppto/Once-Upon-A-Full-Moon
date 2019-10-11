#include <fypch.hpp>
#include "AssetManager.hpp"

namespace Frosty
{
	std::map<std::string, std::shared_ptr<Frosty::VertexArray>> AssetManager::s_Meshes;
	std::map<std::string, std::shared_ptr<Frosty::Shader>> AssetManager::s_Shaders;
	std::map<std::string, std::shared_ptr<Frosty::Texture2D>> AssetManager::s_Textures2D;

	void AssetManager::Init()
	{
		AddMesh("Clock", "assets/models/clock/clock.lu");
		AddMesh("Table", "assets/models/table/table.lu");

		AddStandardShader();
		AddFlatColorShader();
		AddTextureShader();

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

	void AssetManager::AddStandardShader()
	{
		std::string vs = R"(
			#version 440 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TextureCoords;
			layout(location = 2) in vec3 a_Normal;
			layout(location = 3) in vec3 a_Tangent;
			layout(location = 4) in vec3 a_Bitangent;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main()
			{
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
			}
		)";
		std::string fs = R"(
			#version 440 core

			layout(location = 0) out vec4 color;

			void main()
			{
				color = vec4(1.0, 1.0, 1.0, 1.0);
			}
		)";

		s_Shaders.emplace("Standard", FY_NEW Shader(vs, fs, "Standard"));
	}

	void AssetManager::AddFlatColorShader()
	{
		std::string vs = R"(
			#version 440 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main()
			{
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
			}
		)";
		std::string fs = R"(
			#version 440 core

			layout(location = 0) out vec4 color;
			
			uniform vec4 u_Color;

			void main()
			{
				color = u_Color;
			}
		)";

		s_Shaders.emplace("FlatColor", FY_NEW Shader(vs, fs, "FlatColor"));
	}
	
	void AssetManager::AddTextureShader()
	{
		std::string vs = R"(
			#version 440 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TextureCoords;
			layout(location = 2) in vec3 a_Normal;
			layout(location = 3) in vec3 a_Tangent;
			layout(location = 4) in vec3 a_Bitangent;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TextureCoords;

			void main()
			{
				v_TextureCoords = a_TextureCoords;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
			}
		)";
		std::string fs = R"(
			#version 440 core

			layout(location = 0) out vec4 color;
			
			uniform sampler2D u_DiffuseTexture;
			uniform sampler2D u_GlossTexture;
			uniform sampler2D u_NormalTexture;

			in vec2 v_TextureCoords;

			void main()
			{
				vec4 diffuse = texture(u_DiffuseTexture, v_TextureCoords);
				vec4 gloss = texture(u_GlossTexture, v_TextureCoords);
				vec4 normal = texture(u_NormalTexture, v_TextureCoords);
				color = vec4(diffuse + gloss + normal);
			}
		)";

		s_Shaders.emplace("Texture2D", FY_NEW Shader(vs, fs, "Texture2D"));
	}
	
	void AssetManager::LoadTexture2D(const std::string& name, const std::string& filepath)
	{
		s_Textures2D.emplace(name, FY_NEW Texture2D(name, filepath));
	}
}