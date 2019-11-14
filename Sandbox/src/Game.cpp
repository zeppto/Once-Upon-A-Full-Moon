#include <mcspch.hpp>
#include "Game.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"

#include "Systems/LightSystem.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/PlayerControllerSystem.hpp"
#include "Systems/FollowSystem.hpp"
#include "Systems/BasicAttackSystem.hpp"
#include "Systems/CombatSystem.hpp"
#include "Systems/DestroySystem.hpp"
#include "Systems/ArrowSystem.hpp"
#include "Systems/SwordSystem.hpp"
#include "Systems/NavigationSystem.hpp"
#include "Systems/ParticleSystem.hpp"
#include "Frosty/Core/BoolMapGenerator/TestMap.hpp"
//#include "Systems/PlayerAttackSystem.hpp"
//#include "Systems/EnemyAttackSystem.hpp"
//#include "Systems/HealthBarSystem.hpp"



namespace MCS
{
	Game::Game()
	{

		auto& world = Application::Get().GetWorld();
		//std::srand((unsigned)std::time(0)); //We already seed in main. We shouldn't seed twice
		// Add systems
		world->AddSystem<MCS::CameraSystem>();
		world->AddSystem<LightSystem>();
		world->AddSystem<RenderSystem>();
		world->AddSystem<PlayerControllerSystem>();
		world->AddSystem<PhysicsSystem>();
		world->AddSystem<FollowSystem>();
		world->AddSystem<BasicAttackSystem>();
		world->AddSystem<CombatSystem>();
		world->AddSystem<DestroySystem>();
		world->AddSystem<ArrowSystem>();
		world->AddSystem<SwordSystem>();
		Frosty::ECS::BaseSystem* retSystem = world->AddSystem<NavigationSystem>();
		NavigationSystem* navSystem = dynamic_cast<NavigationSystem*>(retSystem);
		//world->AddSystem<CollisionSystem>();		// Have this after MovementSystem
		//world->AddSystem<DashSystem>();
		//world->AddSystem<PlayerAttackSystem>();
		//world->AddSystem<EnemyAttackSystem>();
		//world->AddSystem<SpawnSystem>();
		//world->AddSystem<HealthBarSystem>();
		world->AddSystem<ParticleSystem>();

		world->Awake();



		
		auto& plane = world->CreateEntity({ 64.0f, 0.0f, 36.0f }, { 0.0f, 0.0f, 0.0f }, { 128.0f, 1.0f, 72.0f });
		world->AddComponent<Frosty::ECS::CMesh>(plane, Frosty::AssetManager::GetMesh("pPlane1"));
		auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(plane, Frosty::AssetManager::GetShader("Texture2D"));
		planeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("heartFull");
		//planeMat.BlendMapTexture = Frosty::AssetManager::GetTexture2D("blendMap_Test");	// why is this texture a problem
		//planeMat.BlendTexture1 = Frosty::AssetManager::GetTexture2D("StoneGround");
		//planeMat.BlendTexture2 = Frosty::AssetManager::GetTexture2D("Dirt");
		navSystem->InitiateGridMap(world->GetComponent<Frosty::ECS::CTransform>(plane));

		//auto& testEntity = world->CreateEntity({ -47.5f, 0.01f, 47.5f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 1.0f, 5.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(testEntity, Frosty::AssetManager::GetMesh("pPlane1"));
		//world->AddComponent<Frosty::ECS::CMaterial>(testEntity, Frosty::AssetManager::GetShader("FlatColor"));

		// Night Light
		auto& light = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
		world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.6f, 0.7f, 1.f));

		// Night Light (makeshift reflection)
		auto & light2 = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { -190.0f, 8.0f, -10.0f });
		auto & lightTransform2 = world->GetComponent<Frosty::ECS::CTransform>(light2);
		world->AddComponent<Frosty::ECS::CLight>(light2, Frosty::ECS::CLight::LightType::Directional, 0.3f, glm::vec3(0.6f, 0.7f, 1.f));

		auto & player = world->CreateEntity();
		auto & playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		playerTransform.Scale = glm::vec3(2.0f, 2.0f, 2.0f);
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("scarlet"));
		auto & playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Texture2D"));
		playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		world->AddComponent<Frosty::ECS::CPlayer>(player);
		world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("scarlet"), 10.0f);
		world->AddComponent<Frosty::ECS::CDash>(player);
		world->AddComponent<Frosty::ECS::CBasicAttack>(player, Frosty::ECS::CBasicAttack::AttackType::Range);
		world->AddComponent<Frosty::ECS::CHealth>(player);
		world->AddComponent<Frosty::ECS::CInventory>(player);
		auto & camEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;

		auto & wall = world->CreateEntity({ 0.0f, 5.0f, -3.0f }, { 0.0f, 0.0f, 0.0f }, { 15.0f, 10.0f, 1.0f });
		world->AddComponent<Frosty::ECS::CMesh>(wall, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(wall, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(wall, Frosty::AssetManager::GetBoundingBox("pCube1"));
		
		auto& tree = world->CreateEntity({ 33.0f, 0.0f, 36.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
		world->AddComponent<Frosty::ECS::CMesh>(tree, Frosty::AssetManager::GetMesh("tree1"));
		auto & treeMat = world->AddComponent<Frosty::ECS::CMaterial>(tree, Frosty::AssetManager::GetShader("Texture2D"));
		treeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree1");
		world->AddComponent<Frosty::ECS::CPhysics>(tree, Frosty::AssetManager::GetBoundingBox("tree1"));

		auto & enemy = world->CreateEntity({ 27.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(enemy, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(enemy, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(enemy, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CFollow>(enemy, &playerTransform);
		world->AddComponent<Frosty::ECS::CHealth>(enemy);








		Frosty::NodeMap testMap(100, 100);
		testMap.GenereateMap();


		std::vector<Frosty::Node>  nodeMap = testMap.GetNodes();

		float GridMult = 1.0f;

		for (int i = 0; i < nodeMap.size(); i++)
		{
			float tempX = nodeMap.at(i).GetX() * GridMult;
			float tempY = nodeMap.at(i).GetY() * GridMult;

			const bool* exits = nodeMap.at(i).GetExits();

			float xOffset = 5.0f;
			float yOffset = 5.0f;
			float scaleX = 0.05f;
			float scaleY = 0.05f;




			float PosX = tempX/10.0f ;
			float PosY = tempY/10.0f ;

			auto& Node = world->CreateEntity({ PosX, 10.0f, PosY }, { 0.0f, 0.0f, 0.0f }, { scaleX, 0.1f, scaleY });
			world->AddComponent<Frosty::ECS::CMesh>(Node, Frosty::AssetManager::GetMesh("pCube1"));
			world->AddComponent<Frosty::ECS::CMaterial>(Node, Frosty::AssetManager::GetShader("FlatColor"));



			for (int j = 0; j < 4; j++)
			{
				xOffset = 0.05f;
				yOffset = 0.05f;
				if (exits[j])
				{

					float ofPosX = PosX;
					float ofPosY = PosY;

					if (j == 0)
					{
						ofPosY -= yOffset;
					}
					else if (j == 1)
					{
						ofPosX += xOffset;
					}
					else if (j == 2)
					{
						ofPosX -= xOffset;
					}
					else if (j == 3)
					{
						ofPosY += yOffset;
					}
					/*			scaleY = 2.0f;
									scaleX = 2.0f;*/






					auto& Node = world->CreateEntity({ ofPosX, 10.0f, ofPosY }, { 0.0f, 0.0f, 0.0f }, { scaleX/3, 0.05f/3, scaleY/3 });
					world->AddComponent<Frosty::ECS::CMesh>(Node, Frosty::AssetManager::GetMesh("pSphere1"));
					world->AddComponent<Frosty::ECS::CMaterial>(Node, Frosty::AssetManager::GetShader("FlatColor"));

					

				}

			}

		}








		PushLayer(FY_NEW InspectorLayer());
	}

	Game::~Game()
	{

	}
}