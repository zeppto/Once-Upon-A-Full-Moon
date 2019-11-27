#include <mcspch.hpp>
//#include "MapGenerator.hpp"
#include "LevelsHardCoded.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"

namespace MCS
{
	namespace Level
	{
		glm::vec3 Room(bool up, bool down, bool right, bool left, std::string texture, int rotation, int entrensSide, bool onlyPos)
		{
			glm::vec3 toReturn = glm::vec3(0);
			int NrOfTreeTypes = 10;

			auto& world = Frosty::Application::Get().GetWorld();
			auto& PlaneOne = world->CreateEntity();
			auto& testTranform = world->GetComponent<Frosty::ECS::CTransform>(PlaneOne);
			testTranform.Scale = glm::vec3(300, 1.0f, 300);
			testTranform.Rotation = glm::vec3(0, rotation, 0);
			world->AddComponent<Frosty::ECS::CMesh>(PlaneOne, Frosty::AssetManager::GetMesh("tiledGround"));
			//world->AddComponent<Frosty::ECS::CMesh>(PlaneOne, Frosty::AssetManager::GetMesh("pPlane1"));
			auto& testMaterial = world->AddComponent<Frosty::ECS::CMaterial>(PlaneOne, Frosty::AssetManager::GetShader("Texture2D"));
			//testMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
			//testMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(texture);
			testMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("brown_mud_diffuse");
			//testMaterial.TextureScale = glm::vec2(mapDepth / 10.0f, mapLength / 10.0f);

			for (int i = 0; i < 8; i++)
			{
				auto& Tree = world->CreateEntity();
				auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
				TreeTranform.Rotation = glm::vec3(0, 90 * (i / 2), 0);
				TreeTranform.Position = BorderTrees(i);
				auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
				TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
				std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
				//Tree7 is my fave
				TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");

				world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh("treeBunchWall"));
			}
			for (int i = 0; i < 4; i++)
			{

				auto& Tree = world->CreateEntity();
				auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
				TreeTranform.Position = BorderTrees(i * 4 + 8);
				auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
				TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
				std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
				//Tree7 is my fave
				TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");

				world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh("treeBunch4"));

				auto& Tree2 = world->CreateEntity();
				auto& Tree2Tranform = world->GetComponent<Frosty::ECS::CTransform>(Tree2);
				Tree2Tranform.Position = BorderTrees(i * 4 + 9);
				auto& Tree2Material = world->AddComponent<Frosty::ECS::CMaterial>(Tree2, Frosty::AssetManager::GetShader("Texture2D"));
				Tree2Material.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
				treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
				//Tree7 is my fave
				Tree2Material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");

				world->AddComponent<Frosty::ECS::CMesh>(Tree2, Frosty::AssetManager::GetMesh("treeBunch3"));

				auto& Tree3 = world->CreateEntity();
				auto& Tree3Tranform = world->GetComponent<Frosty::ECS::CTransform>(Tree3);
				Tree3Tranform.Position = BorderTrees(i * 4 + 10);
				auto& Tree3Material = world->AddComponent<Frosty::ECS::CMaterial>(Tree3, Frosty::AssetManager::GetShader("Texture2D"));
				Tree3Material.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
				treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
				//Tree7 is my fave
				Tree3Material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");

				world->AddComponent<Frosty::ECS::CMesh>(Tree3, Frosty::AssetManager::GetMesh("tree1"));

				auto& Tree4 = world->CreateEntity();
				auto& Tree4Tranform = world->GetComponent<Frosty::ECS::CTransform>(Tree4);
				Tree4Tranform.Position = BorderTrees(i * 4 + 11);
				auto& Tree4Material = world->AddComponent<Frosty::ECS::CMaterial>(Tree4, Frosty::AssetManager::GetShader("Texture2D"));
				Tree4Material.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
				treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
				//Tree7 is my fave
				Tree4Material.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");

