#ifndef  SPHEREHITBOX_HPP
#define SPHEREHITBOX_HPP

namespace Frosty
{

	//Arv??? Base Hitbox?
	class SphereHitbox
	{

	public:
		//const float SMALL_NUM = 10e-8f;

		//glm::vec3 m_Position;
		//glm::vec3 m_Direction;
		//glm::vec3 m_Scale;
		//float m_Length;
		//float m_Raduis;

	public:
		SphereHitbox();
		virtual ~SphereHitbox();

		//If you have a better name then rename it
		static bool IsCollidingWith(glm::vec3 length, glm::vec3 center, glm::vec3  rotation, glm::vec3 otherLength, glm::vec3 otherCenter, glm::vec3 otherRotation);


	private:

		static float calcShortestDist(const glm::vec3& S1P0,const glm::vec3& S1P1, const glm::vec3& S2P0, const glm::vec3& S2P1);

	};

}

#endif // ! SPHEREHITBOX_HPP

