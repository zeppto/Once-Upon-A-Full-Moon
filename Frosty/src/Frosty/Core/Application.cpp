#include "fypch.hpp"
#include "Application.hpp"
#include "Frosty/RenderEngine/Renderer.hpp"
#include <glad/glad.h>

namespace Frosty
{
	Application* Application::s_Instance = nullptr;
	
	Application::Application()		
	{
		Log::Init();
		FY_CORE_INFO("Logger initialized..");

		// TODO: Error handling?
		s_Instance = this;

		m_Window = std::make_unique<Window>(Window());

		EventBus::GetEventBus()->Subscribe<Application, BaseEvent>(this, &Application::OnEvent);

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		ECS::ComponentManager<ECS::CTransform> cManager;
		
		//InitPrefabBuffers();

		LoadModel("newClock");
		CreateBuffers("newClock");

		//LoadModel("testingCube");
		//CreateBuffers("testingCube");
		
		InitShaders();
		
		m_Camera.reset(FY_NEW Camera());

		// <<< FORWARD PLUS >>>
		
		FrustumGrid grid;

		// 4) send the three buffers to a frgament shader

		// 5) find out which cell the pixel belongs to (in screen space)

		// 6) calculate lights as usual (world space)
	}

	Application::~Application()
	{		
		EventBus::GetEventBus()->Delete();
		glfwTerminate();
		Assetmanager::Delete();
		Renderer::DeleteSceneData();		
	}
	
	void Application::InitPrefabBuffers()
	{		
		m_VertexArray.reset(VertexArray::Create());

		auto tempAssetsManager = Assetmanager::GetAssetmanager();
		MotherLoader::GetMotherLoader()->LoadFiles();
		MotherLoader::GetMotherLoader()->PrintLoadingAttemptInformation();
		
		AssetMetaData<ModelTemplate>* metaModel = tempAssetsManager->GetModeltemplateMetaData("clock");
		std::shared_ptr<ModelTemplate> model = metaModel->GetData();
		
		struct TempVertex
		{
			glm::vec3 Position;
			glm::vec2 Texture;
			glm::vec3 Normal;

			TempVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 norm) : Position(pos), Texture(uv), Normal(norm) { }
		};

		std::vector<TempVertex> vertices;
		std::vector<uint32_t> indices;
		unsigned int indexCounter = 0;
		int index = -1;

		float pos[3];
		float uv[2];
		float norm[3];
		
