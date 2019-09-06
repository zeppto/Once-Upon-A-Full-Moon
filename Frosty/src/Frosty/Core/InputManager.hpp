#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

namespace Frosty
{
	class InputManager
	{
	public:
		InputManager() { }
		virtual ~InputManager() { }

		bool IsKeyPressed(int key);
		bool IsKeyReleased(int key);
		bool IsMouseButtonPressed(int button);
		bool IsMouseButtonReleased(int button);
		std::pair<float, float> GetMousePosition();
		float GetMouseX();
		float GetMouseY();
		bool IsClicked(int button, const glm::vec4& bounds);

	private:
		int CheckKeyClickType(int key);
		int CheckMouseClickType(int button);

	};
}


#endif // !INPUT_MANAGER_HPP