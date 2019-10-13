#ifndef  SPHEREHITBOX_HPP
#define SPHEREHITBOX_HPP

namespace Frosty
{

	//Arv??? Base Hitbox?
	class SphereHitbox
	{

	public:
		const float SMALL_NUM = 10e-8;

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
		bool ColTest2(const SphereHitbox& other);





	private:

		glm::vec3 m_Temp_Calc_This_Dir;
		glm::vec3 m_Temp_Calc_Other_Dir;
	
		bool SphereCheck(const SphereHitbox& Other);
		const float calcShortestDist(const glm::vec3& S1P0,const glm::vec3& S1P1, const glm::vec3& S2P0, const glm::vec3& S2P1) const;

	};

}

#endif // ! SPHEREHITBOX_HPP