				world->AddComponent<Frosty::ECS::CMesh>(Tree4, Frosty::AssetManager::GetMesh("tree1"));
			}

			if (!up)
			{
				auto& Tree = world->CreateEntity();
				auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
				TreeTranform.Rotation = glm::vec3(0, 90.0f, 0);
				TreeTranform.Position = glm::vec3(2.3f, 0.0f, -135.3f);
				auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
				TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
				std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
				//Tree7 is my fave
				TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");

				world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh("treeBunchWall"));

				//hittbox
				auto& hittbox = world->CreateEntity({ 0.0f, 0.0f, -130.0f }, { 0.0f, 0.0f, 0.0f }, { 270.0f, 40.0f, 28.0f }, true);
				auto& hittboxTransform = world->GetComponent<Frosty::ECS::CTransform>(hittbox);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox, Frosty::AssetManager::GetBoundingBox("pCube1"), hittboxTransform.Scale, 0.0f);
			}
			else
			{
				auto& ExitBBox = world->CreateEntity();
				auto& ExitBBoxTranform = world->GetComponent<Frosty::ECS::CTransform>(ExitBBox);
				ExitBBoxTranform.Position = glm::vec3(0.00f, 2.00f, -145.00f);
				ExitBBoxTranform.Scale = glm::vec3(55.00f, 20.00f, 10.00f);
				world->AddComponent<Frosty::ECS::CPhysics>(ExitBBox, Frosty::AssetManager::GetBoundingBox("pCube1"), ExitBBoxTranform.Scale, 0.0f);
				world->AddComponent<Frosty::ECS::CLevelExit>(ExitBBox, 0);

				world->AddComponent<Frosty::ECS::CMesh>(ExitBBox, Frosty::AssetManager::GetMesh("pCube1"));
				world->AddComponent<Frosty::ECS::CMaterial>(ExitBBox, Frosty::AssetManager::GetShader("FlatColor"));

				//hittbox
				auto& hittbox = world->CreateEntity({ -81.0f, 0.0f, -130.0f }, { 0.0f, 0.0f, 0.0f }, { 110.0f, 40.0f, 28.0f }, true);
				auto& hittboxTransform = world->GetComponent<Frosty::ECS::CTransform>(hittbox);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox, Frosty::AssetManager::GetBoundingBox("pCube1"), hittboxTransform.Scale, 0.0f);
				auto& hittbox2 = world->CreateEntity({ 81.0f, 0.0f, -130.0f }, { 0.0f, 0.0f, 0.0f }, { 110.0f, 40.0f, 28.0f }, true);
				auto& hittbox2Transform = world->GetComponent<Frosty::ECS::CTransform>(hittbox2);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox2, Frosty::AssetManager::GetBoundingBox("pCube1"), hittbox2Transform.Scale, 0.0f);
			}
			if (!down)
			{
				auto& Tree = world->CreateEntity();
				auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
				TreeTranform.Rotation = glm::vec3(0, 90 * 3, 0);
				TreeTranform.Position = glm::vec3(-2.3f, 0.0f, 135.3f);
				auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
				TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
				std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
				//Tree7 is my fave
				TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");

				world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh("treeBunchWall"));

				//hittbox
				auto& hittbox = world->CreateEntity({ 0.0f, 0.0f, 130.0f }, { 0.0f, 0.0f, 0.0f }, { 270.0f, 40.0f, 28.0f }, true);
				auto& hittboxTransform = world->GetComponent<Frosty::ECS::CTransform>(hittbox);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox, Frosty::AssetManager::GetBoundingBox("pCube1"), hittboxTransform.Scale, 0.0f);
			}
			else
			{
				auto& ExitBBox = world->CreateEntity();
				auto& ExitBBoxTranform = world->GetComponent<Frosty::ECS::CTransform>(ExitBBox);
				ExitBBoxTranform.Position = glm::vec3(0.00f, 2.00f, 145.00f);
				ExitBBoxTranform.Scale = glm::vec3(55.00f, 20.00f, 10.00f);
				world->AddComponent<Frosty::ECS::CPhysics>(ExitBBox, Frosty::AssetManager::GetBoundingBox("pCube1"), ExitBBoxTranform.Scale, 0.0f);
				world->AddComponent<Frosty::ECS::CLevelExit>(ExitBBox, 1);

				world->AddComponent<Frosty::ECS::CMesh>(ExitBBox, Frosty::AssetManager::GetMesh("pCube1"));
				world->AddComponent<Frosty::ECS::CMaterial>(ExitBBox, Frosty::AssetManager::GetShader("FlatColor"));

				//hittbox
				auto& hittbox = world->CreateEntity({ -81.0f, 0.0f, 130.0f }, { 0.0f, 0.0f, 0.0f }, { 110.0f, 40.0f, 28.0f }, true);
				auto& hittboxTransform = world->GetComponent<Frosty::ECS::CTransform>(hittbox);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox, Frosty::AssetManager::GetBoundingBox("pCube1"), hittboxTransform.Scale, 0.0f);
				auto& hittbox2 = world->CreateEntity({ 81.0f, 0.0f, 130.0f }, { 0.0f, 0.0f, 0.0f }, { 110.0f, 40.0f, 28.0f }, true);
				auto& hittbox2Transform = world->GetComponent<Frosty::ECS::CTransform>(hittbox2);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox2, Frosty::AssetManager::GetBoundingBox("pCube1"), hittbox2Transform.Scale, 0.0f);
			}
			if (!left)
			{
				auto& Tree = world->CreateEntity();
				auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
				auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
				TreeTranform.Position = glm::vec3(135.3f, 0.0f, -2.3f);
				TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
				std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
				//Tree7 is my fave
				TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");

				world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh("treeBunchWall"));

				//hittbox
				auto& hittbox = world->CreateEntity({ 130.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 28.0f, 40.0f, 270.0f }, true);
				auto& hittboxTransform = world->GetComponent<Frosty::ECS::CTransform>(hittbox);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox, Frosty::AssetManager::GetBoundingBox("pCube1"), hittboxTransform.Scale, 0.0f);
			}
			else
			{
				auto& ExitBBox = world->CreateEntity();
				auto& ExitBBoxTranform = world->GetComponent<Frosty::ECS::CTransform>(ExitBBox);
				ExitBBoxTranform.Position = glm::vec3(145.00f, 2.00f, 0.00f);
				ExitBBoxTranform.Scale = glm::vec3(10.00f, 20.00f, 55.00f);
				world->AddComponent<Frosty::ECS::CPhysics>(ExitBBox, Frosty::AssetManager::GetBoundingBox("pCube1"), ExitBBoxTranform.Scale, 0.0f);
				world->AddComponent<Frosty::ECS::CLevelExit>(ExitBBox, 3);

				world->AddComponent<Frosty::ECS::CMesh>(ExitBBox, Frosty::AssetManager::GetMesh("pCube1"));
				world->AddComponent<Frosty::ECS::CMaterial>(ExitBBox, Frosty::AssetManager::GetShader("FlatColor"));

				//hittbox
				auto& hittbox = world->CreateEntity({ 130.0f, 0.0f, -81.0f }, { 0.0f, 0.0f, 0.0f }, { 28.0f, 40.0f, 110.0f }, true);
				auto& hittboxTransform = world->GetComponent<Frosty::ECS::CTransform>(hittbox);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox, Frosty::AssetManager::GetBoundingBox("pCube1"), hittboxTransform.Scale, 0.0f);
				auto& hittbox2 = world->CreateEntity({ 130.0f, 0.0f, 81.0f }, { 0.0f, 0.0f, 0.0f }, { 28.0f, 40.0f, 110.0f }, true);
				auto& hittbox2Transform = world->GetComponent<Frosty::ECS::CTransform>(hittbox2);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox2, Frosty::AssetManager::GetBoundingBox("pCube1"), hittbox2Transform.Scale, 0.0f);
			}
			if (!right)
			{
				auto& Tree = world->CreateEntity();
				auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
				TreeTranform.Rotation = glm::vec3(0, 90 * 2, 0);
				TreeTranform.Position = glm::vec3(-135.3f, 0.0f, 2.3f);
				auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
				TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
				std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
				//Tree7 is my fave
				TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");

				world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh("treeBunchWall"));

				//hittbox
				auto& hittbox = world->CreateEntity({ -130.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 28.0f, 40.0f, 270.0f }, true);
				auto& hittboxTransform = world->GetComponent<Frosty::ECS::CTransform>(hittbox);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox, Frosty::AssetManager::GetBoundingBox("pCube1"), hittboxTransform.Scale, 0.0f);
			}
			else  
			{
				auto& ExitBBox = world->CreateEntity();
				auto& ExitBBoxTranform = world->GetComponent<Frosty::ECS::CTransform>(ExitBBox);
				ExitBBoxTranform.Position = glm::vec3(-145.00f, 2.00f, 0.00f);
				ExitBBoxTranform.Scale = glm::vec3(10.00f, 20.00f, 55.00f);
				world->AddComponent<Frosty::ECS::CPhysics>(ExitBBox, Frosty::AssetManager::GetBoundingBox("pCube1"), ExitBBoxTranform.Scale, 0.0f);
				world->AddComponent<Frosty::ECS::CLevelExit>(ExitBBox, 2);

				world->AddComponent<Frosty::ECS::CMesh>(ExitBBox, Frosty::AssetManager::GetMesh("pCube1"));
				world->AddComponent<Frosty::ECS::CMaterial>(ExitBBox, Frosty::AssetManager::GetShader("FlatColor"));

				//hittbox
				auto& hittbox = world->CreateEntity({ -130.0f, 0.0f, -81.0f }, { 0.0f, 0.0f, 0.0f }, { 28.0f, 40.0f, 110.0f }, true);
				auto& hittboxTransform = world->GetComponent<Frosty::ECS::CTransform>(hittbox);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox, Frosty::AssetManager::GetBoundingBox("pCube1"), hittboxTransform.Scale, 0.0f);
				auto& hittbox2 = world->CreateEntity({ -130.0f, 0.0f, 81.0f }, { 0.0f, 0.0f, 0.0f }, { 28.0f, 40.0f, 110.0f }, true);
				auto& hittbox2Transform = world->GetComponent<Frosty::ECS::CTransform>(hittbox2);
				world->AddComponent<Frosty::ECS::CPhysics>(hittbox2, Frosty::AssetManager::GetBoundingBox("pCube1"), hittbox2Transform.Scale, 0.0f);
			}

			//Old level
			if (false)
			{
				if (!onlyPos)
				{
					auto& world = Frosty::Application::Get().GetWorld();
					auto& PlaneOne = world->CreateEntity();
					auto& testTranform = world->GetComponent<Frosty::ECS::CTransform>(PlaneOne);
					testTranform.Scale = glm::vec3(300, 1.0f, 300);
					testTranform.Rotation = glm::vec3(0, rotation, 0);
					world->AddComponent<Frosty::ECS::CMesh>(PlaneOne, Frosty::AssetManager::GetMesh("tiledGround"));
					//world->AddComponent<Frosty::ECS::CMesh>(PlaneOne, Frosty::AssetManager::GetMesh("pPlane1"));
					auto& testMaterial = world->AddComponent<Frosty::ECS::CMaterial>(PlaneOne, Frosty::AssetManager::GetShader("Texture2D"));
					//testMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
					//testMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(texture);
					testMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("brown_mud_diffuse");
					//testMaterial.TextureScale = glm::vec2(mapDepth / 10.0f, mapLength / 10.0f);

					//trees
					for (int i = 0; i < 39; i++)
					{

						auto& Tree = world->CreateEntity();
						auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
						TreeTranform.Position = BorderTrees(i);
						auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
						TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
						std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
						TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree7");

						treeName = "tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
						world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh("tree1"));
						world->AddComponent<Frosty::ECS::CPhysics>(Tree, Frosty::AssetManager::GetBoundingBox("tree1"), TreeTranform.Scale);
					}
					if (!up)
					{
						for (int i = 39; i < 44; i++)
						{
							auto& Tree = world->CreateEntity();
							auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
							TreeTranform.Position = BorderTrees(i);
							auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
							TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
							std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
							TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree1");

							treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
							world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh("tree1"));
						}
					}
					else
					{
						auto& ExitBBox = world->CreateEntity();
						auto& ExitBBoxTranform = world->GetComponent<Frosty::ECS::CTransform>(ExitBBox);
						ExitBBoxTranform.Position = glm::vec3(0.50f, 2.50f, -82.60f);
						ExitBBoxTranform.Scale = glm::vec3(56.10f, 16.20f, 12.80f);
						world->AddComponent<Frosty::ECS::CPhysics>(ExitBBox, Frosty::AssetManager::GetBoundingBox("pCube1"), ExitBBoxTranform.Scale, 0.0f);
						world->AddComponent<Frosty::ECS::CLevelExit>(ExitBBox, 0);

						world->AddComponent<Frosty::ECS::CMesh>(ExitBBox, Frosty::AssetManager::GetMesh("pCube1"));
						world->AddComponent<Frosty::ECS::CMaterial>(ExitBBox, Frosty::AssetManager::GetShader("FlatColor"));
					}
					if (!down)
					{
						for (int i = 44; i < 48; i++)
						{
							auto& Tree = world->CreateEntity();
							auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
							TreeTranform.Position = BorderTrees(i);
							auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
							TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
							std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
							TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree1");

							treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
							world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh("tree1"));
						}
					}
					else
					{
						auto& ExitBBox = world->CreateEntity();
						auto& ExitBBoxTranform = world->GetComponent<Frosty::ECS::CTransform>(ExitBBox);
						ExitBBoxTranform.Position = glm::vec3(-2.90f, 4.00f, 95.80f);
						ExitBBoxTranform.Scale = glm::vec3(45.70f, 20.80f, 10.50f);
						world->AddComponent<Frosty::ECS::CPhysics>(ExitBBox, Frosty::AssetManager::GetBoundingBox("pCube1"), ExitBBoxTranform.Scale, 0.0f);
						world->AddComponent<Frosty::ECS::CLevelExit>(ExitBBox, 1);

						world->AddComponent<Frosty::ECS::CMesh>(ExitBBox, Frosty::AssetManager::GetMesh("pCube1"));
						world->AddComponent<Frosty::ECS::CMaterial>(ExitBBox, Frosty::AssetManager::GetShader("FlatColor"));
					}
					if (!left)
					{
						for (int i = 52; i < 55; i++)
						{
							auto& Tree = world->CreateEntity();
							auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
							TreeTranform.Position = BorderTrees(i);
							auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
							TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
							std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
							TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree1");

							treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
							world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh("tree1"));
						}
					}
					else
					{
						auto& ExitBBox = world->CreateEntity();
						auto& ExitBBoxTranform = world->GetComponent<Frosty::ECS::CTransform>(ExitBBox);
						ExitBBoxTranform.Position = glm::vec3(100.00f, 2.00f, 5.70f);
						ExitBBoxTranform.Scale = glm::vec3(15.00f, 29.30f, 57.90f);
						world->AddComponent<Frosty::ECS::CPhysics>(ExitBBox, Frosty::AssetManager::GetBoundingBox("pCube1"), ExitBBoxTranform.Scale, 0.0f);
						world->AddComponent<Frosty::ECS::CLevelExit>(ExitBBox, 3);

						world->AddComponent<Frosty::ECS::CMesh>(ExitBBox, Frosty::AssetManager::GetMesh("pCube1"));
						world->AddComponent<Frosty::ECS::CMaterial>(ExitBBox, Frosty::AssetManager::GetShader("FlatColor"));
					}
					if (!right)
					{
						for (int i = 48; i < 52; i++)
						{
							auto& Tree = world->CreateEntity();
							auto& TreeTranform = world->GetComponent<Frosty::ECS::CTransform>(Tree);
							TreeTranform.Position = BorderTrees(i);
							auto& TreeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(Tree, Frosty::AssetManager::GetShader("Texture2D"));
							TreeMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
							std::string treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
							TreeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree1");

							treeName = "Tree" + std::to_string(((rand() % NrOfTreeTypes) + 1));
							world->AddComponent<Frosty::ECS::CMesh>(Tree, Frosty::AssetManager::GetMesh("tree1"));
						}
					}
					else
					{
						auto& ExitBBox = world->CreateEntity();
						auto& ExitBBoxTranform = world->GetComponent<Frosty::ECS::CTransform>(ExitBBox);
						ExitBBoxTranform.Position = glm::vec3(-91.90f, 0.00f, 5.00f);
						ExitBBoxTranform.Scale = glm::vec3(12.40f, 30.90f, 63.30f);
						world->AddComponent<Frosty::ECS::CPhysics>(ExitBBox, Frosty::AssetManager::GetBoundingBox("pCube1"), ExitBBoxTranform.Scale, 0.0f);
						world->AddComponent<Frosty::ECS::CLevelExit>(ExitBBox, 2);

						world->AddComponent<Frosty::ECS::CMesh>(ExitBBox, Frosty::AssetManager::GetMesh("pCube1"));
						world->AddComponent<Frosty::ECS::CMaterial>(ExitBBox, Frosty::AssetManager::GetShader("FlatColor"));
					}
				}
			}
			//temp
			if (up && entrensSide == 1)
			{
				toReturn = glm::vec3(0.00f, 0.00f, -145.00f) + glm::vec3(0.00f, 0.00f, 10.0f);
			}
			else if (down && entrensSide == 0)
			{
				toReturn = glm::vec3(0.00f, 0.00f, 145.00f) - glm::vec3(0.00f, 0.00f, 10.0f);
			}
			else if (right && entrensSide == 3)
			{
				toReturn = glm::vec3(-145.00f, 0.00f, 0.00f) + glm::vec3(10.00f, 0.00f, 0.00f);
			}
			else if (left && entrensSide == 2)
			{
				toReturn = glm::vec3(145.00f, 0.00f, 0.00f) - glm::vec3(10.00f, 0.00f, 0.00f);
			}
			else
			{
				toReturn = glm::vec3(0, 0, 0);
			}
			return toReturn;
		}

		glm::vec3 MoveToNewRoom(bool up, bool down, bool right, bool left, int entrensSide)
		{
			glm::vec3 toReturn;
			if (up && entrensSide == 1)
			{
				toReturn = glm::vec3(0.00f, 0.00f, -145.00f) + glm::vec3(0.00f, 0.00f, 20.0f);
			}
			else if (down && entrensSide == 0)
			{
				toReturn = glm::vec3(0.00f, 0.00f, 145.00f) - glm::vec3(0.00f, 0.00f, 20.0f);
			}
			else if (right && entrensSide == 3)
			{
				toReturn = glm::vec3(-145.00f, 0.00f, 0.00f) + glm::vec3(20.00f, 0.00f, 0.00f);
			}
			else if (left && entrensSide == 2)
			{
				toReturn = glm::vec3(145.00f, 0.00f, 0.00f) - glm::vec3(20.00f, 0.00f, 0.00f);
			}
			else
			{
				toReturn = glm::vec3(0, 0, 0);
			}



			return toReturn;
		}

		glm::vec3 BorderTrees(int treeNr)
		{
			switch (treeNr)
			{
			case 0:
				return glm::vec3(136.60f, 0.00f, 70.00f);
				break;
			case 1:
				return glm::vec3(136.60f, 0.00f, -70.00f);
				break;
			case 2:
				return glm::vec3(-70.00f, 0.00f, -136.60f);
				break;
			case 3:
				return glm::vec3(70.00f, 0.00f, -136.60f);
				break;
			case 4:
				return glm::vec3(-136.60f, 0.00f, 70.00f);
				break;
			case 5:
				return glm::vec3(-136.60f, 0.00f, -70.00f);
				break;
			case 6:
				return glm::vec3(-70.00f, 0.00f, 136.60f);
				break;
			case 7:
				return glm::vec3(70.00f, 0.00f, 136.60f);
				break;
				//corners
			case 8:
				return glm::vec3(-135.70f, 0.00f, 134.00f);
				break;
			case 9:
				return glm::vec3(-124.40f, 0.00f, 130.40f);
				break;
			case 10:
				return glm::vec3(-135.00f, 0.00f, 118.00f);
				break;
			case 11:
				return glm::vec3(-147.30f, 0.00f, 117.30f);
				break;
			case 12:
				return glm::vec3(-135.70f, 0.00f, -134.00f);
				break;
			case 13:
				return glm::vec3(-124.40f, 0.00f, -130.40f);
				break;
			case 14:
				return glm::vec3(-135.00f, 0.00f, -118.00f);
				break;
			case 15:
				return glm::vec3(-147.30f, 0.00f, -117.30f);
				break;
			case 16:
				return glm::vec3(135.70f, 0.00f, 134.00f);
				break;
			case 17:
				return glm::vec3(124.40f, 0.00f, 130.40f);
				break;
			case 18:
				return glm::vec3(135.00f, 0.00f, 118.00f);
				break;
			case 19:
				return glm::vec3(147.30f, 0.00f, 117.30f);
				break;
			case 20:
				return glm::vec3(135.70f, 0.00f, -134.00f);
				break;
			case 21:
				return glm::vec3(124.40f, 0.00f, -130.40f);
				break;
			case 22:
				return glm::vec3(135.00f, 0.00f, -118.00f);
				break;
			case 23:
				return glm::vec3(147.30f, 0.00f, -117.30f);
				break;

			default:
				break;
			}
			//old
			if (false)
			{
				switch (treeNr)
				{
				case 0:
					return glm::vec3(-40.60f, 0.00f, -61.00f);
					break;
				case 1:
					return glm::vec3(67.00f, 0.00f, -10.50f);
					break;
				case 2:
					return glm::vec3(-47.70f, 0.00f, -75.30f);
					break;
				case 3:
					return glm::vec3(67.20f, 0.00f, -66.90f);
					break;
				case 4:
					return glm::vec3(50.80f, 0.00f, -58.90f);
					break;
				case 5:
					return glm::vec3(32.50f, 0.00f, -74.80f);
					break;
				case 6:
					return glm::vec3(-30.30f, 0.00f, -65.30f);
					break;
				case 7:
					return glm::vec3(36.10f, 0.00f, -57.20f);
					break;
				case 8:
					return glm::vec3(54.80f, 0.00f, -71.30f);
					break;
				case 9:
					return glm::vec3(-58.40f, 0.00f, -58.60f);
					break;
				case 10:
					return glm::vec3(77.30f, 0.00f, -54.70f);
					break;
				case 11:
					return glm::vec3(-71.40f, 0.00f, -76.10f);
					break;
				case 12:
					return glm::vec3(71.00f, 0.00f, -30.50f);
					break;
				case 13:
					return glm::vec3(-75.10f, 0.00f, -55.90f);
					break;
				case 14:
					return glm::vec3(-65.70f, 0.00f, -38.60f);
					break;
				case 15:
					return glm::vec3(-74.60f, 0.00f, -25.30f);
					break;
				case 16:
					return glm::vec3(-81.60f, 0.00f, -43.80f);
					break;
				case 17:
					return glm::vec3(84.60f, 0.00f, -22.70f);
					break;
				case 18:
					return glm::vec3(60.40f, 0.00f, -40.60f);
					break;
				case 19:
					return glm::vec3(-33.70f, 0.00f, 72.10f);
					break;
				case 20:
					return glm::vec3(-49.00f, 0.00f, 84.50f);
					break;
				case 21:
					return glm::vec3(-30.70f, 0.00f, 91.40f);
					break;
				case 22:
					return glm::vec3(84.70f, 0.00f, -77.70f);
					break;
				case 23:
					return glm::vec3(-81.80f, 0.00f, 62.10f);
					break;
				case 24:
					return glm::vec3(-66.60f, 0.00f, 71.00f);
					break;
				case 25:
					return glm::vec3(-81.00f, 0.00f, 85.20f);
					break;
				case 26:
					return glm::vec3(-68.50f, 0.00f, 87.90f);
					break;
				case 27:
					return glm::vec3(19.30f, 0.00f, 75.20f);
					break;
				case 28:
					return glm::vec3(32.90f, 0.00f, 73.90f);
					break;
				case 29:
					return glm::vec3(38.20f, 0.00f, 88.60f);
					break;
				case 30:
					return glm::vec3(62.30f, 0.00f, 73.90f);
					break;
				case 31:
					return glm::vec3(22.60f, 0.00f, 90.30f);
					break;
				case 32:
					return glm::vec3(45.90f, 0.00f, 77.20f);
					break;
				case 33:
					return glm::vec3(64.60f, 0.00f, 90.30f);
					break;
				case 34:
					return glm::vec3(81.40f, 0.00f, 64.20f);
					break;
				case 35:
					return glm::vec3(73.70f, 0.00f, 44.90f);
					break;
				case 36:
					return glm::vec3(84.40f, 0.00f, 33.60f);
					break;
				case 37:
					return glm::vec3(-76.20f, 0.00f, 38.00f);
					break;
				case 38:
					return glm::vec3(-67.80f, 0.00f, 51.20f);
					break;
				default:
					break;
				}

				//Up cases
				switch (treeNr - 39)
				{
				case 0:
					return glm::vec3(0.50f, 0.00f, -75.10f);
					break;
				case 1:
					return glm::vec3(3.40f, 0.00f, -56.80f);
					break;
				case 2:
					return glm::vec3(-13.70f, 0.00f, -60.20f);
					break;
				case 3:
					return glm::vec3(13.70f, 0.00f, -66.70f);
					break;
				case 4:
					return glm::vec3(-24.90f, 0.00f, -78.00f);
					break;
				default:
					break;
				}

				//Down cases
				switch (treeNr - 44)
				{
				case 0:
					return glm::vec3(-18.90f, 0.00f, 83.60f);
					break;
				case 1:
					return glm::vec3(5.60f, 0.00f, 85.70f);
					break;
				case 2:
					return glm::vec3(-7.90f, 0.00f, 75.60f);
					break;
				case 3:
					return glm::vec3(-5.10f, 0.00f, 92.90f);
					break;
				default:
					break;
				}

				//left cases
				switch (treeNr - 48)
				{
				case 0:
					return glm::vec3(-66.30f, 0.00f, -6.10f);
					break;
				case 1:
					return glm::vec3(-74.80f, 0.00f, 7.80f);
					break;
				case 2:
					return glm::vec3(-62.40f, 0.00f, 21.90f);
					break;
				case 3:
					return glm::vec3(-79.00f, 0.00f, 22.20f);
					break;
				default:
					break;
				}

				//right cases
				switch (treeNr - 52)
				{
				case 0:
					return glm::vec3(71.40f, 0.00f, 21.70f);
					break;
				case 1:
					return glm::vec3(88.50f, 0.00f, 16.10f);
					break;
				case 2:
					return glm::vec3(85.00f, 0.00f, -0.30f);
					break;
				default:
					break;
				}
			}
			return glm::vec3(0);
		}
	}
}
