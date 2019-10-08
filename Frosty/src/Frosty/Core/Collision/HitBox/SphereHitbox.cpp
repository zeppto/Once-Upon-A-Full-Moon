#include "fypch.hpp"
#include "SphereHitbox.hpp"
#include "glm/glm.hpp" 

namespace Frosty
{


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

	bool Frosty::SphereHitbox::IsCollidingWith(const SphereHitbox& other)
	{
		float thisLength = ((m_Length / 2) + m_Raduis);
		float otherLength = ((other.m_Length / 2) + other.m_Raduis);
		glm::vec3 TempPOVec = other.m_Position - m_Position;
		glm::vec3 This_Dir = m_Direction;
		glm::vec3 Other_Dir = other.m_Direction;
		bool returnValue = false;
		bool k = SphereCheck(other);

		if (TempPOVec.length() <= (thisLength + otherLength))
		{


			for (uint8_t i = 0; i < 3; i++)
			{
				if (This_Dir[i] < std::pow(10, -5))
				{
					This_Dir[i] = 0;
				}
				if (Other_Dir[i] < std::pow(10, -5))
				{
					Other_Dir[i] = 0;
				}

			}

			This_Dir = glm::normalize(This_Dir);
			Other_Dir = glm::normalize(Other_Dir);


			if (glm::dot(This_Dir, Other_Dir) != 0)
			{
				glm::vec3 OrthoVec = glm::cross(This_Dir, Other_Dir);

				glm::vec3 Final(0.0f);
				if (OrthoVec != glm::vec3(0.0f))
				{
					Final = ((glm::dot(TempPOVec, OrthoVec) / glm::dot(OrthoVec, OrthoVec)) * OrthoVec);
				}

				if (glm::length(Final) < (m_Raduis + other.m_Raduis))
				{

					glm::mat3 OneToTwo =
					{ Other_Dir.x, -This_Dir.x, Other_Dir.x - m_Position.x,
						Other_Dir.y, -This_Dir.y, Other_Dir.y - m_Position.y,
						Other_Dir.z, -This_Dir.z, Other_Dir.z - m_Position.z
					};

					glm::vec3 calcVecOne(0.0f, 0.0f, 0.0f);
					glm::vec3 calcVecTwo(0.0f, 0.0f, 0.0f);

					for (uint8_t i = 0; i < 3; i++)
					{

						calcVecOne[0] += OneToTwo[i][0] * This_Dir[i];
						calcVecOne[1] += OneToTwo[i][1] * This_Dir[i];
						calcVecOne[2] += OneToTwo[i][2] * This_Dir[i];

						calcVecTwo[0] += OneToTwo[i][0] * Other_Dir[i];
						calcVecTwo[1] += OneToTwo[i][1] * Other_Dir[i];
						calcVecTwo[2] += OneToTwo[i][2] * Other_Dir[i];
					}

					float s_Value;
					float t_Value;

					//T
					if (abs(calcVecOne[0]) != abs(calcVecTwo[0]))
					{
						//if (abs(calcVecOne[0]) > abs(calcVecTwo[0]))
						//{
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

						float sSubValue = calcVecOne.y * t_Value;

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
								m_Position.x + This_Dir.x * s_Value,
								m_Position.y + This_Dir.y * s_Value,
								m_Position.z + This_Dir.z * s_Value);

						float P_Pos_Length = glm::length((P_Pos - m_Position));

						glm::vec3 Q_Pos =
							glm::vec3(
								other.m_Position.x + Other_Dir.x * t_Value,
								other.m_Position.y + Other_Dir.y * t_Value,
								other.m_Position.z + Other_Dir.z * t_Value);

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

						glm::vec3 CylEdge_Pos = (m_Position + (This_Dir * m_Length));
						glm::vec3 CylEdge_Vec = CylEdge_Pos - m_Position;


						glm::vec3 OtherCylEdge_Pos = (other.m_Position + Other_Dir * other.m_Length);
						glm::vec3 OtherCylEdge_Vec = OtherCylEdge_Pos - other.m_Position;




						glm::vec3 proVec = ((glm::dot(TempPOVec, CylEdge_Vec) / glm::dot(CylEdge_Vec, CylEdge_Vec)) * CylEdge_Vec);

						float bothLng = glm::length(CylEdge_Vec) + glm::length(OtherCylEdge_Vec);

						if (glm::length(proVec) <= bothLng)
						{
							glm::vec3 LngBtwLin = other.m_Position - proVec;

							if (glm::length(LngBtwLin) <= (m_Raduis + other.m_Raduis))
							{
								returnValue = true;
							}
						}
						else if (glm::length(proVec) <= (bothLng + m_Raduis + other.m_Raduis))
						{
							glm::vec3 ThisE2 = (m_Position - (This_Dir * m_Length));
							glm::vec3 OtherE2 = (other.m_Position - (Other_Dir * other.m_Length));

							glm::vec3 shortOne, ShortTwo;

							float LngOne = glm::length(OtherCylEdge_Pos - CylEdge_Pos);
							float LngTwo = glm::length(OtherE2 - CylEdge_Pos);

							if (LngOne < LngTwo)
							{
								shortOne = OtherCylEdge_Pos - CylEdge_Pos;
							}
							else
							{
								shortOne = OtherE2 - CylEdge_Pos;
							}


							float LngThree = glm::length(OtherCylEdge_Pos - ThisE2);
							float LngFour = glm::length(OtherE2 - ThisE2);

							if (LngThree < LngFour)
							{
								ShortTwo = OtherCylEdge_Pos - ThisE2;
							}
							else
							{
								ShortTwo = OtherE2 - ThisE2;
							}

							if (glm::length(shortOne) < glm::length(ShortTwo))
							{
								if (glm::length(shortOne) <= (m_Raduis + other.m_Raduis))
								{
									returnValue = true;
								}
							}
							else
							{
								if (glm::length(ShortTwo) <= (m_Raduis + other.m_Raduis))
								{
									returnValue = true;
								}
							}



						}
					}
					////shpere same line
					//else if (1 == glm::dot(glm::normalize(TempPOVec),glm::normalize(m_Direction)))
					//{
					//	if (glm::length(TempPOVec) < (bothLng + m_Raduis + other.m_Raduis))
					//	{
					//		returnValue = true;
					//	}
					//}

					//}





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
			else
			{

			    glm::vec3 One_Two_Pro = ((glm::dot(TempPOVec, This_Dir) / glm::dot(This_Dir, This_Dir))* This_Dir);

				if (glm::length(One_Two_Pro) <= thisLength)
				{

					glm::vec3 TempPos = One_Two_Pro + m_Position;
					glm::vec3 tempBr = TempPos - other.m_Position;

					glm::vec3 Two_One_Pro = ((glm::dot(tempBr, Other_Dir) / glm::dot(Other_Dir, Other_Dir)) * Other_Dir);

					if (glm::length(Two_One_Pro) <= other.m_Length)
					{
						returnValue = true;
					}
					else if (glm::length(Two_One_Pro) <= other.m_Length + other.m_Raduis + m_Raduis)
					{
						glm::vec3 CylEdge_Pos = (m_Position + (This_Dir * m_Length));
						glm::vec3 CylEdge_Vec = CylEdge_Pos - m_Position;


						glm::vec3 OtherCylEdge_Pos = (other.m_Position + Other_Dir * other.m_Length);
						glm::vec3 OtherCylEdge_Vec = OtherCylEdge_Pos - other.m_Position;




						glm::vec3 proVec = ((glm::dot(TempPOVec, CylEdge_Vec) / glm::dot(CylEdge_Vec, CylEdge_Vec)) * CylEdge_Vec);

						float bothLng = glm::length(CylEdge_Vec) + glm::length(OtherCylEdge_Vec);

						if (glm::length(proVec) <= bothLng)
						{
							glm::vec3 LngBtwLin = other.m_Position - proVec;

							if (glm::length(LngBtwLin) <= (m_Raduis + other.m_Raduis))
							{
								returnValue = true;
							}
						}
						else if (glm::length(proVec) <= (bothLng + m_Raduis + other.m_Raduis))
						{
							glm::vec3 ThisE2 = (m_Position - (This_Dir * m_Length));
							glm::vec3 OtherE2 = (other.m_Position - (Other_Dir * other.m_Length));

							glm::vec3 shortOne, ShortTwo;

							float LngOne = glm::length(OtherCylEdge_Pos - CylEdge_Pos);
							float LngTwo = glm::length(OtherE2 - CylEdge_Pos);

							if (LngOne < LngTwo)
							{
								shortOne = OtherCylEdge_Pos - CylEdge_Pos;
							}
							else
							{
								shortOne = OtherE2 - CylEdge_Pos;
							}


							float LngThree = glm::length(OtherCylEdge_Pos - ThisE2);
							float LngFour = glm::length(OtherE2 - ThisE2);

							if (LngThree < LngFour)
							{
								ShortTwo = OtherCylEdge_Pos - ThisE2;
							}
							else
							{
								ShortTwo = OtherE2 - ThisE2;
							}

							if (glm::length(shortOne) < glm::length(ShortTwo))
							{
								if (glm::length(shortOne) <= (m_Raduis + other.m_Raduis))
								{
									returnValue = true;
								}
							}
							else
							{
								if (glm::length(ShortTwo) <= (m_Raduis + other.m_Raduis))
								{
									returnValue = true;
								}
							}



						}
					}
				}
				else if (glm::length(One_Two_Pro) <= (thisLength + other.m_Raduis))
				{

					glm::vec3 edgeOne = m_Position + (This_Dir * m_Length);
					glm::vec3 edgeTwo = m_Position - (This_Dir * m_Length);
					glm::vec3 MinVec;
					if (glm::length(edgeOne - other.m_Position) < glm::length(edgeTwo - other.m_Position))
					{
						MinVec = edgeOne;
					}
					else
					{
						MinVec = edgeTwo;
					}
						glm::vec3 temp = MinVec - other.m_Position;
						glm::vec3 Two_One_Pro = ((glm::dot(temp, Other_Dir) / glm::dot(Other_Dir, Other_Dir)) * Other_Dir);
						if (glm::length(Two_One_Pro) <= other.m_Length)
						{
							returnValue = true;

						}
						else if (glm::length(Two_One_Pro) < other.m_Length + other.m_Raduis)
						{
					

							glm::vec3 CylEdge_Pos = (m_Position + (This_Dir * m_Length));
							glm::vec3 CylEdge_Vec = CylEdge_Pos - m_Position;


							glm::vec3 OtherCylEdge_Pos = (other.m_Position + Other_Dir * other.m_Length);
							glm::vec3 OtherCylEdge_Vec = OtherCylEdge_Pos - other.m_Position;




							glm::vec3 proVec = ((glm::dot(TempPOVec, CylEdge_Vec) / glm::dot(CylEdge_Vec, CylEdge_Vec)) * CylEdge_Vec);

							float bothLng = glm::length(CylEdge_Vec) + glm::length(OtherCylEdge_Vec);

							if (glm::length(proVec) <= bothLng)
							{
								glm::vec3 LngBtwLin = other.m_Position - proVec;

								if (glm::length(LngBtwLin) <= (m_Raduis + other.m_Raduis))
								{
									returnValue = true;
								}
							}
							else if (glm::length(proVec) <= (bothLng + m_Raduis + other.m_Raduis))
							{
								glm::vec3 ThisE2 = (m_Position - (This_Dir * m_Length));
								glm::vec3 OtherE2 = (other.m_Position - (Other_Dir * other.m_Length));

								glm::vec3 shortOne, ShortTwo;

								float LngOne = glm::length(OtherCylEdge_Pos - CylEdge_Pos);
								float LngTwo = glm::length(OtherE2 - CylEdge_Pos);

								if (LngOne < LngTwo)
								{
									shortOne = OtherCylEdge_Pos - CylEdge_Pos;
								}
								else
								{
									shortOne = OtherE2 - CylEdge_Pos;
								}


								float LngThree = glm::length(OtherCylEdge_Pos - ThisE2);
								float LngFour = glm::length(OtherE2 - ThisE2);

								if (LngThree < LngFour)
								{
									ShortTwo = OtherCylEdge_Pos - ThisE2;
								}
								else
								{
									ShortTwo = OtherE2 - ThisE2;
								}

								if (glm::length(shortOne) < glm::length(ShortTwo))
								{
									if (glm::length(shortOne) <= (m_Raduis + other.m_Raduis))
									{
										returnValue = true;
									}
								}
								else
								{
									if (glm::length(ShortTwo) <= (m_Raduis + other.m_Raduis))
									{
										returnValue = true;
									}
								}



							}


						}




				}


			}



		}



		return returnValue;
	}

	bool SphereHitbox::ColTest(const SphereHitbox& other)
	{


		float thisLength = m_Length + m_Raduis;
		float otherLength = other.m_Length + other.m_Raduis;
		glm::vec3 TempPOVec = other.m_Position - m_Position;
		m_Temp_Calc_This_Dir = m_Direction;
		m_Temp_Calc_Other_Dir = other.m_Direction;
		bool returnValue = false;


		if (TempPOVec.length() <= (thisLength + otherLength))
		{




			for (uint8_t i = 0; i < 3; i++)
			{
				if (m_Temp_Calc_This_Dir[i] < std::pow(10, -5))
				{
					m_Temp_Calc_This_Dir[i] = 0;
				}
				if (m_Temp_Calc_Other_Dir[i] < std::pow(10, -5))
				{
					m_Temp_Calc_Other_Dir[i] = 0;
				}

			}

			m_Temp_Calc_This_Dir = glm::normalize(m_Temp_Calc_This_Dir);
			m_Temp_Calc_Other_Dir = glm::normalize(m_Temp_Calc_Other_Dir);



			float DotCheck = glm::dot(m_Temp_Calc_This_Dir, m_Temp_Calc_Other_Dir);


			//Par
			if (DotCheck == 1)
			{

				glm::vec3 CylEdge_Vec = m_Temp_Calc_This_Dir * m_Length;
				glm::vec3 CylEdge_Pos_One = (m_Position + CylEdge_Vec);
				glm::vec3 CylEdge_Pos_Two = (m_Position - CylEdge_Vec);


				glm::vec3 OtherCylEdge_Vec = m_Temp_Calc_Other_Dir * other.m_Length;
				glm::vec3 OtherCylEdge_Pos_One = other.m_Position + OtherCylEdge_Vec;
				glm::vec3 OtherCylEdge_Pos_Two = other.m_Position - OtherCylEdge_Vec;


				glm::vec3 proVec = ((glm::dot(TempPOVec, CylEdge_Vec) / glm::dot(CylEdge_Vec, CylEdge_Vec)) * CylEdge_Vec);


				float proVecLen = glm::length(proVec);
				float bothCylLng = m_Length + other.m_Length;
				float bothRadLng = m_Raduis + other.m_Raduis;

				if (proVecLen <= bothCylLng)
				{
					glm::vec3 LngBtwLin = other.m_Position - (proVec + m_Position);

					if (glm::length(LngBtwLin) <= (m_Raduis + other.m_Raduis))
					{
						returnValue = true;
					}
				}
				else if (proVecLen <= (bothCylLng + bothRadLng))
				{


					if (glm::dot(glm::normalize(CylEdge_Vec), glm::normalize(proVec)) == 1)
					{
						glm::vec3 Vec_O_E1 = CylEdge_Pos_One - other.m_Position;
						glm::vec3 OtherProVec_O_E1 = ((glm::dot(Vec_O_E1, m_Temp_Calc_Other_Dir) / glm::dot(m_Temp_Calc_Other_Dir, m_Temp_Calc_Other_Dir)) * m_Temp_Calc_Other_Dir);

							//Kommer nog aldrig att komma in i denna if
						if (glm::dot(glm::normalize(OtherProVec_O_E1), glm::normalize(OtherCylEdge_Vec)) == 1)
						{

							glm::vec3 ControlVec = OtherCylEdge_Pos_One - CylEdge_Pos_One;
							if (glm::length(ControlVec) <= m_Raduis + other.m_Raduis)
							{
								returnValue = true;
							}
						}
						else
						{

							glm::vec3 ControlVec = OtherCylEdge_Pos_Two - CylEdge_Pos_One;
							if (glm::length(ControlVec) <= m_Raduis + other.m_Raduis)
							{
								returnValue = true;
							}


						}
					}
					else
					{
						glm::vec3 Vec_O_E2 = CylEdge_Pos_Two - other.m_Position;
						glm::vec3 OtherProVec_O_E2 = ((glm::dot(Vec_O_E2, m_Temp_Calc_Other_Dir) / glm::dot(m_Temp_Calc_Other_Dir, m_Temp_Calc_Other_Dir)) * m_Temp_Calc_Other_Dir);


						if (glm::dot(glm::normalize(OtherProVec_O_E2), glm::normalize(OtherCylEdge_Vec)) == 1)
						{

							glm::vec3 ControlVec = OtherCylEdge_Pos_One - CylEdge_Pos_Two;
							if (glm::length(ControlVec) <= m_Raduis + other.m_Raduis)
							{
								returnValue = true;
							}
						}

						//Kommer nog inte in i denna else
						else
						{

							glm::vec3 ControlVec = OtherCylEdge_Pos_Two - CylEdge_Pos_Two;
							if (glm::length(ControlVec) <= m_Raduis + other.m_Raduis)
							{
								returnValue = true;
							}


						}
					}


					/*glm::vec3 ThisE2 = (m_Position - (m_Temp_Calc_This_Dir * m_Length));
					glm::vec3 OtherE2 = (other.m_Position - (m_Temp_Calc_Other_Dir * other.m_Length));

					glm::vec3 shortOne, ShortTwo;

					float LngOne = glm::length(OtherCylEdge_Pos - CylEdge_Pos);
					float LngTwo = glm::length(OtherE2 - CylEdge_Pos);

					if (LngOne < LngTwo)
					{
						shortOne = OtherCylEdge_Pos - CylEdge_Pos;
					}
					else
					{
						shortOne = OtherE2 - CylEdge_Pos;
					}


					float LngThree = glm::length(OtherCylEdge_Pos - ThisE2);
					float LngFour = glm::length(OtherE2 - ThisE2);

					if (LngThree < LngFour)
					{
						ShortTwo = OtherCylEdge_Pos - ThisE2;
					}
					else
					{
						ShortTwo = OtherE2 - ThisE2;
					}

					if (glm::length(shortOne) < glm::length(ShortTwo))
					{
						if (glm::length(shortOne) <= (m_Raduis + other.m_Raduis))
						{
							returnValue = true;
						}
					}
					else
					{
						if (glm::length(ShortTwo) <= (m_Raduis + other.m_Raduis))
						{
							returnValue = true;
						}
					}
*/


				}
			}
			//Orth
			//else if (DotCheck == 0)
			//{

			//	int i = 0;














			//}
			else
			{

			//Continue here... Non Par and Non Orth


			glm::vec3 OrthoVec = glm::cross(m_Temp_Calc_This_Dir, m_Temp_Calc_Other_Dir);

			glm::vec3 Final(0.0f);
			if (OrthoVec != glm::vec3(0.0f))
			{
				Final = ((glm::dot(TempPOVec, OrthoVec) / glm::dot(OrthoVec, OrthoVec)) * OrthoVec);
			}

			if (glm::length(Final) < (m_Raduis + other.m_Raduis))
			{

				glm::mat3 OneToTwo =
				{ Other_Dir.x, -This_Dir.x, Other_Dir.x - m_Position.x,
					Other_Dir.y, -This_Dir.y, Other_Dir.y - m_Position.y,
					Other_Dir.z, -This_Dir.z, Other_Dir.z - m_Position.z
				};

				glm::vec3 calcVecOne(0.0f, 0.0f, 0.0f);
				glm::vec3 calcVecTwo(0.0f, 0.0f, 0.0f);

				for (uint8_t i = 0; i < 3; i++)
				{

					calcVecOne[0] += OneToTwo[i][0] * This_Dir[i];
					calcVecOne[1] += OneToTwo[i][1] * This_Dir[i];
					calcVecOne[2] += OneToTwo[i][2] * This_Dir[i];

					calcVecTwo[0] += OneToTwo[i][0] * Other_Dir[i];
					calcVecTwo[1] += OneToTwo[i][1] * Other_Dir[i];
					calcVecTwo[2] += OneToTwo[i][2] * Other_Dir[i];
				}

				float s_Value;
				float t_Value;


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

					float sSubValue = calcVecOne.y * t_Value;

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
							m_Position.x + This_Dir.x * s_Value,
							m_Position.y + This_Dir.y * s_Value,
							m_Position.z + This_Dir.z * s_Value);

					float P_Pos_Length = glm::length((P_Pos - m_Position));

					glm::vec3 Q_Pos =
						glm::vec3(
							other.m_Position.x + Other_Dir.x * t_Value,
							other.m_Position.y + Other_Dir.y * t_Value,
							other.m_Position.z + Other_Dir.z * t_Value);

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
			}

		}
			return returnValue;
	}
	bool SphereHitbox::SphereCheck(const SphereHitbox& Other)
	{
		return false;
	}

}
