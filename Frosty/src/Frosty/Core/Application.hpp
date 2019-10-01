#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include "ECS.hpp"
#include "Window.hpp"
#include "LayerHandler.hpp"
#include "EventSystem.hpp"
#include "Frosty/ImGui/ImGuiLayer.hpp"
#include "Frosty/API/AssetManager/Assetmanager.hpp"
#include "Frosty/RenderEngine/Shader.hpp"
#include "Frosty/RenderEngine/VertexArray.hpp"
#include "Frosty/Core/Camera/Camera.hpp"

namespace Frosty
{
	class Application
	{
	public:
		Application();
		virtual ~Application();
		
		// Temporary function (Testing)
		void InitPrefabBuffers();
		void InitShaders();

		void Run();

		void OnEvent(BaseEvent& e);

		// Layer functions
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);		

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
		
		static Application* s_Instance;

		// ECS stuff (TEMPORARY)
		ECS::EntityManager m_EntityManager;
		//std::unique_ptr<ECS::BaseComponentManager> m_TransformManager;
		
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;	
		std::shared_ptr<Camera> m_Camera;
	};
}
#endif 
