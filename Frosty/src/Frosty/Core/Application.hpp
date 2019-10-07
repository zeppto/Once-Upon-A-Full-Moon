#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Window.hpp"
#include "LayerHandler.hpp"
#include "EventSystem.hpp"
#include "Frosty/ImGui/ImGuiLayer.hpp"
#include "Frosty/API/AssetManager/Assetmanager.hpp"
#include "Frosty/RenderEngine/Shader.hpp"
#include "Frosty/RenderEngine/Buffer.hpp"
#include "Frosty/RenderEngine/VertexArray.hpp"
#include "Frosty/RenderEngine/Texture.hpp"
#include "Frosty/RenderEngine/OrthographicCamera.hpp"
#include "Frosty/API/Scene.hpp"

namespace Frosty
{
	class Application
	{
	public:
		Application();
		virtual ~Application();
		
		// Temporary function (Testing)
		void InitiateQuadMesh();
		void InitiateTriangleMesh();
		void InitShaders();

		void Run();

		void OnEvent(BaseEvent& e);

		// Layer Functions
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);
		
		// Scene Functions
		std::unique_ptr<Scene>& CreateScene();
		void DestroyScene();
		std::unique_ptr<Scene>& GetScene();
		const std::unique_ptr<Scene>& GetScene() const;

		inline Window& GetWindow() { return *m_Window; }
		static inline Application& Get() { return *s_Instance; }

		// TEMPORARY
		inline const std::shared_ptr<VertexArray>& GetQuadMesh() const { return m_Quad; }
		inline const std::shared_ptr<VertexArray>& GetTriangleMesh() const { return m_Triangle; }
		inline const std::shared_ptr<Shader>& GetShader() const { return m_ShaderAssets; }

	private:
		void OnWindowCloseEvent(WindowCloseEvent& e);
		void OnKeyPressedEvent(KeyPressedEvent& e);	
	private:
		bool m_Running = true;

		ImGuiLayer* m_ImGuiLayer;
		LayerHandler m_LayerHandler;
		std::unique_ptr<Scene> m_Scene;

		std::unique_ptr<Window> m_Window;

		static Application* s_Instance;

		// Renderer Stuff (TEMPORARY)
		std::shared_ptr<Shader> m_ShaderAssets;
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_Quad;
		std::shared_ptr<VertexArray> m_Triangle;
		std::shared_ptr<Texture2D> m_Texture;

	};
}
#endif 
