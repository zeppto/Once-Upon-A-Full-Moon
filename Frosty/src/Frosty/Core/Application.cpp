
#include "fypch.hpp"
#include "Application.hpp"
#include <glad/glad.h>
#include "Frosty/RenderEngine/Renderer.hpp"

#include "Frosty/UI/UIText.h"

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

		//Here

		m_Sprite = FY_NEW Sprite();

		ECS::ComponentManager<ECS::CTransform> cManager;
		InitPrefabBuffers();
		InitShaders();
		//if (!loadShaderSource()) {
		//	FY_CORE_ERROR("Could not load shader source.");
		//}
		MotherLoader::GetMotherLoader()->LoadFiles();
	}

	Application::~Application()
	{
		//delete m_RenderEngine;
		//SAFE_DELETE(m_Sprite);
		EventBus::GetEventBus()->Delete();
		glfwTerminate();
		Assetmanager::Delete();
	}
		

	void Application::InitPrefabBuffers()
	{
		m_VertexArray.reset(VertexArray::Create());
		
		//Sprite test
		m_Sprite->setColor(glm::vec4(0.2, 0.2, 0.2, 1.0));
		m_Sprite->GetTransform().setScale(glm::vec3(0.2, 0.2, 0.2));
		m_Sprite->GetTransform().setTranslate(glm::vec3(0.7, -0.7, 0.0));
		m_Sprite->Init();
		//

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(VertexBuffer::Create(m_Sprite->GetQuad(), m_Sprite->GetSize()));

		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "vsInPos" },
			{ ShaderDataType::Float4, "vsInCol" },
			{ ShaderDataType::Float2, "vsInUV" }
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[6] = { 0, 1, 2, 3, 4, 5};
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Application::InitShaders()
	{
		//{
		//	std::string VertexSrc = R"(
		//	#version 440 core
		//	
		//	layout(location = 0) in vec3 vsInPos;
		//	layout(location = 1) in vec4 vsInCol;
		//	
		//	out vec3 vsOutPos;
		//	out vec4 vsOutCol;
		//	
		//	void main()
		//	{
		//		gl_Position = vec4(vsInPos, 1.0f);
		//		vsOutPos = vsInPos;
		//		vsOutCol = vsInCol;
		//	}
		//)";
		//	std::string FragmentSrc = R"(
		//	#version 440 core

		//	in vec3 vsOutPos;
		//	in vec4 vsOutCol;

		//	layout(location = 0) out vec4 fsOutCol;
		//	
		//	void main()
		//	{
		//		//fsOutCol = vec4(0.8f, 0.2f, 0.3f, 1.0f);
		//		//fsOutCol = vec4(vsOutPos + 0.5f, 1.0f);				
		//		fsOutCol = vsOutCol;
		//	}
		//)";
		//	//m_Shader.reset(new Shader(VertexSrc, FragmentSrc));
		//}
		//
		//std::string VertexSrc2 = R"(
		//	#version 440 core
		//	
		//	layout(location = 0) in vec3 vsInPos;
		//	layout(location = 1) in vec4 vsInCol;
		//	layout(location = 2) in vec2 vsInUV;			
		//	
		//	uniform mat4 model;

		//	out vec3 vsOutPos;
		//	out vec4 vsOutCol;
		//	out vec2 vsOutUV;
		//	
		//	void main()
		//	{

		//		gl_Position = model * vec4(vsInPos, 1.0f);
		//		vsOutPos = vsInPos;
		//		vsOutCol = vsInCol;
		//		vsOutUV = vsInUV;
		//	}
		//)";
		//std::string FragmentSrc2 = R"(
		//	#version 440 core

		//	in vec3 vsOutPos;
		//	in vec4 vsOutCol;
		//	in vec2 vsOutUV;			

		//	uniform sampler2D sprite_Texture;

		//	layout(location = 0) out vec4 fsOutCol;
		//	
		//	void main()
		//	{
		//		//fsOutCol = vec4(0.8f, 0.2f, 0.3f, 1.0f);
		//		//fsOutCol = vec4(vsOutPos + 0.5f, 1.0f);
		//		vec3 tex = texture(sprite_Texture, vsOutUV).rgb;				
		//		fsOutCol = vec4(tex, 1);
		//	}
		//)";
		m_Shader.reset(new Shader((std::string(FY_SHADERS_FOLDER_ROOT) + "SpriteVertexShader.glsl"), ((std::string(FY_SHADERS_FOLDER_ROOT) + "SpriteFragmentShader.glsl"))));
		
	}

	bool Application::loadShaderSource(std::string src)
	{

		return false;
	}

	void Application::Run()
	{

		states.AddState(Frosty::StateRef(new MainMenuState(s_Instance)), false);
		states.ProcessStateChanges();


		while (m_Running)
		{

			states.GetActiveState()->OnUpdate();
			if (mainMenuReturn == true)
			{
				states.AddState(Frosty::StateRef(new MainMenuState(s_Instance)), true);
				mainMenuReturn = false;
			}
			states.ProcessStateChanges();

			/// Frame Start
			Time::OnUpdate();

			/// Input			
			
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RenderCommand::Clear();

			Renderer::BeginScene();
			m_Shader->Bind();
			Renderer::Submit(m_VertexArray);
			Renderer::EndScene();

			/// Input

			//TEST SPRITE
			m_Shader->UploadUniformInt(m_Sprite->GetTexure().name, 0);
			glActiveTexture(GL_TEXTURE);
			glBindTexture(GL_TEXTURE_2D, m_Sprite->GetTexure().id);
			m_Shader->UploadUniforMat4("model", m_Sprite->GetTransform().getModel());

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

		//if (e.GetKeyCode() == GLFW_KEY_O)
		//{
		//	std::cout << "Options was clicked!!" << std::endl;
		//}
		if (e.GetKeyCode() == GLFW_KEY_M)
		{
			states.GetActiveState()->OnInput();
		}
	}
}

