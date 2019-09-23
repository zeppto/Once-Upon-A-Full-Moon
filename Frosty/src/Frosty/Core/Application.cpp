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
		//m_RenderEngine = new RenderEngine();

		/// New ...
		//-------------------------------------------------------------------------

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);		

		float vertices[3 * 3] = 
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));		

		BufferLayout layout = 
		{			
			//{ShaderDataType::Float3, "vsInPos" }			
		};
		
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);		
		
		uint32_t indices[3] = { 0, 1, 2 };
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		std::string VertexSrc = R"(
			#version 440 core
			
			layout(location = 0) in vec3 vsInPos;
			
			out vec3 vsOutPos;
			
			void main()
			{
				gl_Position = vec4(vsInPos, 1.0f);
				vsOutPos = vsInPos;
			}
		)";
		std::string FragmentSrc = R"(
			#version 440 core

			in vec3 vsOutPos;			

			layout(location = 0) out vec4 fsOutCol;
			
			void main()
			{
				//fsOutCol = vec4(0.8f, 0.2f, 0.3f, 1.0f);
				fsOutCol = vec4(vsOutPos, 1.0f);				
			}
		)";

		m_Shader.reset(new Shader(VertexSrc, FragmentSrc));
	}

	Application::~Application()
	{
		//delete m_RenderEngine;
		EventBus::GetEventBus()->Delete();
		glfwTerminate();
	}

	void Application::Run()
	{
		while (m_Running)
		{			
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Shader->Bind();

			glBindVertexArray(m_VertexArray);
			//glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetSize(), GL_UNSIGNED_INT, nullptr);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

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
