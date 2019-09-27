#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include "Window.hpp"
#include "LayerHandler.hpp"
#include "EventSystem.hpp"
#include "Frosty/ImGui/ImGuiLayer.hpp"
#include "Frosty/RenderEngine/RenderEngine.hpp"
#include "Frosty/StateMachine/StateMachine.hpp"
#include "Frosty/Core/InputManager.hpp"
#include "Frosty/StateMachine/MainMenuState.h"

namespace Frosty
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(BaseEvent& e);

		// Layer functions
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);

		// RenderEngine
		RenderEngine* GetRenderEngine() { return m_RenderEngine; }

		inline Window& GetWindow() { return *m_Window; }
		inline InputManager& GetInputManager() { return m_InputManager; }
		static inline Application& Get() { return *s_Instance; }

		inline StateMachine& getStateMachine() { return &states; }

	private:
		void OnWindowCloseEvent(WindowCloseEvent& e);
		void OnKeyPressedEvent(KeyPressedEvent& e);

	private:
		InputManager m_InputManager;
		bool m_Running = true;

		ImGuiLayer* m_ImGuiLayer;
		LayerHandler m_LayerHandler;

		std::unique_ptr<Window> m_Window;

		RenderEngine* m_RenderEngine;

		static Application* s_Instance;

		StateMachine states;
	};
}
#endif // !APPLICATION_HPP
