#include <fypch.hpp>
#include "Window.hpp"
#include "Frosty/DEFINITIONS.hpp"
#include "Frosty/Core/Application.hpp"
#include "Frosty/Events/KeyEvent.hpp"
#include "Frosty/Events/MouseEvent.hpp"

#include <glad/glad.h>

namespace Frosty
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		FY_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	BaseWindow* BaseWindow::Create()
	{
		return FY_NEW Window();
	}
	//This function specifies the layout of debug messages
	void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
		//Take out 131185 for example to test debug messages
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) { //Insignificant errors/notifications
			return;
		}

		//std::cout << "Debug message (" << id << "): " << message << std::endl;

		std::string sourceStr;
		switch (source) {
		case GL_DEBUG_SOURCE_API:             sourceStr = "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Source: Other"; break;
		}

		std::string typeStr;
		switch (type) {
		case GL_DEBUG_TYPE_ERROR:               typeStr = "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          typeStr = "Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           typeStr = "Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               typeStr = "Other"; break;
		}

		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:         FY_CORE_ERROR("OpenGL Debug message ({0}): {1}\n{2}\nType: {3}\nSeverity: high", id, message, sourceStr, typeStr); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       FY_CORE_ERROR("OpenGL Debug message ({0}): {1}\n{2}\nType: {3}\nSeverity: medium", id, message, sourceStr, typeStr); break;
		case GL_DEBUG_SEVERITY_LOW:          FY_CORE_INFO("OpenGL Debug message ({0}): {1}\n{2}\nType: {3}\nSeverity: low", id, message, sourceStr, typeStr); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: FY_CORE_TRACE("OpenGL Debug message ({0}): {1}\n{2}\nType: {3}\nSeverity: notification", id, message, sourceStr, typeStr); break;
		} std::cout << std::endl;
		std::cout << std::endl;
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

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		glfwWindowHint(GLFW_SAMPLES, 4); //Anti-aliasing

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);

		glfwSetWindowPos(m_Window, props.PositionX, props.PositionY);
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.PositionX = props.PositionX;
		m_Data.PositionY = props.PositionY;
		m_Data.refWidth = 1280.f;
		m_Data.refHeight = 720.f;
		m_Data.Viewport = glm::vec4(0, 0, m_Data.Width, m_Data.Height);

		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FY_CORE_ASSERT(status, "Failed to initialize Glad!");
		FY_CORE_INFO("OpenGL Info:");
		FY_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		FY_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		FY_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

		glfwSetWindowUserPointer(m_Window, &m_Data);

		glEnable(GL_MULTISAMPLE); //Anti-aliasing
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);

		glViewport(0, 0, m_Data.Width, m_Data.Height);

		//Test if debug output could be initialized
		GLint flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			FY_CORE_INFO("Debug output successfully initialized.");
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}

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

	

	float Window::GetWidthMultiplier() const
	{
		return  m_Data.Width / m_Data.refWidth;;
	}

	float Window::GetHeightMultiplier() const
	{
		return m_Data.Height / m_Data.refHeight;
	}

	

	const glm::vec4& Window::GetViewport() const
	{
		return m_Data.Viewport;
	}

	void Window::ActivateEditorMode()
	{
		m_Data.EditorMode = true;
		m_Data.Viewport = glm::vec4(
			EDITOR_EXPLORER_WIDTH,													// Start from left side
			EDITOR_ASSETS_HEIGHT,													// Start from bottom
			m_Data.Width - EDITOR_INSPECTOR_WIDTH - EDITOR_EXPLORER_WIDTH,			// Width
			m_Data.Height - EDITOR_ASSETS_HEIGHT - EDITOR_MAIN_MENU_BAR_HEIGHT);	// Height

		glViewport((int)m_Data.Viewport.x, (int)m_Data.Viewport.y, (int)m_Data.Viewport.z, (int)m_Data.Viewport.w);
	}

	void Window::ActivateGameMode()
	{
		m_Data.EditorMode = false;
		m_Data.Viewport = glm::vec4(0, 0, m_Data.Width, m_Data.Height);

		glViewport((int)m_Data.Viewport.x, (int)m_Data.Viewport.y, (int)m_Data.Viewport.z, (int)m_Data.Viewport.w);
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

		auto& app = Application::Get();
		if (app.GameIsRunning()) ActivateGameMode();
		else ActivateEditorMode();
	}

	void Window::OnWindowMovedEvent(WindowMovedEvent& e)
	{
		m_Data.PositionX = e.GetXPos();
		m_Data.PositionY = e.GetYPos();

		auto& app = Application::Get();
		if (app.GameIsRunning()) ActivateGameMode();
		else ActivateEditorMode();
	}
}