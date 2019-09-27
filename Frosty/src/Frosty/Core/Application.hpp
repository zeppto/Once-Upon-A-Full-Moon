#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Window.hpp"
#include "ECS.hpp"
#include "LayerHandler.hpp"
#include "EventSystem.hpp"
#include "Frosty/ImGui/ImGuiLayer.hpp"
#include "Frosty/RenderEngine/RenderEngine.hpp"
#include "Frosty/API/AssetManager/Assetmanager.hpp"

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
		static inline Application& Get() { return *s_Instance; }

		// ECS Stuff (TEMPORARY)
		const ECS::EntityManager& GetEntityManager() const { return m_EntityManager; }
		std::shared_ptr<ECS::Entity>& CreateEntity() { return m_EntityManager.CreateEntity(); }

	private:
		void OnWindowCloseEvent(WindowCloseEvent& e);
		void OnKeyPressedEvent(KeyPressedEvent& e);


	private:
		bool m_Running = true;

		ImGuiLayer* m_ImGuiLayer;
		LayerHandler m_LayerHandler;

		std::unique_ptr<Window> m_Window;

		RenderEngine* m_RenderEngine;

		static Application* s_Instance;

		// ECS stuff (TEMPORARY)
		ECS::EntityManager m_EntityManager;

	public:
		std::array<std::shared_ptr<ECS::BaseComponentManager>, ECS::MAX_COMPONENTS> m_ComponentManagers;
		unsigned int m_TotalComponentManagers{ 0 };

	};
}
#endif // !APPLICATION_HPP
