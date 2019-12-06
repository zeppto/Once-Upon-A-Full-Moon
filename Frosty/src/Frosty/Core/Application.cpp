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

		EventBus::GetEventBus()->Subscribe<Application, BaseEvent>(this, &Application::OnEvent);

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
		m_RenderEngine = new RenderEngine();

		//test

		//Assetmanager::GetAssetmanager();
		//Assetmanager::GetAssetmanager()->LoadFile("FbxAztec.fbx");
		//tempManager->LoadFile("FbxAztec.fbx");

	}

	Application::~Application()
	{
		delete m_RenderEngine;
		EventBus::GetEventBus()->Delete();
		glfwTerminate();
		Assetmanager::Delete();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float ramMemory = RamUsage();
			FY_CORE_FATAL("1: {0} MiB RAM committed.", ramMemory);

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

			ramMemory = RamUsage();
			FY_CORE_FATAL("2: {0} MiB RAM committed.", ramMemory);
			m_Window->OnUpdate();
			ramMemory = RamUsage();
			FY_CORE_FATAL("3: {0} MiB RAM committed.", ramMemory);
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
	}

	float Application::RamUsage()
	{
		//src: https://docs.microsoft.com/en-us/windows/desktop/api/psapi/ns-psapi-_process_memory_counters

		DWORD currentProcessID = GetCurrentProcessId();

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, currentProcessID);

		if (NULL == hProcess)
			return 0.0f;

		float memoryUsage = 0.0f;
		PROCESS_MEMORY_COUNTERS pmc{};
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
		{
			//PagefileUsage is the:
				//The Commit Charge value in bytes for this process.
				//Commit Charge is the total amount of memory that the memory manager has committed for a running process.

			memoryUsage = float(pmc.PagefileUsage / 1024.0 / 1024.0); //MiB
			memoryUsage = memoryUsage - VramUsage();

			//if (memoryUsage > 256) FY_CORE_FATAL("{0} MiB RAM committed.", memoryUsage);
		}

		CloseHandle(hProcess);

		return memoryUsage;
	}

	float Application::VramUsage()
	{
		IDXGIFactory* dxgifactory = nullptr;
		HRESULT ret_code = ::CreateDXGIFactory(
			__uuidof(IDXGIFactory),
			reinterpret_cast<void**>(&dxgifactory));

		float memoryUsage = 0.0f;
		if (SUCCEEDED(ret_code))
		{
			IDXGIAdapter* dxgiAdapter = nullptr;

			if (SUCCEEDED(dxgifactory->EnumAdapters(0, &dxgiAdapter)))
			{
				IDXGIAdapter4* dxgiAdapter4 = NULL;
				if (SUCCEEDED(dxgiAdapter->QueryInterface(__uuidof(IDXGIAdapter4), (void**)&dxgiAdapter4)))
				{
					DXGI_QUERY_VIDEO_MEMORY_INFO info;

					if (SUCCEEDED(dxgiAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
					{
						memoryUsage = float(info.CurrentUsage / 1024.0 / 1024.0); //MiB

						//if (memoryUsage > 256) FY_CORE_FATAL("{0} MiB VRAM used.", memoryUsage);
					};

					dxgiAdapter4->Release();
				}
				dxgiAdapter->Release();
			}
			dxgifactory->Release();
		}

		return memoryUsage;
	}
}
