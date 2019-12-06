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
		m_VertexArray.reset(VertexArray::Create());

		float vertices[4 * 7] =
		{
			-0.5f, -0.5f, 0.0f, 0.8f, 0.0f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
			 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
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

		uint32_t indices[2 * 3] = { 
			0, 1, 2,
			2, 1, 3
		};
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Application::InitShaders()
	{
		std::string VertexSrc = R"(
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
		)";

		m_Shader.reset(new Shader(VertexSrc, FragmentSrc));
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float ramMemory = RamUsage();
			FY_CORE_FATAL("1: {0} MiB RAM committed.", ramMemory);

			/// Frame Start
			Time::OnUpdate();

			/// Input			
			
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RenderCommand::Clear();

			Renderer::BeginScene();

			m_Shader->Bind();
			Renderer::Submit(m_VertexArray);

			Renderer::EndScene();

			ramMemory = RamUsage();
			FY_CORE_FATAL("2: {0} MiB RAM committed.", ramMemory);
			/// Input

			/// Update
			for (Layer* layer : m_LayerHandler)
			{
				layer->OnUpdate();
			}

			ramMemory = RamUsage();
			FY_CORE_FATAL("3: {0} MiB RAM committed.", ramMemory);
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

			ramMemory = RamUsage();
			FY_CORE_FATAL("4: {0} MiB RAM committed.", ramMemory);
			m_Window->OnUpdate();

			ramMemory = RamUsage();
			FY_CORE_FATAL("5: {0} MiB RAM committed.", ramMemory);
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
