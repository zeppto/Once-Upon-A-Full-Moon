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
		Window(const WindowProps& props = WindowProps());
		virtual ~Window() { }

		inline std::string GetTitle() const { return m_Data.Title; }
		inline unsigned int GetWidth() const { return m_Data.Width; }
		inline unsigned int GetHeight() const { return m_Data.Height; }
		inline unsigned int GetPositionX() const { return m_Data.PositionX; }
		inline unsigned int GetPositionY() const { return m_Data.PositionY; }
		std::pair<unsigned int, unsigned int> GetPosition() const;

		void OnEvent(BaseEvent& e);
		void OnUpdate();

		inline void* GetNativeWindow() const { return m_Window; }

	private:
		void Init(const WindowProps& props);
		void InitCallbacks();

		void OnWindowResizeEvent(WindowResizeEvent& e);
		void OnWindowMovedEvent(WindowMovedEvent& e);

		void UpdateViewPort();
	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title{ "Frosty Engine" };
			unsigned int Width, Height;
			unsigned int PositionX, PositionY;
		};
		WindowData m_Data;
	};
}
#endif
