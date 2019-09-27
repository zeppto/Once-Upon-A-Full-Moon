#include "fypch.hpp"
#include "Application.hpp"
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

		EventBus::GetEventBus()->Subscribe<Application, BaseEvent>(this, &Application::OnEvent);

		m_Window.reset(FY_NEW Window());
		m_Window->Init();

		m_ImGuiLayer = FY_NEW ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
		m_RenderEngine = FY_NEW RenderEngine();

		//ECS::ComponentManager<ECS::CTransform> cManager;

		//m_ComponentManagers[m_TotalComponentManagers++].reset(new ECS::ComponentManager<ECS::CTransform>);

		//auto entity = m_EntityManager.CreateEntity();
		//cManager.AddComponent(entity);

		//FY_CORE_TRACE("{0}", ECS::ComponentInfo::GetComponentNameById(ECS::getComponentTypeID<ECS::CTransform>()));
	}

	Application::~Application()
	{
		delete m_RenderEngine;
		EventBus::GetEventBus()->Delete();
		m_Window->Shutdown();
		Assetmanager::Delete();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			/// Frame Start
			Time::OnUpdate();

			// TEMPORARY
			glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
			glClear(GL_DEPTH_BITS);

			/// Input			

			/// Update
			for (Layer* layer : m_LayerHandler)
				layer->OnUpdate();

			m_RenderEngine->UpdateCamera();
			/// Render
			m_RenderEngine->Render();
			
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

		//glfwTerminate();
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
