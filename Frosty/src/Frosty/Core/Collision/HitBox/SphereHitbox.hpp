#ifndef  SPHEREHITBOX_HPP
#define SPHEREHITBOX_HPP

namespace Frosty
{

	//Arv??? Base Hitbox?
	class SphereHitbox
	{

	public:


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
		bool IsCollidingWith(const SphereHitbox& other);

		bool ColTest(const SphereHitbox& other);





	private:

		glm::vec3 m_Temp_Calc_This_Dir;
		glm::vec3 m_Temp_Calc_Other_Dir;
	
		bool SphereCheck(const SphereHitbox& Other);

	};

}

#endif // ! SPHEREHITBOX_HPP

