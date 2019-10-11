#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include "ECS.hpp"
#include "Window.hpp"
#include "LayerHandler.hpp"
#include "EventSystem.hpp"
#include "Frosty/ImGui/ImGuiLayer.hpp"

#include "Frosty/StateMachine/StateMachine.hpp"
#include "Frosty/Core/InputManager.hpp"
#include "Frosty/StateMachine/MainMenuState.h"

#include "Frosty/API/AssetManager/Assetmanager.hpp"
//#include "Frosty/RenderEngine/RenderEngine.hpp"

// test
#include "Frosty/RenderEngine/ForwardPlus.hpp"
#include "Frosty/RenderEngine/Shader.hpp"
#include "Frosty/RenderEngine/Buffer.hpp"
#include "Frosty/RenderEngine/VertexArray.hpp"
#include "Frosty/UI/Sprite.h"
#include "Frosty/UI/Canvas.h"
#include "Frosty/DEFINITIONS.hpp"
#include "Frosty/API/PrefabManager/PrefabManager.h"
#include "Frosty/Core/Camera/Camera.hpp"
#include "Frosty/Particles/ParticleSystem.h"

namespace Frosty
{
	class Application
	{
	public:
		Application();
		virtual ~Application();
		
		// Temporary function (Testing)		
		void LoadModel(const std::string filename);			
		void InitShaders();

		void Run();

		void OnEvent(BaseEvent& e);

		// Layer functions
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);	

		//RenderEngine* GetRenderEngine() { return m_RenderEngine; }

		inline Window& GetWindow() { return *m_Window; }
		inline InputManager& GetInputManager() { return m_InputManager; }
		static inline Application& Get() { return *s_Instance; }

		//~~ States stuff, temp
		inline StateMachine& getStateMachine() { return states; }
		void setMainMenuReturn(bool ret) { mainMenuReturn = ret; }
		bool getMainMenuReturn() { return mainMenuReturn; }

		// ECS Stuff (TEMPORARY)
		const ECS::EntityManager& GetEntityManager() const { return m_EntityManager; }
		std::shared_ptr<ECS::Entity>& CreateEntity() { return m_EntityManager.CreateEntity(); }

	private:
		void OnWindowCloseEvent(WindowCloseEvent& e);
		void OnKeyPressedEvent(KeyPressedEvent& e);	
		void InitPrefabBuffers();
		void SubmitPrefab(std::string prefabName);//Temp

	private:
		InputManager m_InputManager;
		bool m_Running = true;

		ImGuiLayer* m_ImGuiLayer;
		LayerHandler m_LayerHandler;

		std::unique_ptr<Window> m_Window;
		
		static Application* s_Instance;

		//~~ States stuff (temp)
		StateMachine states;
		bool mainMenuReturn;

		// ECS stuff (TEMPORARY)
		ECS::EntityManager m_EntityManager;
		//std::unique_ptr<ECS::BaseComponentManager> m_TransformManager;		
		
		//RenderEngine* m_RenderEngine;
		std::shared_ptr<Canvas> m_Canvas;
		std::shared_ptr<Sprite> m_Sprite;
		std::shared_ptr<ParticleSystem> m_particleSystem;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<Shader> m_Shader2;
		std::shared_ptr<Shader> m_textShader;
		std::shared_ptr<Shader> m_particleShader;
		std::shared_ptr<Shader> m_computeParticleShader;
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexArray> m_TextVertexArray;
		std::shared_ptr<VertexBuffer> m_textVertBuffer;

		std::shared_ptr<Camera> m_Camera;
	};
}
#endif
