#ifndef FROSTY_HPP
#define FROSTY_HPP

#include <Frosty.h>
#include <iostream>
#include "imgui/imgui.h"
#include "Frosty/Core/Collision/CollisionDetection.hpp"
#include "ObjectHandler/Player/Player.hpp"
#include "ObjectHandler/GameObject.hpp"

class ExampleLayerA : public Frosty::Layer
{

private:
	Frosty::CollisionDetection m_CollisionDetection;
	Player m_Player;
	//Player m_Player2;
	//GameObject m_Ground = GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f));
	//GameObject m_Collidable1 = GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "tempPlayer");
	//GameObject m_Collidable2 = GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "tempPlayer");
	//GameObject m_Collidable3 = GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "tempPlayer");


	GameObject m_SphereOne = GameObject(glm::vec3(0.0f, 0.0f, 0.99f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "capsule_hitbox");
	GameObject m_SphereTwo = GameObject(glm::vec3(0.0f, 0.99f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "capsule_hitbox");

	Frosty::SphereHitbox Test1;
	Frosty::SphereHitbox Test2;




	float m_Temp_Rot = 0.0f;

public:
	ExampleLayerA()
		: Layer("Example")
	{

	}
	void OnAttach() override
	{


		//Test1.m_Position = glm::vec3(0.0f,0.0f,0.0f);
		//Test2.m_Position = glm::vec3(0.0f,4.0f,0.0f);

		//float rot = 180;

		//glm::mat4 rotateZ = glm::mat4(
		//	cos(glm::radians(rot)), -sin(glm::radians(rot)), 0.0f, 0.0f,
		//	sin(glm::radians(rot)), cos(glm::radians(rot)), 0.0f, 0.0f,
		//	0.0f, 0.0f, 1.0f, 0.0f,
		//	0.0f, 0.0f, 0.0f, 1.0f
		//);

		//glm::vec4 tempVec4 = rotateZ * glm::vec4(Test2.m_Direction,0.0f);

		//Test2.m_Direction = glm::normalize(glm::vec3(tempVec4.x,tempVec4.y,tempVec4.z));


		//bool jj = Test1.ColTest(Test2);

		//if (jj) 
		//{
		//	FY_CORE_INFO("TEST 1 OK");
		//}

		//Test2.m_Position = glm::vec3(0.0f, 4.1f, 0.0f);

		//jj = Test1.ColTest(Test2);

		//if (!jj)
		//{
		//	FY_CORE_INFO("TEST 2 OK");
		//}



		//Test2.m_Position = glm::vec3(0.1f, 4.0f, 0.0f);

		//jj = Test1.ColTest(Test2);

		//if (!jj)
		//{
		//	FY_CORE_INFO("TEST 3 OK");
		//}

		//Test2.m_Position = glm::vec3(2.0f, 0.0f, 0.0f);

		//jj = Test1.ColTest(Test2);

		//if (jj)
		//{
		//	FY_CORE_INFO("TEST 4 OK");
		//}


		//Test2.m_Position = glm::vec3(2.1f, 0.0f, 0.0f);

		//jj = Test1.ColTest(Test2);

		//if (!jj)
		//{
		//	FY_CORE_INFO("TEST 5 OK");
		//}


		//Test2.m_Position = glm::vec3(2.5f, 0.0f, 0.0f);

		//rot = 90;
		//rotateZ = glm::mat4(
		//	cos(glm::radians(rot)), -sin(glm::radians(rot)), 0.0f, 0.0f,
		//	sin(glm::radians(rot)), cos(glm::radians(rot)), 0.0f, 0.0f,
		//	0.0f, 0.0f, 1.0f, 0.0f,
		//	0.0f, 0.0f, 0.0f, 1.0f
		//);



		//tempVec4 = rotateZ * glm::vec4(Test2.m_Direction, 0.0f);

		//Test2.m_Direction = glm::normalize(glm::vec3(tempVec4.x, tempVec4.y, tempVec4.z));


		//jj = Test1.ColTest(Test2);

		//if (!jj)
		//{
		//	FY_CORE_INFO("TEST 6 OK");
		//}



		//bool gg = Test1.IsCollidingWith(Test2);


	}
	void OnUpdate() override
	{

		m_Player.UpdatePlayer();

			//if (m_CollisionDetection.AABBIntersect(m_Player.GetHitBoxLength(),
			//	m_Player.GetHitBoxCenter(), m_Collidable1.GetHitBoxLength(), m_Collidable1.GetHitBoxCenter()))
			//	FY_INFO("the player is in box1");
			//else
			//	FY_INFO("the player is not in box1");
			//m_Player.SetPos(m_Player.GetPos() - m_CollisionDetection.AABBIntersecPushback(m_Player.GetHitBoxLength(),
			//	m_Player.GetHitBoxCenter(), m_Collidable2.GetHitBoxLength(), m_Collidable2.GetHitBoxCenter()));
			//m_Player.SetPos(m_Player.GetPos() - m_CollisionDetection.AABBIntersecPushback(m_Player.GetHitBoxLength(),
			//	m_Player.GetHitBoxCenter(), m_Collidable3.GetHitBoxLength(), m_Collidable3.GetHitBoxCenter()));

			m_SphereOne.SetPos(glm::vec3(1.5f,0.0f,0.0f));
			m_SphereOne.SetRotation(glm::vec3(0.0f, 0.0f,m_Temp_Rot));
			m_SphereOne.UpdateWorldMatrix();
			
			m_SphereTwo.SetPos(glm::vec3(-1.5f,0.0f,0.0f));
			//m_SphereTwo.SetRotation(glm::vec3(0.0f, 0.0f, -m_Temp_Rot));
			//m_SphereTwo.UpdateWorldMatrix();



			if (m_SphereOne.GetSphereHitbox().ColTest(m_SphereTwo.GetSphereHitbox()))
			{
				m_SphereOne.SetColour(glm::vec3(1.0f, 0.0f, 0.0f));
			}
			else
			{
				m_SphereOne.SetColour(glm::vec3(0.0f, 1.0f, 0.0f));
			}

			//Test1.m_Position = m_SphereOne.GetPos();
			//Test2.m_Position = m_SphereTwo.GetPos();

			//Test1.m_Direction = glm::normalize(glm::vec3((Test1.m_Direction)));

			//if (Test1.ColTest(Test2))
			//{
			//	m_SphereOne.SetColour(glm::vec3(1.0f, 0.0f, 0.0f));
			//}
			//else
			//{
			//	m_SphereOne.SetColour(glm::vec3(0.0f, 1.0f, 0.0f));
			//}


			m_Temp_Rot += 0.5f;


	}
	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	bool OnEvent(Frosty::BaseEvent& event) override
	{
		FY_TRACE("ExampleLayerA event handled!");
		//FY_TRACE("{0}", event);
		return true;
	}
};

class Game : public Frosty::Application
{
public:
	Game();
	virtual ~Game();

};

#endif // !FROSTY_HPP
