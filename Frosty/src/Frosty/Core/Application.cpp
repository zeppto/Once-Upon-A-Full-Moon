#include <fypch.hpp>
#include "Application.hpp"
#include <glad/glad.h>
#include "Frosty/RenderEngine/Renderer.hpp"
#include "Frosty/Core/KeyCodes.h"
#include"Frosty/API/AssetManager/AssetManager.hpp"
#include"Frosty/Core/BoolMapGenerator/BoolMapGenerator.hpp"
#include"Frosty/Core/BoolMapGenerator/TestMap.hpp"


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

		//m_Window.reset(FY_NEW Window());
		m_Window.reset(BaseWindow::Create());
		m_Window->Init();

		Renderer::Init();

		m_EditorCamera.Init(EditorCameraProps({ 0.0f, 20.0f, 20.0f }, { -90.0f, -50.0f, 0.0f }));

		m_ImGuiLayer = FY_NEW ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_World.reset(FY_NEW World());
		m_World->Init();

		//Testing AssetManager
		//Assetmanager::Get()->LoadFiles();
		//TextureFile* l =  Assetmanager::GetTexture("pCube10_diffuse");
		//Animation* g =  Assetmanager::GetAnimation("klocka");

		//From Particle. Re-integrate with new system isntead__________________________________
		/*MotherLoader::GetMotherLoader()->LoadFiles();

		m_Canvas.reset(FY_NEW Canvas);
		m_Sprite.reset(FY_NEW Sprite);
		m_particleSystem.reset(FY_NEW ParticleSystem("Test", DEFAULT_PARTICLE_PATH, 10));

		ECS::ComponentManager<ECS::CTransform> cManager;
		InitPrefabBuffers();
		InitShaders();
		//if (!loadShaderSource()) {
		//	FY_CORE_ERROR("Could not load shader source.");
		//}
		MotherLoader::GetMotherLoader()->LoadFiles();
		*/




		//Assetmanager::Get()->LoadFiles();

		//std::unordered_map<std::string, LinkedMaterial>::iterator it = Assetmanager::GetMaterialMap()->begin();

		//while (it != Assetmanager::GetMaterialMap()->end())
		//{
		//	FY_CORE_INFO("Mat: {0}", it->first);

		//	if (it->second.GetDiffuse() != nullptr)
		//	{
		//	FY_CORE_INFO("Has Dif");
		//	}
		//	if (it->second.GetNormal() != nullptr)
		//	{
		//		FY_CORE_INFO("Has Norm");
		//	}
		//	if (it->second.GetGlow() != nullptr)
		//	{
		//		FY_CORE_INFO("Has Glow");
		//	}
		//	it++;
		//}







	}

	Application::~Application()
	{
		EventBus::GetEventBus()->Delete();
		m_Window->Shutdown();
		Renderer::Shutdown();
		AssetManager::Delete();
		BoolMapGenerator::Delete();
	}

	void Application::InitPrefabBuffers()
	{
		//m_VertexArray.reset(VertexArray::Create());
		//
		////Canvas test
		//m_Canvas->SetScale(glm::vec3(0.5, 0.5, 0.5));
		//m_Canvas->SetPosition(glm::vec3(0.0, 0.0, 0.0));
		//m_Canvas->Init();
		////Sprite test
		////m_Sprite->SetScale(glm::vec3(0.2, 0.2, 0.2));
		////m_Sprite->SetPosition(glm::vec3(0.7, -0.7, 0.0));
		////m_Sprite->Init();
		////

		////Layout
		//BufferLayout layout =
		//{
		//	{ ShaderDataType::Float3, "vsInPos" },
		//	{ ShaderDataType::Float4, "vsInCol" },
		//	{ ShaderDataType::Float2, "vsInUV" }
		//};

		//////Sprite quad
		////std::shared_ptr<VertexBuffer> m_VertexBuffer1;
		////m_VertexBuffer1.reset(VertexBuffer::Create(m_Sprite->vertices, sizeof(m_Sprite->vertices)));
		////m_VertexBuffer1->SetLayout(layout);
		////m_VertexArray->AddVertexBuffer(m_VertexBuffer1); //Add to array

		////Canvas quad
		//std::shared_ptr<VertexBuffer> m_VertexBuffer2;
		//m_VertexBuffer2.reset(VertexBuffer::Create(m_Canvas->m_Vertices, sizeof(m_Canvas->m_Vertices)));
		//m_VertexBuffer2->SetLayout(layout);
		//m_VertexBuffer2->SetNrOfVertices(6);
		//m_VertexArray->AddVertexBuffer(m_VertexBuffer2); //Add to array

		//uint32_t indices[6] = { 0, 1, 2, 3, 4, 5};
		//std::shared_ptr<IndexBuffer> m_IndexBuffer;
		//m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		//m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		////m_VertexArray->Unbind();
		////m_IndexBuffer->Unbind();
		////m_VertexBuffer2->Unbind();

		////Text
		//m_TextVertexArray.reset(VertexArray::Create());
		//m_textVertBuffer.reset(VertexBuffer::Create());
		//BufferLayout textLayout =
		//{
		//	{ ShaderDataType::Float4, "vertex" }
		//};
		//m_textVertBuffer->SetLayout(textLayout);
		//m_TextVertexArray->AddVertexBuffer(m_textVertBuffer);

		//uint32_t textIndices[6] = { 0, 1, 2, 3, 4, 5 };
		//std::shared_ptr<IndexBuffer> textIndexBuffer;
		//textIndexBuffer.reset(IndexBuffer::Create(textIndices, sizeof(textIndices) / sizeof(uint32_t)));
		//m_TextVertexArray->SetIndexBuffer(textIndexBuffer);
	}

	//void Application::InitShaders()
	//{
	//	m_Shader.reset(new Shader((std::string(FY_SHADERS_FOLDER_ROOT) + "SpriteVertexShader.glsl"), ((std::string(FY_SHADERS_FOLDER_ROOT) + "SpriteFragmentShader.glsl"))));
	//	m_textShader.reset(new Shader((std::string(FY_SHADERS_FOLDER_ROOT) + "TextVertexShader.glsl"), ((std::string(FY_SHADERS_FOLDER_ROOT) + "TextFragmentShader.glsl"))));
	//	m_particleShader.reset(new Shader((std::string(FY_SHADERS_FOLDER_ROOT) + "ParticleVertexShader.glsl"), (std::string(FY_SHADERS_FOLDER_ROOT) + "ParticleGeometryShader.glsl"), std::string(FY_SHADERS_FOLDER_ROOT) + "ParticleFragmentShader.glsl"));
	//	m_computeParticleShader.reset(new Shader(std::string(FY_SHADERS_FOLDER_ROOT) + "ParticleComputeShader.glsl"));
	//}

	//void Application::Run()
	//{
	//	states.AddState(Frosty::StateRef(new MainMenuState(s_Instance)), false);
	//	states.ProcessStateChanges();

	//	//m_Canvas.reset(new Canvas);
	//	//m_Sprite.reset(new Sprite);

	//	//ECS::ComponentManager<ECS::CTransform> cManager;
	//	Renderer::DeleteSceneData();
	//}

	void Application::Run()
	{
		/*Renderer::InitScene(m_Shader2);*/
	/*	states.AddState(Frosty::StateRef(new MainMenuState(s_Instance)), false);*/
		/*states.ProcessStateChanges();*/

		while (m_Running)
		{

			/// Frame Start
			Time::OnUpdate();


			//states.GetActiveState()->OnUpdate();
			//if (mainMenuReturn == true)
			//{
			//	states.AddState(Frosty::StateRef(new MainMenuState(s_Instance)), true);
			//	mainMenuReturn = false;
			//}



			////TEST SPRITE
			//m_Shader->UploadUniformInt(m_Sprite->GetTexture().name, 0);
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, m_Canvas->GetTexture());
			//Renderer::Submit2D(m_Shader, m_VertexArray, m_Sprite->GetTexture().name, m_Canvas->GetTransform().GetModel());

			//glBindTexture(GL_TEXTURE_2D, 0);

			////Render particles
			//m_particleShader->Bind();
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			//m_particleSystem->LoadTexture();
			//glBindTexture(GL_TEXTURE_2D, m_particleSystem->GetTextureID());
			//Renderer::SubmitParticles(m_particleShader, m_particleSystem->GetVertexArray(), m_particleSystem->GetModelMatrix(), m_particleSystem->GetParticleCount());
			//m_particleShader->UnBind();
			//glDisable(GL_BLEND);

			////Render text
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//std::string tempText = "Hello team";
			//Renderer::SubmitText(m_textShader, m_TextVertexArray, tempText);
			//glDisable(GL_BLEND);

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
				//Renderer::SetCamera(m_EditorCamera.GetPosition(), m_EditorCamera.GetViewProjectionMatrix());


				//ModelBatch

				//std::vector<float> tempVert = {
				//	-0.5f,  0.0f, -0.5f,
				//	-0.5f,  0.0f,  0.5f,
				//	0.5f,  0.0f,  0.5f,
				//	0.5f,  0.0f,  -0.5f
				//};
				//std::vector<unsigned int> tempIndices = {
				//0,1,2,
				//0,2,3
				//};

				//glm::mat4 temp(1.0f);

				//temp[3][0] = 0.0f;
				//temp[3][1] = 0.0f;
				//temp[3][2] = 72.0f;

				//temp[0][0] = 4.0f;
				//temp[1][1] = 4.0f;
				//temp[2][2] = 4.0f;


				//ModelBatch tempBatch;
				//tempBatch.Verticies = tempVert;
				//tempBatch.Indices = tempIndices;
				//tempBatch.Transforms.emplace_back(temp);

				//BoolMapGenerator::AddBatch(tempBatch);


				////BoundBatch

				//BoundBatch testBound;

				//testBound.VertexArrayID = AssetManager::GetMesh("pSphere1")->GetRenderID();
				//testBound.NrOfIndices = AssetManager::GetMesh("pSphere1")->GetIndexBuffer()->GetCount();
				//glm::mat4 tempMat;

				////Botleft
				//tempMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
				//tempMat = glm::scale(tempMat, glm::vec3(1.0f, 1.0f, 1.0f));
				//testBound.Transforms.emplace_back(tempMat);



				////topRight
				//tempMat = glm::translate(glm::mat4(1.0f), glm::vec3(128.0f, 0.0f, 72.0f));
				//tempMat = glm::scale(tempMat, glm::vec3(1.0f, 1.0f, 1.0f));
				//testBound.Transforms.emplace_back(tempMat);


				////RandomSpot
				//tempMat = glm::translate(glm::mat4(1.0f), glm::vec3(33.0f, 0.0f, 36.0f));
				//tempMat = glm::scale(tempMat, glm::vec3(1.0f, 1.0f, 1.0f));
				//testBound.Transforms.emplace_back(tempMat);



				////Rand1 
				//tempMat = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 0.0f, 50.0f));
				//tempMat = glm::scale(tempMat, glm::vec3(1.0f, 1.0f, 1.0f));
				//testBound.Transforms.emplace_back(tempMat);

				////Rand2
				//tempMat = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 0.0f, 20.0f));
				//tempMat = glm::scale(tempMat, glm::vec3(1.0f, 1.0f, 1.0f));
				//testBound.Transforms.emplace_back(tempMat);


				////Rand5
				//tempMat = glm::translate(glm::mat4(1.0f), glm::vec3(35.0f, 0.0f, 20.0f));
				//tempMat = glm::scale(tempMat, glm::vec3(1.0f, 1.0f, 1.0f));
				//testBound.Transforms.emplace_back(tempMat);


				//BoolMapGenerator::AddBatch(testBound);

				//Render
				//std::shared_ptr<BoolMap> ptr = BoolMapGenerator::RenderBoolMap();



				//Check


				//bool topRight = ptr->CheckCollition(glm::vec3(128.0f,1.0f,72.0f));

				//bool botLeft = ptr->CheckCollition(glm::vec3(0.0f,1.0f, 0.0f));

				//bool botLeft1 = ptr->CheckCollition(glm::vec3(1.0f,1.0f, 0.0f));

				//bool botLeft2 = ptr->CheckCollition(glm::vec3(0.99f,1.0f, 0.0f));

				//bool botLeft3 = ptr->CheckCollition(glm::vec3(0.999f,1.0f, 0.0f));

				//bool botLeft4 = ptr->CheckCollition(glm::vec3(1.001f,1.0f, 0.0f));

				//bool botLeft5 = ptr->CheckCollition(glm::vec3(1.0f,1.0f, 0.0f));

				//bool botLeft6 = ptr->CheckCollition(glm::vec3(0.999f,1.0f, 0.0f));

				//bool botLeft7 = ptr->CheckCollition(glm::vec3(1.1f,1.0f, 0.0f));

				//bool botLeft8 = ptr->CheckCollition(glm::vec3(1.099f,1.0f, 0.0f));


				//bool botRight = ptr->CheckCollition(glm::vec3(128.0f,1.0f,0.0f));
				//bool topLeft = ptr->CheckCollition(glm::vec3(0.0f,1.0f,72.0f));
				//bool randomSpot = ptr->CheckCollition(glm::vec3(33.0f,1.0f,38.0f));
				//bool randomSpot2 = ptr->CheckCollition(glm::vec3(34.0f,1.0f,40.0f));

				//bool random1 = ptr->CheckCollition(glm::vec3(51.0f, 1.0f, 50.0f));
				//bool random2= ptr->CheckCollition(glm::vec3(31.5f, 1.0f, 20.0f));
				//bool random3= ptr->CheckCollition(glm::vec3(34.0f, 1.0f, 19.0f));


				NodeMap testMap(100, 100);

				testMap.GenereateMap();

				testMap.RenderMap();

				Renderer::SetCamera(m_EditorCamera.GetPosition(), m_EditorCamera.GetViewMatrix(), m_EditorCamera.GetProjectionMatrix());
			}
			else
			{
	
				
				m_World->OnStart();
				m_World->OnInput();
				m_World->OnUpdate();
				m_World->BeginScene();
			}

			//(Change if needed)
			glm::vec4 temp = m_Window->GetViewport();
			glViewport((int)temp.x, (int)temp.y, (int)temp.z, (int)temp.w);
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
		//if (e.GetKeyCode() == GLFW_KEY_O)
		//{
		//	std::cout << "Options was clicked!!" << std::endl;
		//}
		if (e.GetKeyCode() == FY_KEY_M)
		{
			/*states.GetActiveState()->OnInput();*/
		}
	}

	void Application::SubmitPrefab(std::string prefabName)
	{
		/*auto tempPrefabManager = PrefabManager::GetPrefabManager();
		Prefab* tempPrefab = tempPrefabManager->GetPrefab(prefabName);

		tempPrefab->GetModelKey().GetKeyData().GetVertexArray(0)->Bind();
		Renderer::Submit(m_Shader2, tempPrefab->GetModelKey().GetKeyData().GetVertexArray(0));

		std::shared_ptr<VertexArray> v = tempPrefab->GetModelKey().GetKeyData().GetVertexArray(0);*/

		////I want a texture! >:C

		///*RenderModel
		//(
		//	tempPrefab->GetModelKey().GetKeyData().GetVBO(0),
		//	tempPrefab->GetModelKey().GetKeyData().GetMeshConst(0).vertexCount,
		//	m_Transform.getModel(), //temp
		//	tempPrefab->GetMaterialKey().GetKeyData().Diffuse_Texture_MetaData_Ptr->GetData()->GetBufferID()

		//);*/
	}
}
