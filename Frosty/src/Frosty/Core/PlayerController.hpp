#ifndef PLAYERCONTROLLER_HPP
#define PLAYERCONTROLLER_HPP

namespace Frosty
{
	class PlayerController
	{
	public:
		PlayerController();
		~PlayerController();
		glm::vec3 PlayerControllerUpdate();

	private:
		glm::vec3 m_Pos;
		float m_DeltaTime = 0.0f, m_CurrentFrame = 0.0f, m_LastFrame = 0.0f, m_Speed = 2.0f;
	};
}

#endif