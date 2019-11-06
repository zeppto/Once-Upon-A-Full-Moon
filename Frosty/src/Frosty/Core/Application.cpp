#include "fypch.hpp"
#include "Application.hpp"
#include "Frosty/RenderEngine/Renderer.hpp"
#include "../API/BoolMapGenerator/BoolMapGenerator.hpp"



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
		
		InitPrefabBuffers();
		//InitShaders();
		
		m_Camera.reset(FY_NEW Camera());

		// <<< FORWARD PLUS >>>
		m_LightManager.reset(FY_NEW LightManager());
		m_LightManager->AddPointLight(glm::vec3(0.f, 0.f, 1.f), glm::vec4(1.f, 1.f, 1.f, 1.f), 1.f, 10.f);
		m_LightManager->AddDirectionalLight(glm::vec4(0.2f, 0.3f, 0.7f, 1.f), 0.7f, glm::vec3(-1.f, -1.f, -1.f));


		Renderer::InitForwardPlus(m_LightManager);
		//BoolMapGenerator::Get()->Initiate();
		
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

		//float vertices[3 * 7] =
		//{
		//	-0.5f, -0.5f, 0.0f, 0.8f, 0.0f, 0.8f, 1.0f,
		//	 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		//	 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		//};

		float PlaneX = 192.0f;
		float PlaneY = 108.0f;

		float vertices[6 * 7] =
		{
			0.f, 0.f, 0.f, 1.0f, 0.0f, 0.0f, 1.0f,		//Top Left
			PlaneX, 0.f, 0.f, 0.0f, 0.0f, 1.0f, 1.0f, // Top Rigth
			0.f,  0.f, PlaneY, 0.0f, 1.0f, 0.0f, 1.0f, //Bot Left

			PlaneX, 0.f, PlaneY, 1.0f, 1.0f, 1.0f, 1.0f, //Bot Rigth
			0.f, 0.f, PlaneY, 0.0f, 1.0f, 0.0f, 1.0f,	//Bot Left
			PlaneX,  0.f, 0.f, 0.0f, 0.0f, 1.0f, 1.0f	//Top Rigth
		};

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "vsInPos" },
			{ ShaderDataType::Float4, "vsInCol" }
		};
		
		m_VertexBuffer->SetLayout(layout);		
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[4] = { 0, 1, 2, 3};
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
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

		//----------------------------------------------------------------------
		std::string VertexSrc2 = R"(
			#version 440 core
			
			layout(location = 0) in vec3 vsInPos;
			layout(location = 1) in vec4 vsInCol;
			
			uniform mat4 u_ViewProjection;
			
			out vec3 vsOutPos;
			out vec4 vsOutCol;
			
			void main()
			{
				vsOutPos = vsInPos;
				vsOutCol = vsInCol;
				gl_Position = u_ViewProjection * vec4(vsInPos, 1.0f);
			}
		)";
		std::string FragmentSrc2 = R"(
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
		)";

		//m_Shader.reset(new Shader(VertexSrc2, FragmentSrc2));		
	}

	void Application::Run()
	{
		Renderer::ShaderInit(m_Shader);
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
			RenderCommand::SetClearColor({ 0.0f, 0.0f, 1.0f, 5.0f });
			RenderCommand::Clear();

			Renderer::BeginScene(m_Camera);			
		//	Renderer::BeginScene(CollisionMap::Get()->GetCamera());			

			Renderer::Submit(m_Shader, m_VertexArray, m_LightManager);

			std::vector<float> tempVert;
			tempVert = { 
				-0.5f,  0.0f, 0.5f,
				0.5f,  0.0f, 0.5f,
				0.0f,  0.0f,  0.0f
			};


			glm::mat4 temp(1.0f);

			temp[3][0] = 64.0f;
			temp[3][1] = 1.0f;
			temp[3][2] = 36.0f;

			temp[0][0] *= 30.0f;
			temp[1][1] *= 1.0f;
			temp[2][2] *= 30.0f;



			glm::mat4 tempMat = glm::translate(glm::mat4(1.0f), glm::vec3(64.0f, 0.0f, 36.0f));
			tempMat = glm::scale(tempMat, glm::vec3(30.0f, 1.0f, 300.0f));


			ModelBatch tempBatch;
			tempBatch.Verticies = tempVert;
			tempBatch.Transforms.emplace_back(tempMat);

			BoolMapGenerator::AddToRenderList(tempBatch);

			BoolMapGenerator::RenderBoolMap();

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