		for (size_t i = 0; i < tempAssetsManager->GetModeltemplateMetaData("tempPlayer")->GetData()->GetMeshInfoMap()->at(0).MeshVertices.size(); i++)
		{
			index = -1;
			pos[0] = tempAssetsManager->GetModeltemplateMetaData("tempPlayer")->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).position[0];
			pos[1] = tempAssetsManager->GetModeltemplateMetaData("tempPlayer")->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).position[1];
			pos[2] = tempAssetsManager->GetModeltemplateMetaData("tempPlayer")->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).position[2];

			uv[0] = tempAssetsManager->GetModeltemplateMetaData("tempPlayer")->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).uv[0];
			uv[1] = tempAssetsManager->GetModeltemplateMetaData("tempPlayer")->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).uv[1];

			norm[0] = tempAssetsManager->GetModeltemplateMetaData("tempPlayer")->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).normal[0];
			norm[1] = tempAssetsManager->GetModeltemplateMetaData("tempPlayer")->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).normal[1];
			norm[2] = tempAssetsManager->GetModeltemplateMetaData("tempPlayer")->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).normal[2];


			std::vector<Luna::Vertex> ff = tempAssetsManager->GetModeltemplateMetaData("tempPlayer")->GetData()->GetMeshInfoMap()->at(0).MeshVertices;

			for (size_t j = 0; j < vertices.size() && index != -1; j++)
			{
				if (vertices.at(j).Position == glm::vec3(pos[0], pos[1], pos[2]) &&
					vertices.at(j).Texture == glm::vec2(uv[0], uv[1]) &&
					vertices.at(j).Normal == glm::vec3(norm[0], norm[1], norm[2]))
				{
					index = j;
				}
			}

			if (index == -1)
			{
				indices.emplace_back(indexCounter++);
				vertices.emplace_back(TempVertex({ pos[0], pos[1], pos[2] }, { uv[0], uv[1] }, { norm[0], norm[1], norm[2] }));
			}
			else
			{
				indices.emplace_back(index);
			}
		}


		//float vertices[8 * 7] = {
		//	-0.5f, -0.5f,  0.5f, 0.8f, 0.0f, 0.8f, 1.0f,
		//	 0.5f, -0.5f,  0.5f, 0.2f, 0.3f, 0.8f, 1.0f,
		//	 0.5f,  0.5f,  0.5f, 0.8f, 0.8f, 0.2f, 1.0f,
		//	-0.5f,  0.5f,  0.5f, 0.8f, 0.8f, 0.2f, 1.0f,
		//	-0.5f, -0.5f, -0.5f, 0.8f, 0.8f, 0.2f, 1.0f,
		//	 0.5f, -0.5f, -0.5f, 0.8f, 0.8f, 0.2f, 1.0f,
		//	 0.5f,  0.5f, -0.5f, 0.8f, 0.8f, 0.2f, 1.0f,
		//	-0.5f,  0.5f, -0.5f, 0.8f, 0.8f, 0.2f, 1.0f
		//};

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(VertexBuffer::Create(&vertices.front(), sizeof(TempVertex) * (uint64_t)vertices.size()));

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" }
		};
		
		m_VertexBuffer->SetLayout(layout);		
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		//uint32_t indices[12 * 3] = {
		//	0, 1, 2,
		//	0, 2, 3,
		//
		//	3, 2, 4,
		//	3, 4, 5,
		//
		//	5, 4, 6,
		//	5, 6, 7,
		//
		//	7, 6, 1,
		//	7, 1, 0,
		//
		//	1, 6, 4,
		//	1, 4, 2,
		//
		//	7, 0, 3,
		//	7, 3, 5
		//};

		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(IndexBuffer::Create(&indices.front(), indices.size()));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Application::LoadModel(const std::string filename)
	{
		auto tempAssetsManager = Assetmanager::GetAssetmanager();
		MotherLoader::GetMotherLoader()->LoadFiles();
		MotherLoader::GetMotherLoader()->PrintLoadingAttemptInformation();

		AssetMetaData<ModelTemplate>* metaModel = tempAssetsManager->GetModeltemplateMetaData(filename);
		std::shared_ptr<ModelTemplate> model = metaModel->GetData();
	}

	void Application::CreateBuffers(const std::string filename)
	{
		m_VertexArray.reset(VertexArray::Create());

		auto tempAssetsManager = Assetmanager::GetAssetmanager();
		struct TempVertex
		{
			glm::vec3 Position;
			glm::vec2 Texture;
			glm::vec3 Normal;

			TempVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 norm) : Position(pos), Texture(uv), Normal(norm) { }
		};

		std::vector<TempVertex> vertices;
		std::vector<uint32_t> indices;
		unsigned int indexCounter = 0;
		int index = -1;

		float pos[3];
		float uv[2];
		float norm[3];

		for (size_t i = 0; i < tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.size(); i++)
		{
			index = -1;
			pos[0] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).position[0];
			pos[1] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).position[1];
			pos[2] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).position[2];

			uv[1] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).uv[1];
			uv[0] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).uv[0];

			norm[0] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).normal[0];
			norm[1] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).normal[1];
			norm[2] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).normal[2];

			for (size_t j = 0; j < vertices.size() && index != -1; j++)
			{
				if (vertices.at(j).Position == glm::vec3(pos[0], pos[1], pos[2]) &&
					vertices.at(j).Texture == glm::vec2(uv[0], uv[1]) &&
					vertices.at(j).Normal == glm::vec3(norm[0], norm[1], norm[2]))
				{
					index = j;
				}
			}

			if (index == -1)
			{
				indices.emplace_back(indexCounter++);
				vertices.emplace_back(TempVertex({ pos[0], pos[1], pos[2] }, { uv[0], uv[1] }, { norm[0], norm[1], norm[2] }));
			}
			else
			{
				indices.emplace_back(index);
			}
		}

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(VertexBuffer::Create(&vertices.front(), sizeof(TempVertex) * (uint64_t)vertices.size()));

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" }
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);		

		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(IndexBuffer::Create(&indices.front(), indices.size()));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Application::InitShaders()
	{
		/*std::string VertexSrc = R"(
			#version 440 core
			
			layout(location = 0) in vec3 vsInPos;
			layout(location = 1) in vec4 vsInCol;
			
			out vec3 vsOutPos;
			out vec4 vsOutCol;
			
			void main()
			{
				gl_Position = vec4(vsInPos, 1.0f);
				vsOutPos = vsInPos;
				vsOutCol = vsInCol;
			}
		)";
		std::string FragmentSrc = R"(
			#version 440 core

			in vec3 vsOutPos;
			in vec4 vsOutCol;

			layout(location = 0) out vec4 fsOutCol;
			
			void main()
			{
				//fsOutCol = vec4(0.8f, 0.2f, 0.3f, 1.0f);
				//fsOutCol = vec4(vsOutPos + 0.5f, 1.0f);				
				fsOutCol = vsOutCol;
			}
		)";*/

		//m_Shader.reset(new Shader(VertexSrc, FragmentSrc));
		//----------------------------------------------------

		std::string VertexSrc2 = R"(
			#version 440 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			layout(location = 2) in vec3 a_Normal;
			
			uniform mat4 u_ViewProjection;

			out vec3 v_Normal;
			out vec2 v_UV;
			
			void main()
			{
				v_Normal = a_Normal;
				v_UV = a_TexCoord;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
			}
		)";
		std::string FragmentSrc2 = R"(
			#version 440 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Normal;
			in vec2 v_UV;

			void main()
			{				
				//color = vec4(1.0, 1.0, 0.0f, 1.0f);
				//color = vec4(v_UV, 0.0f, 1.0f);
				color = vec4(v_Normal, 1.0f);
			}
		)";

		m_Shader.reset(FY_NEW Shader(VertexSrc2, FragmentSrc2));		
	}

	void Application::Run()
	{		
		//Renderer::ShaderInit(m_Shader);
		while (m_Running)
		{
			/// Frame Start
			Time::OnUpdate();
			/// Input
			
			/// Update
			for (Layer* layer : m_LayerHandler)
			{
				layer->OnUpdate();
			}
			
			/// Render
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RenderCommand::Clear();

			Renderer::BeginScene(m_Camera);			
			Renderer::Submit(m_Shader, m_VertexArray);
			Renderer::EndScene();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerHandler)
			{
				if (layer->IsActive())
				{
					layer->OnImGuiRender();
				}
			}
			m_ImGuiLayer->End();			

			m_Window->OnUpdate();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerHandler.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerHandler.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::PopLayer(Layer * layer)
	{
		if (layer != nullptr)
		{
			layer->OnDetach();
			m_LayerHandler.PopLayer(layer);
		}
		delete layer;
	}

	void Application::PopOverlay(Layer * layer)
	{
		if (layer != nullptr)
		{
			layer->OnDetach();
			m_LayerHandler.PopOverlay(layer);
		}
		delete layer;
	}

	void Application::OnEvent(BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::WindowClose:
			OnWindowCloseEvent(static_cast<WindowCloseEvent&>(e));
			break;
		case Frosty::EventType::KeyPressed:
			OnKeyPressedEvent(static_cast<KeyPressedEvent&>(e));
			break;
		default:
			break;
		}

		m_Window->OnEvent(e);

		// Goes through all our layers and calls the OnEvent function in all layers
		// We iterate from the back when handling events
		for (auto it = m_LayerHandler.end(); it != m_LayerHandler.begin(); )
		{
			// We need to break the loop when an event has been handled so we don't handle all layer events
			if ((*--it)->OnEvent(e))
			{
				break;
			}
		}
	}

	void Application::OnWindowCloseEvent(WindowCloseEvent& e)
	{
		FY_CORE_TRACE("Stopping application.");
		m_Running = false;
	}

	void Application::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == GLFW_KEY_ESCAPE)
		{
			m_Running = false;
		}
	}
}
