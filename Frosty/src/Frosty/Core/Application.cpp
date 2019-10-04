#include "fypch.hpp"
#include "Application.hpp"
#include "Frosty/RenderEngine/RenderEngine.hpp"

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
		
		m_RenderEngine = new RenderEngine();

		// <<< FORWARD PLUS >>>
		
		//FrustumGrid grid;

		// 4) send the three buffers to a frgament shader

		// 5) find out which cell the pixel belongs to (in screen space)

		// 6) calculate lights as usual (world space)
	}

	Application::~Application()
	{		
		EventBus::GetEventBus()->Delete();
		glfwTerminate();
		Assetmanager::Delete();	
		delete m_RenderEngine;
	}

	void Application::Run()
	{		
		while (m_Running)
		{
			/// Frame Start
			m_RenderEngine->ClearColor();
			Time::OnUpdate();
			/// Input

			/// Update
			for (Layer* layer : m_LayerHandler)
			{
				layer->OnUpdate();
			}
			
			/// Render
			m_RenderEngine->Render();
			m_RenderEngine->UpdateCamera();

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
