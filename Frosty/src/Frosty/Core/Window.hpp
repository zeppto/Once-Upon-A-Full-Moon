#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <fypch.hpp>		// This is already included in the cpp file so we shouldn't need it here but doesn't work otherwise

#include "Frosty/Core/Base.hpp"
#include <GLFW/glfw3.h>

namespace Frosty
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		unsigned int PositionX;
		unsigned int PositionY;

		WindowProps(const std::string& title = "Frosty Engine", unsigned int width = 1280, unsigned int height = 720, unsigned int posx = 100, unsigned int posy = 200)
			: Title(title), Width(width), Height(height), PositionX(posx), PositionY(posy) { }
	};

	class Window
	{
	public:
		Window();
		Window(const Window& org) { FY_CORE_ASSERT(false, "Copy constructor in Window called."); }
		virtual ~Window() { }

		// Operators
		Window& operator=(const Window& org) { FY_CORE_ASSERT(false, "Assignment operator in Window called."); return *this; }

		inline std::string GetTitle() const { return m_Data.Title; }
		inline unsigned int GetWidth() const { return m_Data.Width; }
		inline unsigned int GetHeight() const { return m_Data.Height; }
		inline unsigned int GetPositionX() const { return m_Data.PositionX; }
		inline unsigned int GetPositionY() const { return m_Data.PositionY; }
		std::pair<unsigned int, unsigned int> GetPosition() const { return { m_Data.PositionX, m_Data.PositionY }; }

		bool IsMaximized() const { return m_Data.Maximized; }
		void Maximize() { glfwMaximizeWindow(m_Window); }

		void SetVSync(bool enabled);
		bool IsVSync();

		void OnEvent(BaseEvent& e);
		void OnUpdate();

		inline void* GetNativeWindow() const { return m_Window; }

	private:
		void Init(const WindowProps& props = WindowProps());
		void InitCallbacks();

		void Shutdown() { glfwTerminate(); }

		void UpdateViewport();

		void OnWindowResizeEvent(WindowResizeEvent& e);
		void OnWindowMovedEvent(WindowMovedEvent& e);

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
