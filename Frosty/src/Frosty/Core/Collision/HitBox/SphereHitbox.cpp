#include "fypch.hpp"
#include "SphereHitbox.hpp"
#include "glm/glm.hpp" 

namespace Frosty
{

	double gcd(double a, double b)
	{
		if (a < b)
			return gcd(b, a);

		// base case 
		if (fabs(b) < 0.001)
			return a;

		else
			return (gcd(b, a - floor(a / b) * b));
	}


	Frosty::SphereHitbox::SphereHitbox()
	{

		m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
		m_Direction = glm::vec3(0.0f, 1.0f, 0.0f);

		m_Length = 1.0f;
		m_Raduis = 1.0f;

	}

	Frosty::SphereHitbox::SphereHitbox(const float& Length, const float& Raduis)
	{


		m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
		m_Direction = glm::vec3(0.0f, 1.0f, 0.0f);

		m_Length = Length;
		m_Raduis = Raduis;

	}

	Frosty::SphereHitbox::~SphereHitbox()
	{
	}

	bool Frosty::SphereHitbox::IsCollidingWith(const SphereHitbox& other) const
	{
		float thisLength = ((m_Length / 2) + m_Raduis);
		float otherLength = ((other.m_Length / 2) + other.m_Raduis);
		glm::vec3 TempPOVec = other.m_Position - m_Position;
		bool returnValue = false;

		if (TempPOVec.length() <= (thisLength + otherLength))
		{
			glm::vec3 OrthoVec = glm::cross(m_Direction, other.m_Direction);
			glm::vec3 Final = ((glm::dot(TempPOVec, OrthoVec) / glm::dot(OrthoVec, OrthoVec)) * OrthoVec);

			if (glm::length(Final) < (m_Raduis + other.m_Raduis))
			{

				glm::mat3 OneToTwo = 
				{	other.m_Direction.x, -m_Direction.x, other.m_Position.x - m_Position.x,
					other.m_Direction.y, -m_Direction.y, other.m_Position.y - m_Position.y,
					other.m_Direction.z, -m_Direction.z, other.m_Position.z - m_Position.z
				};

				glm::vec3 calcVecOne(0.0f, 0.0f, 0.0f);
				glm::vec3 calcVecTwo(0.0f, 0.0f, 0.0f);

				for (uint8_t i = 0; i < 3; i++)
				{

					calcVecOne[0] += OneToTwo[i][0] * m_Direction[i];
					calcVecOne[1] += OneToTwo[i][1] * m_Direction[i];
					calcVecOne[2] += OneToTwo[i][2] * m_Direction[i];

					calcVecTwo[0] += OneToTwo[i][0] * other.m_Direction[i];
					calcVecTwo[1] += OneToTwo[i][1] * other.m_Direction[i];
					calcVecTwo[2] += OneToTwo[i][2] * other.m_Direction[i];
				}

				float s_Value;
				float t_Value;

				//T
				if (abs(calcVecOne[0]) != abs(calcVecTwo[0]))
				{
		/*			if (abs(calcVecOne[0]) > abs(calcVecTwo[0]))
					{*/
						float tempTimes = calcVecOne[0] / calcVecTwo[0];
						glm::vec3 tempVec = calcVecTwo * tempTimes;


						glm::vec3 tempRes;


						if (tempVec[0] < 0 && calcVecOne[0] > 0)
						{
							tempRes = tempVec + calcVecOne;
						}

						else if (tempVec[0] > 0 && calcVecOne[0] > 0)
						{
							tempRes = tempVec - calcVecOne;
						}

						else if (tempVec[0] > 0 && calcVecOne[0] < 0)
						{
							tempRes = tempVec + calcVecOne;
						}

						else
						{
							tempRes = tempVec - calcVecOne;
						}
						
						if (tempRes[1] < 0)
						{
							tempRes *= -1;
						}

						t_Value = tempRes[2] / tempRes[1];

						float sSubValue = calcVecOne.y* t_Value;

						if (calcVecOne[0] < 0)
						{
						s_Value = -((calcVecOne[2] - sSubValue) / calcVecOne[0]);
						}
						else
						{
						s_Value = (calcVecOne[2] - sSubValue) / calcVecOne[0];
						}


						glm::vec3 P_Pos = 
							glm::vec3(
							m_Position.x + m_Direction.x * s_Value,
							m_Position.y + m_Direction.y * s_Value,
							m_Position.z + m_Direction.z * s_Value);

						float P_Pos_Length = glm::length((P_Pos - m_Position));

						glm::vec3 Q_Pos =
							glm::vec3(
								other.m_Position.x + other.m_Direction.x * t_Value,
								other.m_Position.y + other.m_Direction.y * t_Value,
								other.m_Position.z + other.m_Direction.z * t_Value);

						float Q_Pos_Length = glm::length((Q_Pos - other.m_Position));


						if (P_Pos_Length <= (m_Length + m_Raduis) && Q_Pos_Length <= (other.m_Length + other.m_Raduis))
						{
							if (P_Pos_Length <= m_Length && Q_Pos_Length <= other.m_Length)
							{

								if (glm::length(Q_Pos - P_Pos) <= (m_Raduis + other.m_Raduis))
								{
									returnValue = true;
								}


							}


						}



				//	}
					//else
					//{


					//	float tempTimes = calcVecTwo[0] / calcVecOne[0];
					//	glm::vec3 tempVec = calcVecOne * tempTimes;

					//	glm::vec3 tempRes = tempVec - calcVecTwo;

					//	t_Value = tempRes[2] / tempRes[1];


					//}


				}
				else
				{

				}

				//S
			/*	if (abs(calcVecOne[1]) != abs(calcVecTwo[1]))
				{
					if (abs(calcVecOne[1]) > abs(calcVecTwo[1]))
					{


						float tempTimes = calcVecOne[1] / calcVecTwo[1];
						glm::vec3 tempVec = calcVecTwo * tempTimes;

						glm::vec3 tempRes = tempVec - calcVecOne;

						s_Value = tempVec[2] / tempVec[0];

					}
					else
					{

					}
				}
				else
				{

				}
*/

					



			}



		}



		return returnValue;
	}
}
