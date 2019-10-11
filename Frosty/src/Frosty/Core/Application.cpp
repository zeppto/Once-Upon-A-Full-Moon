#include "fypch.hpp"
#include "Application.hpp"
#include "Frosty/RenderEngine/Renderer.hpp"
#include "Frosty/API/PrefabManager/PrefabManager.h"
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

		m_Window = std::make_unique<Window>(Window());

		EventBus::GetEventBus()->Subscribe<Application, BaseEvent>(this, &Application::OnEvent);

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		ECS::ComponentManager<ECS::CTransform> cManager;	
		
		

		//LoadModel("newClock");
		//LoadModel("testingCube");
		MotherLoader::GetMotherLoader()->LoadFiles();
		//PrefabManager::GetPrefabManager()->setPrefab("TestPrefab1", "newClock", "Mat_0:newClock");
		//PrefabManager::GetPrefabManager()->setPrefab("TestPrefab1", "testingCube", "Mat_0:testingCube");
		PrefabManager::GetPrefabManager()->setPrefab("TestPrefab1", "table", "Mat_0:testingCube");

		//LoadModel("testingCube");
		//PrefabManager::GetPrefabManager()->setPrefab("TestPrefab1", "testingCube", "Mat_0:testingCube");
		
		
		//InitShaders();
		
		m_Camera.reset(FY_NEW Camera());

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
	
	void Application::LoadModel(const std::string filename)
	{
		auto tempAssetsManager = Assetmanager::GetAssetmanager();
		MotherLoader::GetMotherLoader()->LoadFiles();
		MotherLoader::GetMotherLoader()->PrintLoadingAttemptInformation();

		AssetMetaData<ModelTemplate>* metaModel = tempAssetsManager->GetModeltemplateMetaData(filename);
		std::shared_ptr<ModelTemplate> model = metaModel->GetData();
	}	
	
	void Application::InitShaders()
	{
		/*std::string VertexSrc = R"(
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
		)";*/

		//m_Shader.reset(new Shader(VertexSrc, FragmentSrc));
		//----------------------------------------------------

		/*std::string VertexSrc2 = R"(
			#version 440 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			layout(location = 2) in vec3 a_Normal;
			
			//uniform mat4 u_ViewProjection;

			out vec3 v_Normal;
			out vec2 v_UV;
			
			void main()
			{
				v_Normal = a_Normal;
				v_UV = a_TexCoord;
				gl_Position = vec4(a_Position, 1.0f);
				//gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
			}
		)";
		std::string FragmentSrc2 = R"(
			#version 440 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Normal;
			in vec2 v_UV;

			void main()
			{				
				color = vec4(1.0, 1.0, 0.0f, 1.0f);
				//color = vec4(v_UV, 0.0f, 1.0f);
				//color = vec4(v_Normal, 1.0f);
			}
		)";*/

		//m_Shader.reset(FY_NEW Shader(VertexSrc2, FragmentSrc2));		
	}

	void Application::Run()
	{		
		Renderer::InitScene(m_Shader);

		//auto tempPrefabManager = PrefabManager::GetPrefabManager();
		//Prefab* tempPrefab = tempPrefabManager->GetPrefab("TestPrefab1");

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
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });		
			//RenderCommand::SetClearColor({ 0.0f, 0.2f, 0.39f, 1.0f });		
			RenderCommand::Clear();

			Renderer::BeginScene(m_Camera);
			//Renderer::Submit(m_Shader, tempVertexArray);
			SubmitPrefab("TestPrefab1");			
			//RenderCommand::DrawIndexed(tempPrefab->GetModelKey().GetKeyData().GetVertexArray(0));
			//Renderer::Submit(m_Shader, tempPrefab->GetModelKey().GetKeyData().GetVertexArray(0));

			//Renderer::Submit(m_Shader, m_VertexArray);
			//Renderer::Submit(m_Shader, tempPrefab->GetModelKey().GetKeyData().GetVertexArray(0));
			
			//tempPrefab->GetModelKey().GetKeyData().GetVertexArray(0)->Bind();
			//RenderCommand::DrawIndexed(tempPrefab->GetModelKey().GetKeyData().GetVertexArray(0));
			//SubmitPrefab("TestPrefab1");
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

	void Application::SubmitPrefab(std::string prefabName)
	{
		auto tempPrefabManager = PrefabManager::GetPrefabManager();
		Prefab* tempPrefab = tempPrefabManager->GetPrefab(prefabName);
		
		tempPrefab->GetModelKey().GetKeyData().GetVertexArray(0)->Bind();
		Renderer::Submit(m_Shader, tempPrefab->GetModelKey().GetKeyData().GetVertexArray(0));

		std::shared_ptr<VertexArray> v = tempPrefab->GetModelKey().GetKeyData().GetVertexArray(0);
		//I want a texture! >:C

		/*RenderModel
		(
			tempPrefab->GetModelKey().GetKeyData().GetVBO(0),
			tempPrefab->GetModelKey().GetKeyData().GetMeshConst(0).vertexCount,
			m_Transform.getModel(), //temp
			tempPrefab->GetMaterialKey().GetKeyData().Diffuse_Texture_MetaData_Ptr->GetData()->GetBufferID()

		);*/
	}
}
