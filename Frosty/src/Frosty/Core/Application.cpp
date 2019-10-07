#include "fypch.hpp"
#include "Application.hpp"
#include <glad/glad.h>
#include "Frosty/RenderEngine/Renderer.hpp"



namespace Frosty
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		Log::Init();
		FY_CORE_INFO("Logger initialized..");

		FY_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		EventBus::GetEventBus()->Subscribe<Application, BaseEvent>(this, &Application::OnEvent);

		m_Window.reset(FY_NEW Window());
		m_Window->Init();

		m_ImGuiLayer = FY_NEW ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_Scene.reset(FY_NEW Scene());
		m_Scene->Init();

		InitiateQuadMesh();
		InitiateTriangleMesh();
		InitShaders();
	}

	Application::~Application()
	{
		EventBus::GetEventBus()->Delete();
		m_Window->Shutdown();
		Renderer::Shutdown();
		Assetmanager::Delete();
	}

	void Application::InitiateQuadMesh()
	{
		m_Quad.reset(VertexArray::Create());

		float vertices[4 * 3] =
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "a_Position" }
		};

		m_VertexBuffer->SetLayout(layout);
		m_Quad->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[2 * 3] = {
			0, 1, 2,
			2, 3, 0
		};
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_Quad->SetIndexBuffer(m_IndexBuffer);
	}

	void Application::InitiateTriangleMesh()
	{
		m_Triangle.reset(VertexArray::Create());

		float vertices[3 * 3] =
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f,
		};

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "a_Position" }
		};

		m_VertexBuffer->SetLayout(layout);
		m_Triangle->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[1 * 3] = {
			0, 1, 2
		};
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_Triangle->SetIndexBuffer(m_IndexBuffer);
	}

	void Application::InitShaders()
	{
		std::string flatColorVS = R"(
			#version 440 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main()
			{
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
			}
		)";
		std::string flatColorFS = R"(
			#version 440 core

			layout(location = 0) out vec4 color;
			
			uniform vec4 u_Color;

			void main()
			{
				color = u_Color;
			}
		)";

		m_ShaderAssets.reset(FY_NEW Shader(flatColorVS, flatColorFS));

		//m_Texture.reset(FY_NEW Texture2D("assets/textures/Checkerboard.png"));

		//m_Shader->Bind();
		//m_Shader->UploadUniformInt("u_Texture", 0);
	}

	void Application::Run()
	{
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
			//RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
			//RenderCommand::Clear();

			//Renderer::BeginScene(m_Camera);

			m_Scene->Render();

			//m_Texture->Bind();
			//m_Shader->Bind();
			//Renderer::Submit(m_VertexArray);

			//Renderer::EndScene();

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

	std::unique_ptr<Scene>& Application::CreateScene()
	{
		m_Scene.reset(FY_NEW Scene());

		return m_Scene;
	}

	void Application::DestroyScene()
	{
		if (!m_Shader)
		{
			m_Scene.reset();;
		}
	}

	std::unique_ptr<Scene>& Application::GetScene()
	{
		FY_CORE_ASSERT(m_Scene, "Scene needs to be created before it get retrieved.");
		return m_Scene;
	}

	const std::unique_ptr<Scene>& Application::GetScene() const
	{
		FY_CORE_ASSERT(m_Scene, "Scene needs to be created before it get retrieved.");
		return m_Scene;
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
