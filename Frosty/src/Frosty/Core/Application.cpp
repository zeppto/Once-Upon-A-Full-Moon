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

		// TODO: Error handling?
		s_Instance = this;

		m_Window = std::make_unique<Window>(Window());

		EventBus::GetEventBus()->Subscribe<Application, BaseEvent>(this, &Application::OnEvent);

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		ECS::ComponentManager<ECS::CTransform> cManager;

		InitPrefabBuffers();

		MotherLoader::GetMotherLoader()->LoadFiles();
		MotherLoader::GetMotherLoader()->PrintLoadingAttemptInformation();

		InitShaders();
	}

	Application::~Application()
	{
		//delete m_RenderEngine;
		EventBus::GetEventBus()->Delete();
		glfwTerminate();
		Assetmanager::Delete();
	}
		
	void Application::InitPrefabBuffers()
	{
		/*m_VertexArray.reset(VertexArray::Create());

		float vertices[3 * 8] =
		{
					-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
					 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
					 0.0f,  0.5f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 1.0f
		};

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(VertexBuffer::Create(vertices, 3*8));

		BufferLayout layout =
		{
				{ ShaderDataType::Float3, "vsInPos" },
				{ ShaderDataType::Float2, "vsInUV" },
				{ ShaderDataType::Float3, "vsInNormal" }
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(IndexBuffer::Create(indices, 3));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		auto tempManager = Assetmanager::GetAssetmanager();

		m_VertexArray->Unbind();*/

		/*AssetMetaData<ModelTemplate>* metaModel = tempManager->GetModeltemplateMetaData("clock");
		std::shared_ptr<ModelTemplate> model = metaModel->GetData();


		AssetMetaData<TextureFile>* metaTexture = tempManager->GetTextureMetaData("pCube10_diffuse");*/


	}

	void Application::InitShaders()
	{
		std::string VertexSrc = R"(
			#version 440 core
			
			layout(location = 0) in vec3 vsInPos;
			layout(location = 1) in vec2 vsInUV;
			layout(location = 2) in vec3 vsInNormal;
			
			out vec3 vsOutPos;
			out vec3 vsOutNormal;
			
			void main()
			{
				gl_Position = vec4(vsInPos, 1.0f);
				vsOutPos = vsInPos;
				vsOutNormal = vsInNormal;
			}
		)";
		std::string FragmentSrc = R"(
			#version 440 core

			in vec3 vsOutPos;
			in vec3 vsOutNormal;

			layout(location = 0) out vec4 fsOutCol;
			
			void main()
			{
				//fsOutCol = vec4(0.0f, 1.0f, 1.0f, 1.0f);
				//fsOutCol = vec4(vsOutPos + 0.5f, 1.0f);				
				fsOutCol = vec4(vsOutNormal, 1.0f);
			}
		)";

		m_Shader.reset(new Shader(VertexSrc, FragmentSrc));
	}

	void Application::Run()
	{
		while (m_Running)
		{
			/// Frame Start
			Time::OnUpdate();

			/// Input			
			
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RenderCommand::Clear();

			Renderer::BeginScene();

			m_Shader->Bind();
			//Renderer::Submit(m_VertexArray);
			Renderer::Submit(Assetmanager::GetAssetmanager()->GetModeltemplateMetaData("clock")->GetData()->GetVertexArray(0));

		
			Renderer::EndScene();

			/// Input

			/// Update
			for (Layer* layer : m_LayerHandler)
			{
				layer->OnUpdate();
			}

			//m_RenderEngine->UpdateCamera();
			/// Render
			//m_RenderEngine->Render();

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
