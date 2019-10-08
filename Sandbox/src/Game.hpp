#ifndef FROSTY_HPP
#define FROSTY_HPP

#include <Frosty.h>
#include <iostream>
#include "imgui/imgui.h"
#include "Frosty/Core/CollisionDetection.hpp"
#include "ObjectHandler/Player/Player.hpp"
#include "ObjectHandler/GameObject.hpp"

class ExampleLayerA : public Frosty::Layer
{

private:
	Frosty::CollisionDetection m_CollisionDetection;
	Player m_Player;
	//Player m_Player2;
	GameObject m_Ground = GameObject(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f));
	GameObject m_Collidable1 = GameObject(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 10, "tempPlayer");
	GameObject m_Collidable2 = GameObject(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 10, "tempPlayer");
	GameObject m_Collidable3 = GameObject(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 10, "tempPlayer");

	Frosty::Attacks m_Attacks;
	Frosty::PlayerController m_GameInput;
	//temp
	int m_EnemyDamage = 1;
	int m_PlayerDamage = 1;

	//Frosty::PrefabInstance* m_testInstance;
	std::vector< Frosty::PrefabInstance*> m_Instances;
	
	std::vector< float> m_Rotation;
	
	bool phase2 = false;

public:
	ExampleLayerA()
		: Layer("Example")
	{

	}
	void OnAttach() override
	{
		Frosty::PrefabManager::GetPrefabManager()->setPrefab("TestPrefab1", "clock", "Mat_0:table");
		//m_testInstance = Frosty::PrefabManager::GetPrefabManager()->CreatePrefabInstance("TestPrefab1");
		//m_testInstance->GetTransform()->setTranslate(glm::vec3(10, 5, 0));

		int i = 0;
			for (uint32_t x = 0; x < 5; x++)
			{
				for (uint32_t y = 0; y < 5; y++)
				{
					for (uint32_t z = 0; z < 5; z++)
					{
						m_Instances.emplace_back(Frosty::PrefabManager::GetPrefabManager()->CreatePrefabInstance("TestPrefab1"));
						m_Instances.at(i)->GetTransform()->setTranslate(glm::vec3(10 * x, 10 *y, 10 * z));
						m_Rotation.emplace_back(0.0f);
	
						i++;
					}
				}
			}
			
		


	}
	void OnUpdate() override
	{
		//Frosty::PrefabManager::GetPrefabManager()->setPrefab("TestPrefab1", "clock", "Mat_0:clock");

		float dt = Frosty::Time::DeltaTime();
		int frame = Frosty::Time::GetFrameCount();
		
		if (frame % (50*30) == 0)
		{
			if (m_Instances.size() > 0 && phase2 == false)
			{
				m_Instances.at(0)->Destroy();
				m_Instances.erase(m_Instances.begin());
				

			}
			
			
			
			
		}

		if (frame % 1000 == 0 )
		{
			Frosty::PrefabManager::GetPrefabManager()->setPrefab("TestPrefab1", "clock", "Mat_0:clock");
		}
		else if (frame % 750 == 0)
		{
			Frosty::PrefabManager::GetPrefabManager()->setPrefab("TestPrefab1", "clock", "Mat_0:table");
		}
		else if (frame % 500 == 0)
		{
			Frosty::PrefabManager::GetPrefabManager()->setPrefab("TestPrefab1", "table", "Mat_0:table");
		}
		else if (frame % 250 == 0)
		{
			Frosty::PrefabManager::GetPrefabManager()->setPrefab("TestPrefab1", "table", "Mat_0:clock");
		}
		
		for (uint32_t i = 0; i < m_Instances.size(); i++)
		{
			m_Instances.at(i)->GetTransform()->setRotate(glm::vec3(0, 0, m_Rotation.at(i) -= 40 * Frosty::Time::DeltaTime()));
		}

		//m_testInstance->GetTransform()->setTranslate(glm::vec3(m_testTranslateX += 0.01, 5, 0));
		glm::vec3 test = Frosty::PrefabManager::GetPrefabManager()->GetPrefab("TestPrefab1")->GetInstances()->at(0)->GetTransform()->getTranslate();

		m_Attacks = m_GameInput.PlayerControllerAttacks();

		m_Player.UpdatePlayer();

		if (m_CollisionDetection.AABBIntersect(m_Player.GetHitBoxLength(),
			m_Player.GetHitBoxCenter(), m_Collidable1.GetHitBoxLength(), m_Collidable1.GetHitBoxCenter()))
			m_Collidable1.SetShouldRender(false);
		else if (m_Collidable1.GetHp() > 0)
			m_Collidable1.SetShouldRender(true);
		m_Player.SetPos(m_Player.GetPos() - m_CollisionDetection.AABBIntersecPushback(m_Player.GetHitBoxLength(),
			m_Player.GetHitBoxCenter(), m_Collidable2.GetHitBoxLength(), m_Collidable2.GetHitBoxCenter()));
		m_Player.SetPos(m_Player.GetPos() - m_CollisionDetection.AABBIntersecPushback(m_Player.GetHitBoxLength(),
			m_Player.GetHitBoxCenter(), m_Collidable3.GetHitBoxLength(), m_Collidable3.GetHitBoxCenter()));

		if (m_Attacks.enemyCanAttack)
			m_Player.SetHp(m_Player.GetHp() - m_EnemyDamage);
		if (m_Attacks.playerCanAttack)
			m_Collidable2.SetHp(m_Collidable2.GetHp() - m_PlayerDamage);
		m_Collidable2.UpdateHp();
		m_Player.UpdateHp();

		m_Player.UpdateWorldMatrix();

	}
	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	bool OnEvent(Frosty::BaseEvent& event) override
	{
		//FY_TRACE("ExampleLayerA event handled!");
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