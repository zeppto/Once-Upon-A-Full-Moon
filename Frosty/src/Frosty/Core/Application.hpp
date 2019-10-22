#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Frosty/Core/BaseWindow.hpp"
#include "Frosty/Core/LayerHandler.hpp"
#include "Frosty/Core/EventSystem.hpp"
#include "Frosty/ImGui/ImGuiLayer.hpp"
//#include"Frosty/API/PrefabManager/PrefabManager.h"
#include "Frosty/RenderEngine/EditorCamera.hpp"
#include "Frosty/Core/World.hpp"
#include "Frosty/UI/Canvas.h"

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

	private:
		void OnWindowCloseEvent(WindowCloseEvent& e);
		void OnKeyPressedEvent(KeyPressedEvent& e);

	private:
		bool m_Running = true;
		bool m_GameRunning = false;

		ImGuiLayer* m_ImGuiLayer;
		LayerHandler m_LayerHandler;
		std::unique_ptr<World> m_World;

		std::unique_ptr<BaseWindow> m_Window;

		static Application* s_Instance;

		EditorCamera m_EditorCamera;

		Canvas m_Canvas;
	};
}
#endif 