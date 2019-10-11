#include "fypch.hpp"
#include "Window.hpp"
#include "Frosty/DEFINITIONS.hpp"

#include <glad/glad.h>

namespace Frosty
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		FY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window::Window()
	{
	}

	void Window::Init(const WindowProps& props)
	{
		FY_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			FY_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);

		glfwSetWindowPos(m_Window, props.PositionX, props.PositionY);
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.PositionX = props.PositionX;
		m_Data.PositionY = props.PositionY;

		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FY_CORE_ASSERT(status, "Failed to initialize Glad!");
		FY_CORE_INFO("OpenGL Info:");
		FY_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		FY_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		FY_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

		glfwSetWindowUserPointer(m_Window, &m_Data);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);

		glViewport(0, 0, m_Data.Width, m_Data.Height);

		InitCallbacks();

		glfwMaximizeWindow(m_Window);
	}

	void Window::InitCallbacks()
	{
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			EventBus::GetEventBus()->Publish<WindowCloseEvent>(WindowCloseEvent());
		});

		glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximize)
		{
			auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

			data.Maximized = maximize;

			EventBus::GetEventBus()->Publish<WindowMaximizeEvent>(WindowMaximizeEvent(maximize));
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

			data.Width = width;
			data.Height = height;

			EventBus::GetEventBus()->Publish<WindowResizeEvent>(WindowResizeEvent(width, height));
		});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
		{
			auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

			data.PositionX = xpos;
			data.PositionY = ypos;

			EventBus::GetEventBus()->Publish<WindowMovedEvent>(WindowMovedEvent(xpos, ypos));
		});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
		{
			EventBus::GetEventBus()->Publish<WindowFocusEvent>(WindowFocusEvent(focused));
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			switch (action)
			{
			case GLFW_PRESS:
				EventBus::GetEventBus()->Publish<KeyPressedEvent>(KeyPressedEvent(key, 0));
				break;
			case GLFW_REPEAT:
				EventBus::GetEventBus()->Publish<KeyPressedEvent>(KeyPressedEvent(key, 1));
				break;
			case GLFW_RELEASE:
				EventBus::GetEventBus()->Publish<KeyReleasedEvent>(KeyReleasedEvent(key));
				break;
			default:
				break;
			}
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
		{
			EventBus::GetEventBus()->Publish<MouseMovedEvent>(MouseMovedEvent((float)xpos, (float)ypos));
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			EventBus::GetEventBus()->Publish<MouseScrolledEvent>(MouseScrolledEvent((float)xoffset, (float)yoffset));
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			switch (action)
			{
			case GLFW_PRESS:
				EventBus::GetEventBus()->Publish<MouseButtonPressedEvent>(MouseButtonPressedEvent(button));
				break;
			case GLFW_RELEASE:
				EventBus::GetEventBus()->Publish<MouseButtonReleasedEvent>(MouseButtonReleasedEvent(button));
				break;
			default:
				break;
			}
		});

	}

	glm::vec4 Window::GetViewport() const
	{
		return glm::vec4(
			EDITOR_EXPLORER_WIDTH,													// Start from left side
			EDITOR_ASSETS_HEIGHT,													// Start from bottom
			m_Data.Width - EDITOR_INSPECTOR_WIDTH - EDITOR_EXPLORER_WIDTH,			// Width
			m_Data.Height - EDITOR_ASSETS_HEIGHT - EDITOR_MAIN_MENU_BAR_HEIGHT);	// Height
	}

	void Window::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool Window::IsVSync()
	{
		return m_Data.VSync;
	}

	void Window::UpdateViewport()
	{
		glViewport(
			EDITOR_EXPLORER_WIDTH,													// Start from left side
			EDITOR_ASSETS_HEIGHT,													// Start from bottom
			m_Data.Width - EDITOR_INSPECTOR_WIDTH - EDITOR_EXPLORER_WIDTH,			// Width
			m_Data.Height - EDITOR_ASSETS_HEIGHT - EDITOR_MAIN_MENU_BAR_HEIGHT);	// Height
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void Window::OnEvent(BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case EventType::WindowResize:
			OnWindowResizeEvent(static_cast<WindowResizeEvent&>(e));
			break;
		case EventType::WindowMoved:
			OnWindowMovedEvent(static_cast<WindowMovedEvent&>(e));
			break;
		default:
			break;
		}
	}

	void Window::OnWindowResizeEvent(WindowResizeEvent& e)
	{
		m_Data.Width = e.GetWidth();
		m_Data.Height = e.GetHeight();

		UpdateViewport();
	}

	void Window::OnWindowMovedEvent(WindowMovedEvent& e)
	{
		m_Data.PositionX = e.GetXPos();
		m_Data.PositionY = e.GetYPos();

		UpdateViewport();
	}
}