#include <mcspch.hpp>
#include "Game.hpp"
#include "Frosty/API/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"

#include "Systems/CombatSystem.hpp"
#include "Systems/EnemyAttackSystem.hpp"
#include "Systems/ConsumablesSystem.hpp"
#include "Systems/HealthBarSystem.hpp"
#include "Systems/LightSystem.hpp"
#include "Systems/SpawnSystem.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/FollowSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/PlayerControllerSystem.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/PlayerAttackSystem.hpp"
#include "LevelHandeler/MapGenerator.hpp"

namespace MCS
{
	void generateTrees();
	void generateBorders();
	void generatePlanes();
	void generateMapMarkers();
	void generateLights();
	float randValue(int max, int min);

	Game::Game()
	{

		auto& world = Application::Get().GetWorld();
		std::srand((unsigned)std::time(0)); //We already seed in main. We shouldn't seed twice
		// Add systems
		world->AddSystem<CameraSystem>();
		world->AddSystem<LightSystem>();
		world->AddSystem<RenderSystem>();
		world->AddSystem<PlayerControllerSystem>();
		world->AddSystem<MovementSystem>();
		world->AddSystem<FollowSystem>();
		world->AddSystem<CollisionSystem>();
		world->AddSystem<PlayerAttackSystem>();
		world->AddSystem<EnemyAttackSystem>();
		world->AddSystem<SpawnSystem>();
		world->AddSystem<ConsumablesSystem>();
		world->AddSystem<CombatSystem>();
		world->AddSystem<HealthBarSystem>();

		// Add components

		//world->InitiateComponent<Frosty::ECS::CTransform>();
		//world->InitiateComponent<Frosty::ECS::CCamera>();
		world->Start();

		MapGenerator map;
		map.generateMap();
		Room myRoome = map.getRoom(glm::ivec2(10, 15));

		//for (int i = 0; i < 30; i++)
		//{
		//	for (int j = 0; j < 30; j++)
		//	{
		//		auto& cube = world->CreateEntity();
		//		auto& cubeTranform = world->GetComponent<Frosty::ECS::CTransform>(cube);
		//		cubeTranform.Position = glm::vec3(i,2,j);
		//		auto& cubeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(cube, Frosty::AssetManager::GetShader("Texture2D"));
		//		Room myRoome = map.getRoom(glm::ivec2(i, j));
		//		int rotation = 0;
		//		//cubeMaterial.Albedo = glm::vec4( 0.1f, 0.8f, 0.1f, 1.0f);
		//		cubeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(map.getRoomTextur(glm::ivec2(i,j), &rotation));
		//		cubeTranform.Rotation = glm::vec3(0, rotation, 0);
		//		if (i == 10 && j == 15)
		//			cubeTranform.Scale = glm::vec3(0.5f, 0.5f, 0.5f);
		//		//std::string cubeName = "cube" + std::to_string(((rand() % NrOfcubeTypes) + 1));
		//		//cubeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(cubeName);

		//		//cubeName = "cube" + std::to_string(((rand() % NrOfcubeTypes) + 1));
		//		world->AddComponent<Frosty::ECS::CMesh>(cube, Frosty::AssetManager::GetMesh("Plane"));
		//	}
		//}


		//auto& plane = world->CreateEntity();
		//auto& planeTransform = world->GetComponent<Frosty::ECS::CTransform>(plane);
		//planeTransform.Scale = glm::vec3(100.0f, 1.0f, 100.0f);
		//world->AddComponent<Frosty::ECS::CMesh>(plane, Frosty::AssetManager::GetMesh("Plane"));
		//auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(plane, Frosty::AssetManager::GetShader("Texture2D"));
		//planeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Brown Mud Diffuse");
		//planeMat.TextureScale = 10.0f;
		
		//auto& light = world->CreateEntity();
		//auto& lightTransform = world->GetComponent<Frosty::ECS::CTransform>(light);
		//lightTransform.Position = glm::vec3(0.0f, 30.0f, -6.0f);
		//lightTransform.Rotation = glm::vec3(180.0f, 0.0f, 10.0f);
		////world->AddComponent<Frosty::ECS::CMesh>(light, Frosty::AssetManager::GetMesh("Cube"));
		////world->AddComponent<Frosty::ECS::CMaterial>(light, Frosty::AssetManager::GetShader("FlatColor"));
		//world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Point, 0.6f, 200.0f);

		// Night Light
		auto& light = world->CreateEntity();
		auto& lightTransform = world->GetComponent<Frosty::ECS::CTransform>(light);
		lightTransform.Rotation = glm::vec3(120.f, 8.f, -10.f);
		world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.6f, 0.7f, 1.f));

		// Night Light (makeshift reflection)
		auto& light2 = world->CreateEntity();
		auto& lightTransform2 = world->GetComponent<Frosty::ECS::CTransform>(light2);
		lightTransform2.Rotation = glm::vec3(-190.f, 8.f, -10.f);
		world->AddComponent<Frosty::ECS::CLight>(light2, Frosty::ECS::CLight::LightType::Directional, 0.3f, glm::vec3(0.6f, 0.7f, 1.f));

		//generateTrees();
		//generateBorders();
		//generatePlanes();

		PushLayer(FY_NEW InspectorLayer());
	}

	Game::~Game()
	{

	}

	void generateTrees()
	{

		uint8_t mapLength = 192;
		uint8_t mapDepth = 108;

		
		//srand(time(NULL));

		auto& world = Frosty::Application::Get().GetWorld();

		float Tree_Y_Offset = 0.0f;
		int NrOfTreeTypes = 10;

		int randOffsetMax = 5;
		int randOffsetMin = -5;

		//Top Trees

		for (int i = 0; i < 25; i++)
		{
			auto& Tree = world->CreateEntity();
			auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
			TreeTranform.Position = glm::vec3(-(mapLength / 2) + (((mapLength * 2) / 24) * i), Tree_Y_Offset, (mapDepth / 2) + randValue(randOffsetMax, randOffsetMin));
			auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
			TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
			std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(treeName);

			treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh(treeName));
		}



		//Bot Trees

		for (int i = 0; i < 25; i++)
		{
			auto& Tree = world->CreateEntity();
			auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
			TreeTranform.Position = glm::vec3(-(mapLength / 2) + (((mapLength * 2) / 24) * i), Tree_Y_Offset, (-mapDepth / 2) + randValue(randOffsetMax, randOffsetMin));
			auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
			TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
			std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(treeName);

			treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh(treeName));
		}


		//Left Trees

		for (int i = 1; i < 6; i++)
		{
			auto& Tree = world->CreateEntity();
			auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
			TreeTranform.Position = glm::vec3(-(mapLength / 2) + randValue(randOffsetMax, randOffsetMin), Tree_Y_Offset, (-mapDepth / 2 + (mapDepth / 6) * i));
			auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
			TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
			std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(treeName);

			treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh(treeName));
		}



		//Right Trees

		for (int i = 1; i < 6; i++)
		{
			auto& Tree = world->CreateEntity();
			auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
			TreeTranform.Position = glm::vec3((mapLength * 1.5) + randValue(randOffsetMax, randOffsetMin), Tree_Y_Offset, (-mapDepth / 2 + (mapDepth / 6) * i));
			auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
			TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
			std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(treeName);

			treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh(treeName));
		}



		//Middle Trees

		for (int i = 1; i < 3; i++)
		{
			auto& Tree = world->CreateEntity();
			auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
			TreeTranform.Position = glm::vec3((mapLength / 2) + randValue(randOffsetMax, randOffsetMin), Tree_Y_Offset, (-mapDepth / 2 + (mapDepth / 6) * i));
			auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
			TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
			std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(treeName);

			treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh(treeName));
		}

		for (int i = 1; i < 3; i++)
		{
			auto& Tree = world->CreateEntity();
			auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
			TreeTranform.Position = glm::vec3((mapLength / 2) + randValue(randOffsetMax, randOffsetMin), Tree_Y_Offset, (mapDepth / 6) * i);
			auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
			TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
			std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(treeName);

			treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
			world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh(treeName));
		}



	}

	void generateBorders()
	{
		bool showHB = 0;
		auto& world = Frosty::Application::Get().GetWorld();
		uint8_t mapLength = 192;
		uint8_t mapDepth = 108;

		float HB_Y_pos = 1.0f;
		float HB_Y_scale = 100.0f;
		float middleWidth = 0.5f;
		float middleDepth = 5.0f;
		float HitboxMapInflunce = 0.15f;


		//Bot Hitbox
		auto& BorderBox_1 = world->CreateEntity();
		auto& BorderTransform_1 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_1);
		BorderTransform_1.Position = glm::vec3(mapLength / 2, 1.0f, mapDepth / 2);
		BorderTransform_1.Scale = glm::vec3(mapLength * 2, HB_Y_scale, mapDepth * HitboxMapInflunce);


		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_1, 0.0f);
		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_1, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_1, "Border");


		//Top Hitbox
		auto& BorderBox_2 = world->CreateEntity();
		auto& BorderTransform_2 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_2);
		BorderTransform_2.Position = glm::vec3(mapLength / 2, 1.0f, -mapDepth / 2);
		BorderTransform_2.Scale = glm::vec3(mapLength * 2, HB_Y_scale, mapDepth * HitboxMapInflunce);


		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_2, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_2, 0.0f);
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_2, "Border");

		//Left Hitbox
		auto& BorderBox_3 = world->CreateEntity();
		auto& BorderTransform_3 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_3);
		BorderTransform_3.Position = glm::vec3(-mapLength / 2, 1.0f, 0.0);
		BorderTransform_3.Scale = glm::vec3(mapLength * HitboxMapInflunce, HB_Y_scale, mapDepth);

		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_3, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_3, 0.0f);
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_3, "Border");


		//Right Hitbox
		auto& BorderBox_4 = world->CreateEntity();
		auto& BorderTransform_4 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_4);
		BorderTransform_4.Position = glm::vec3(mapLength * 1.5, 1.0f, 0.0);
		BorderTransform_4.Scale = glm::vec3(mapLength * HitboxMapInflunce, HB_Y_scale, mapDepth);


		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_4, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_4, 0.0f);
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_4, "Border");



		//Middle Top Hitbox
		auto& BorderBox_5 = world->CreateEntity();
		auto& BorderTransform_5 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_5);
		BorderTransform_5.Position = glm::vec3(mapLength / 2, 1.0f, mapDepth / 2);
		BorderTransform_5.Scale = glm::vec3(mapLength * HitboxMapInflunce * middleWidth, HB_Y_scale, mapDepth * HitboxMapInflunce * middleDepth);

		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_5, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_5, 0.0f);
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_5, "Border");


		//Middle Bot Hitbox
		auto& BorderBox_6 = world->CreateEntity();
		auto& BorderTransform_6 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_6);
		BorderTransform_6.Position = glm::vec3(mapLength / 2, 1.0f, -mapDepth / 2);
		BorderTransform_6.Scale = glm::vec3(mapLength * HitboxMapInflunce * middleWidth, HB_Y_scale, mapDepth * HitboxMapInflunce * middleDepth);


		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_6, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_6, 0.0f);
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_6, "Border");



		if (showHB)
		{
				world->AddComponent<Frosty::ECS::CMesh>(BorderBox_6, Frosty::AssetManager::GetMesh("Cube"));
		auto& BorderBoxMaterial_6 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_6, Frosty::AssetManager::GetShader("FlatColor"));
		BorderBoxMaterial_6.Albedo = glm::vec4(0.3f, 0.4f, 0.8f, 1.0f);


		world->AddComponent<Frosty::ECS::CMesh>(BorderBox_5, Frosty::AssetManager::GetMesh("Cube"));
		auto& BorderBoxMaterial_5 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_5, Frosty::AssetManager::GetShader("FlatColor"));
		BorderBoxMaterial_5.Albedo = glm::vec4(0.3f, 0.4f, 0.8f, 1.0f);

					world->AddComponent<Frosty::ECS::CMesh>(BorderBox_4, Frosty::AssetManager::GetMesh("Cube"));
		auto& BorderBoxMaterial_4 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_4, Frosty::AssetManager::GetShader("FlatColor"));
		BorderBoxMaterial_4.Albedo = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);


		world->AddComponent<Frosty::ECS::CMesh>(BorderBox_3, Frosty::AssetManager::GetMesh("Cube"));
		auto& BorderBoxMaterial_3 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_3, Frosty::AssetManager::GetShader("FlatColor"));
		BorderBoxMaterial_3.Albedo = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);


					world->AddComponent<Frosty::ECS::CMesh>(BorderBox_2, Frosty::AssetManager::GetMesh("Cube"));
		auto& BorderBoxMaterial_2 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_2, Frosty::AssetManager::GetShader("FlatColor"));
		BorderBoxMaterial_2.Albedo = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

					world->AddComponent<Frosty::ECS::CMesh>(BorderBox_1, Frosty::AssetManager::GetMesh("Cube"));
		auto& BorderBoxMaterial_1 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_1, Frosty::AssetManager::GetShader("FlatColor"));
		BorderBoxMaterial_1.Albedo = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}



	}

	void generatePlanes()
	{
		auto& world = Frosty::Application::Get().GetWorld();

		uint8_t mapLength = 240;
		uint8_t mapDepth = 200;

		//Planes

		auto& PlaneOne = world->CreateEntity();
		auto& testTranform = world->GetComponent<Frosty::ECS::CTransform>(PlaneOne);
		testTranform.Position.x = -24.0f;
		testTranform.Scale = glm::vec3(mapLength, 1.0f, mapDepth);
		world->AddComponent<Frosty::ECS::CMesh>(PlaneOne, Frosty::AssetManager::GetMesh("Plane"));
		auto& testMaterial = world->AddComponent<Frosty::ECS::CMaterial>(PlaneOne, Frosty::AssetManager::GetShader("Texture2D"));
		testMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
		testMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Brown Mud Diffuse");
		testMaterial.TextureScale = glm::vec2(mapDepth / 10.0f, mapLength / 10.0f);
		//testMaterial.TextureScale = glm::vec2(23.0f, 20.0f);




		auto& PlaneTwo = world->CreateEntity();
		auto& testTranformTwo = world->GetComponent<Frosty::ECS::CTransform>(PlaneTwo);
		testTranformTwo.Scale = glm::vec3(mapLength, 1.0f, mapDepth);
		testTranformTwo.Position = glm::vec3(192.0f, 0.0f, 0.0f);
		world->AddComponent<Frosty::ECS::CMesh>(PlaneTwo, Frosty::AssetManager::GetMesh("Plane"));
		auto& testMaterialTwo = world->AddComponent<Frosty::ECS::CMaterial>(PlaneTwo, Frosty::AssetManager::GetShader("Texture2D"));
		testMaterialTwo.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
		testMaterialTwo.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Brown Mud Diffuse");
		testMaterialTwo.NormalTexture = Frosty::AssetManager::GetTexture2D("Rusty Metal Normal");
		testMaterialTwo.SpecularTexture = Frosty::AssetManager::GetTexture2D("Rusty Metal Normal");
		testMaterialTwo.TextureScale = glm::vec2(mapDepth / 10.0f, mapLength / 10.0f);





		auto& PlaneThree = world->CreateEntity();
		auto& testTranformThree = world->GetComponent<Frosty::ECS::CTransform>(PlaneThree);
		testTranformThree.Position = glm::vec3(0.0f, -0.10f, 0.0f);
		testTranformThree.Scale = glm::vec3(1000.0f, 1.0f, 1000.0f);
		world->AddComponent<Frosty::ECS::CMesh>(PlaneThree, Frosty::AssetManager::GetMesh("Plane"));
		auto& testMaterialThree = world->AddComponent<Frosty::ECS::CMaterial>(PlaneThree, Frosty::AssetManager::GetShader("Texture2D"));
		//testMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
		testMaterialThree.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Brown Mud Diffuse");


	}

	void generateMapMarkers()
	{

		//uint8_t mapLength = 192;
		//uint8_t mapDepth = 108;

		//float startDebugBoxPosX = -mapLength / 2.0f;
		//float startDebugBoxPosZ = -mapDepth / 2.0f;


		//Map Markers
		//for (int i = 0; i < 3; i++)
		//{

		//	for (int j = 0; j < 3; j++)
		//	{
		//		auto& DebugBox = world->CreateEntity();
		//		auto& DebugBoxTranform = world->GetComponent<Frosty::ECS::CTransform>(DebugBox);
		//		DebugBoxTranform.Scale = glm::vec3(1.0f, 10.0f, 1.0f);
		//		DebugBoxTranform.Position = glm::vec3(startDebugBoxPosX + mapLength * j, 0.0f, startDebugBoxPosZ + ((mapDepth / 2) * i));
		//		world->AddComponent<Frosty::ECS::CMesh>(DebugBox, Frosty::AssetManager::GetMesh("Cube"));
		//		auto& DebugBoxMaterial = world->AddComponent<Frosty::ECS::CMaterial>(DebugBox, Frosty::AssetManager::GetShader("FlatColor"));
		//		DebugBoxMaterial.Albedo = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		//	}
		//}



	}

	void generateLights()
	{
		uint8_t mapLength = 192;
		uint8_t mapDepth = 108;

		//float Light_Y_Offset = 3.0f;




		//Lights 
		//for (int i = 0; i < 3;i++)
		//{

		//	for (int j = 0; j < 8; j++)
		//	{
		//		auto& Light_1 = world->CreateEntity();
		//		auto& Light_1_Transform = world->GetComponent<Frosty::ECS::CTransform>(Light_1);
		//		Light_1_Transform.Position = glm::vec3(-(mapLength / 2) + (mapLength/8) * j, 0.0f, -(mapDepth / 2) + ((mapDepth / 2)* i ));
		//		//world->AddComponent<Frosty::ECS::CMesh>(light, Frosty::AssetManager::GetMesh("Cube"));
		//		//world->AddComponent<Frosty::ECS::CMaterial>(light, Frosty::AssetManager::GetShader("FlatColor"));
		//		world->AddComponent<Frosty::ECS::CLight>(Light_1, Frosty::ECS::CLight::LightType::Point, 200.0f, 20.0f);
		//	}
		//}

	}

	float randValue(int max, int min)
	{
		int dif = max - min;

		float returnValue = std::rand() % dif;

		returnValue += min;

		return returnValue;
	}
}