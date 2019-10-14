#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <fypch.hpp>		// This is already included in the cpp file so we shouldn't need it here but doesn't work otherwise
#include "Frosty/Core/BaseWindow.hpp"

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
		virtual inline unsigned int GetWidth() const override { return m_Data.Width; }
		virtual inline unsigned int GetHeight() const override { return m_Data.Height; }
		virtual inline unsigned int GetPositionX() const override { return m_Data.PositionX; }
		virtual inline unsigned int GetPositionY() const override { return m_Data.PositionY; }
		virtual std::pair<unsigned int, unsigned int> GetPosition() const override { return { m_Data.PositionX, m_Data.PositionY }; }
		virtual glm::vec4 GetViewport() const override;

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

		void UpdateViewport();

		void OnWindowResizeEvent(WindowResizeEvent& e);
		void OnWindowMovedEvent(WindowMovedEvent& e);

		//void UpdateViewPort();
	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title{ "Frosty Engine" };
			int Maximized;
			unsigned int Width, Height;
			unsigned int PositionX, PositionY;
			bool VSync;
		};
		WindowData m_Data;

		friend class Application;
	};
}
#endif
