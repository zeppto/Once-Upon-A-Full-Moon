#include "fypch.hpp"
#include "Application.hpp"

#include <glad/glad.h>

namespace Frosty
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		// TODO: Error handling?
		s_Instance = this;

		Log::Init();
		FY_CORE_INFO("Logger initialized..");

		m_Window = std::make_unique<Window>(Window());

		EventBus::GetEventBus()->Subscribe<Application, BaseEvent>(this, &Application::OnEvent)     ;

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
		m_RenderEngine = new RenderEngine();
	}

	Application::~Application()
	{
		delete m_RenderEngine;
		EventBus::GetEventBus()->Delete();
		glfwTerminate();
	}

	void Application::Run()
	{

		states.AddState(Frosty::StateRef(new MainMenuState(s_Instance)), false);
		states.ProcessStateChanges();


		while (m_Running)
		{
			states.GetActiveState()->OnUpdate();
			states.ProcessStateChanges();
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

		//if (e.GetKeyCode() == GLFW_KEY_O)
		//{
		//	std::cout << "Options was clicked!!" << std::endl;
		//}
	}

}
