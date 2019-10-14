#include <fypch.hpp>
#include "InputManager.hpp"
#include "Application.hpp"
#include "Frosty/Core/KeyCodes.h"

#include <GLFW/glfw3.h>

namespace Frosty
{
	bool InputManager::IsKeyPressed(int key)
	{
		return CheckKeyClickType(key) == FY_PRESS;
	}

	bool InputManager::IsKeyReleased(int key)
	{
		return CheckKeyClickType(key) == FY_RELEASE;
	}

	bool InputManager::IsMouseButtonPressed(int button)
	{
		return CheckMouseClickType(button) == FY_PRESS;
	}

	bool InputManager::IsMouseButtonReleased(int button)
	{
		return CheckMouseClickType(button) == FY_RELEASE;
	}

	std::pair<float, float> InputManager::GetMousePosition()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		y = Application::Get().GetWindow().GetHeight() - y;
		return { (float)x, (float)y };
	}

	float InputManager::GetMouseX()
	{
		auto[x, y] = GetMousePosition();
		return x;
	}

	float InputManager::GetMouseY()
	{
		auto[x, y] = GetMousePosition();
		return y;
	}

	bool InputManager::IsClicked(int button, const glm::vec4& bounds)
	{
		if (IsMouseButtonPressed(button))
		{
			auto[x, y] = GetMousePosition();
			if (x >= bounds.x && x <= (bounds.z + bounds.x) && y <= bounds.y && y >= (bounds.y - bounds.w))
			{
				return true;
			}
		}
		return false;
	}

	int InputManager::CheckKeyClickType(int key)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		return glfwGetKey(window, key);
	}

	int InputManager::CheckMouseClickType(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		return glfwGetMouseButton(window, button);
	}
}