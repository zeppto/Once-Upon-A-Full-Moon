#include <fypch.hpp>
#include "Application.hpp"
#include <glad/glad.h>
#include "Frosty/RenderEngine/Renderer.hpp"
#include "Frosty/Core/KeyCodes.h"
#include"Frosty/API/AssetManager/AssetManager.hpp"

//Particle Branch Includes
#include "Frosty/RenderEngine/VertexArray.hpp"
#include "Frosty/UI/UIText.h"

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

		m_Window.reset(BaseWindow::Create());
		m_Window->Init();

		Renderer::Init();

		m_EditorCamera.Init(EditorCameraProps({ 0.0f, 20.0f, 20.0f }, { -90.0f, -50.0f, 0.0f }));

		m_ImGuiLayer = FY_NEW ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_World.reset(FY_NEW World());
		m_World->Init();
	}

	Application::~Application()
	{
		EventBus::GetEventBus()->Delete();
		m_Window->Shutdown();
		Renderer::Shutdown();
		AssetManager::Delete();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			/// Frame Start
			Time::OnUpdate();

			m_EditorCamera.OnUpdate();
			for (Layer* layer : m_LayerHandler)
			{
				layer->OnUpdate();
			}

			if (!m_GameRunning)
			{
				RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
				RenderCommand::Clear();
			
				Renderer::BeginScene();
				Renderer::SetCamera(m_EditorCamera.GetPosition(), m_EditorCamera.GetViewMatrix(), m_EditorCamera.GetProjectionMatrix());
			}
			else
			{
				m_World->OnStart();
				m_World->OnInput();
				m_World->OnUpdate();
				m_World->BeginScene();
			}
			Renderer::RenderScene();

			m_World->Render();

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

	void Application::PopLayer(Layer* layer)
	{
		if (layer != nullptr)
		{
			layer->OnDetach();
			m_LayerHandler.PopLayer(layer);
		}
		delete layer;
	}

	void Application::PopOverlay(Layer* layer)
	{
		if (layer != nullptr)
		{
			layer->OnDetach();
			m_LayerHandler.PopOverlay(layer);
		}
		delete layer;
	}

	void Application::StartGame(bool maximize)
	{
		m_GameRunning = true;
		bool* eCam = m_EditorCamera.ActiveStatus();
		*eCam = false;
		if (maximize) m_Window->ActivateGameMode();
	}

	void Application::StopGame(bool maximize)
	{
		m_GameRunning = false;
		bool* eCam = m_EditorCamera.ActiveStatus();
		*eCam = true;
		if (maximize) m_Window->ActivateEditorMode();
	}

	void Application::OnEvent(BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case EventType::WindowClose:
			OnWindowCloseEvent(static_cast<WindowCloseEvent&>(e));
			break;
		case EventType::KeyPressed:
			OnKeyPressedEvent(static_cast<KeyPressedEvent&>(e));
			break;
		case EventType::KeyReleased:
			OnKeyReleasedEvent(static_cast<KeyReleasedEvent&>(e));
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

		if (m_GameRunning)
		{
			m_World->OnEvent(e);
		}
	}

	void Application::OnWindowCloseEvent(WindowCloseEvent& e)
	{
		FY_CORE_TRACE("Stopping application.");
		m_Running = false;
	}

	void Application::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == FY_KEY_ESCAPE)
		{
			m_Running = false;
		}
		if ((InputManager::IsKeyPressed(FY_KEY_LEFT_CONTROL) || InputManager::IsKeyPressed(FY_KEY_RIGHT_CONTROL)) && e.GetKeyCode() == FY_KEY_P && m_CanPrintInfo)
		{
			m_World->PrintWorld();
			m_CanPrintInfo = false;
		}
	}

	void Application::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		if (e.GetKeyCode() == FY_KEY_P)
		{
			m_CanPrintInfo = true;
		}
	}
}
