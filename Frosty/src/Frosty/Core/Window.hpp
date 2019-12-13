#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <fypch.hpp>		// This is already included in the cpp file so we shouldn't need it here but doesn't work otherwise
#include "Frosty/Core/BaseWindow.hpp"

// Temp
#include "Frosty/Events/ApplicationEvent.hpp"

#include <GLFW/glfw3.h>

namespace Frosty
{

	class Window : public BaseWindow
	{
	public:
		Window();
		Window(const Window& org) { FY_CORE_ASSERT(false, "Copy constructor in Window called."); }
		virtual ~Window() { }

		// Operators
		Window& operator=(const Window& org) { FY_CORE_ASSERT(false, "Assignment operator in Window called."); return *this; }

		virtual inline std::string GetTitle() const override { return m_Data.Title; }
		virtual inline float GetWidth() const override { return m_Data.Width; }
		virtual inline float GetHeight() const override { return m_Data.Height; }
		virtual float GetWidthMultiplier() const override;
		virtual float GetHeightMultiplier() const override;
		virtual inline unsigned int GetPositionX() const override { return m_Data.PositionX; }
		virtual inline unsigned int GetPositionY() const override { return m_Data.PositionY; }

		virtual std::pair<unsigned int, unsigned int> GetPosition() const override { return { m_Data.PositionX, m_Data.PositionY }; }
		virtual const glm::vec4& GetViewport() const override;

		virtual bool IsEditorModeActivated() override { return m_Data.EditorMode; }
		virtual void ActivateEditorMode() override;
		virtual void ActivateGameMode() override;

		virtual bool IsMaximized() const override { return m_Data.Maximized; }
		virtual void Maximize() override { glfwMaximizeWindow(m_Window); }

		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() override;

		virtual void OnEvent(BaseEvent& e) override;
		virtual void OnUpdate() override;

		virtual inline void* GetNativeWindow() const override { return m_Window; }

	private:
		virtual void Init(const WindowProps& props = WindowProps()) override;
		void InitCallbacks();

		virtual void Shutdown() override { glfwTerminate(); }

		void OnWindowResizeEvent(WindowResizeEvent& e);
		void OnWindowMovedEvent(WindowMovedEvent& e);

	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title{ "Frosty Engine" };
			int Maximized = 0;
			float Width = 0.0f;
			float Height = 0.0f;
			unsigned int PositionX = 0;
			unsigned int PositionY = 0;
			float refWidth = 0.0f;
			float refHeight = 0.0f;
			float widthMultiplier{ 0.0f }, heightMultiplier{ 0.0f };
			float lastWidthMultiplier{ 0.0f }, lastHeightMultiplier{ 0.0f };
			bool VSync = false;
			glm::vec4 Viewport;
			bool EditorMode{ true };
		};
		WindowData m_Data;

		friend class Application;
	};
}
#endif
