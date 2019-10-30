#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Frosty/Core/BaseWindow.hpp"
#include "Frosty/Core/LayerHandler.hpp"
#include "Frosty/Core/EventSystem.hpp"
#include "Frosty/ImGui/ImGuiLayer.hpp"
//#include"Frosty/API/PrefabManager/PrefabManager.h"
#include "Frosty/RenderEngine/EditorCamera.hpp"
#include "Frosty/Core/World.hpp"
//#include "Frosty/UI/Canvas.h"

//Particle Branch
#include "Frosty/StateMachine/StateMachine.hpp"
#include "Frosty/Core/InputManager.hpp"
#include "Frosty/StateMachine/MainMenuState.h"

// test
#include "Frosty/RenderEngine/Shader.hpp"
#include "Frosty/RenderEngine/Buffer.hpp"
#include "Frosty/RenderEngine/VertexArray.hpp"
//#include "Frosty/UI/Sprite.h"
//#include "Frosty/UI/Canvas.h"
#include "Frosty/DEFINITIONS.hpp"
#include "Frosty/Particles/ParticleSystem.h"

namespace Frosty
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(BaseEvent& e);

		// Layer Functions
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);

		inline EditorCamera& GetEditorCamera() { return m_EditorCamera; }
		inline std::unique_ptr<World>& GetWorld() { return m_World; }
		inline BaseWindow& GetWindow() { return *m_Window; }
		static inline Application& Get() { return *s_Instance; }

		// Game Functions
		inline bool GameIsRunning() const { return m_GameRunning; }
		void StartGame(bool maximize);
		void StopGame(bool maximize);

		//~~ States stuff, temp
		//inline StateMachine& getStateMachine() { return states; }
		//void setMainMenuReturn(bool ret) { mainMenuReturn = ret; }
		//bool getMainMenuReturn() { return mainMenuReturn; }

	private:
		void OnWindowCloseEvent(WindowCloseEvent& e);
		void OnKeyPressedEvent(KeyPressedEvent& e);	
		void InitPrefabBuffers();
		void SubmitPrefab(std::string prefabName);//Temp

	private:
		InputManager m_InputManager;
		bool m_Running = true;
		bool m_GameRunning = false;

		ImGuiLayer* m_ImGuiLayer;
		LayerHandler m_LayerHandler;
		std::unique_ptr<World> m_World;

		std::unique_ptr<BaseWindow> m_Window;

		static Application* s_Instance;

		EditorCamera m_EditorCamera;

		// Particle Shader Stuff
		//~~ States stuff (temp)
	/*	StateMachine states;*/
		bool mainMenuReturn;		
		
		//std::shared_ptr<Canvas> m_Canvas;
		//std::shared_ptr<Sprite> m_Sprite;
		//std::shared_ptr<ParticleSystem> m_particleSystem;
		//
		//std::shared_ptr<Shader> m_Shader;
		//std::shared_ptr<Shader> m_Shader2;
		//std::shared_ptr<Shader> m_textShader;
		//std::shared_ptr<Shader> m_particleShader;
		//std::shared_ptr<Shader> m_computeParticleShader;
		//std::shared_ptr<VertexArray> m_VertexArray;
		//std::shared_ptr<VertexArray> m_TextVertexArray;
		//std::shared_ptr<VertexBuffer> m_textVertBuffer;
		//Canvas m_Canvas;
	};
}
#endif 