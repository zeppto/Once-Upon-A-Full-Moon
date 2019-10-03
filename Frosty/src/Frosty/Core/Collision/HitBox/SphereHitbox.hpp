#ifndef  SPHEREHITBOX_HPP
#define SPHEREHITBOX_HPP

namespace Frosty
{

	//Arv??? Base Hitbox?
	class SphereHitbox
	{

	public:


	private:

		glm::vec3 m_Position;
		glm::vec3 m_Direction;
		glm::vec3 m_Scale;
		float m_Length;
		float m_Raduis;

	public:
		SphereHitbox();
		SphereHitbox(const float& Length, const float& Raduis);
		virtual ~SphereHitbox();

		//If you have a better name then rename it
		bool IsCollidingWith(const SphereHitbox& other) const;





	private:

	};

}

#endif // ! SPHEREHITBOX_HPP

