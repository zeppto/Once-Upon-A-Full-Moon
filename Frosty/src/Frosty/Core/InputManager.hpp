#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

namespace Frosty
{
	class InputManager
	{
	public:
		InputManager() { }
		virtual ~InputManager() { }

		static bool IsKeyPressed(int key);
		static bool IsKeyReleased(int key);
		static bool IsMouseButtonPressed(int button);
		static bool IsMouseButtonReleased(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
		static bool IsClicked(int button, const glm::vec4& bounds);

	private:
		static int CheckKeyClickType(int key);
		static int CheckMouseClickType(int button);

	};
}


#endif // !INPUT_MANAGER_HPP