#include <mcspch.hpp>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Game.hpp"


struct Entity
{
	inline static int s_LastId{ 1 };
	int Id{ -1 };

	Entity() : Id(s_LastId++) { }
};

struct EntityManager
{
	std::vector<std::shared_ptr<Entity>> Entities;

	EntityManager() { Entities.reserve(10); }

	std::shared_ptr<Entity>& CreateEntity()
	{
		Entities.emplace_back(FY_NEW Entity());
		return Entities.back();
	}

	void DestroyEntity(const std::shared_ptr<Entity>& entity)
	{
		int index = -1;

		for (size_t i = 0; i < Entities.size() && index == -1; i++)
		{
			if (entity->Id == Entities.at(i)->Id)
			{
				index = i;
			}
		}

		if (index >= 0)
		{
			Entities.at(index).reset();
			Entities.erase(Entities.begin() + index);
		}
	}

	void PrintInfo()
	{
		std::cout << "\t-----------Entity Info-----------\n";
		std::cout << "\tIndex\tId\tAddress\t\t\tRefs\n";
		for (size_t i = 0; i < Entities.size(); i++)
		{
			std::cout << "\t" << i << "\t" << Entities.at(i)->Id << "\t" << Entities.at(i) << "\t" << Entities.at(i).use_count() << std::endl;
		}
		std::cout << "\t----------------Done----------------\n\n";
	}
};

struct TransformComponent
{
	std::shared_ptr<Entity> EntityPtr;
	glm::vec3 Position{ 0.0f };

	TransformComponent() = default;
};

struct TransformManager
{
	int Total{ 1 };
	std::array<TransformComponent, 10> Components;

	TransformComponent& Add(const std::shared_ptr<Entity>& entity)
	{
		Components[Total] = TransformComponent();
		Components[Total].EntityPtr = entity;
		return Components[Total++];
	}

	void Remove(const std::shared_ptr<Entity>& entity)
	{
		int index = 0;

		for (int i = 1; i < Total && index == 0; i++)
		{
			if (Components[i].EntityPtr->Id == entity->Id)
			{
				index = i;
			}
		}

		if (index > 0)
		{
			Total--;
			Components[index].EntityPtr.reset();
			Components[index] = Components[Total];
			Components[Total].EntityPtr.reset();
			Components[Total] = TransformComponent();
		}
	}

	void PrintInfo()
	{
		std::cout << "\t-----------Component Info-----------\n";
		std::cout << "\tIndex\tComponent Address\tEntity Id\tEntity Address\t\tEntity Refs\n";
		for (size_t i = 1; i < Total; i++)
		{
			std::cout << "\t" << i << "\t" << &Components[i] << "\t" << Components[i].EntityPtr->Id << "\t\t" << Components[i].EntityPtr << "\t\t" << Components[i].EntityPtr.use_count() << std::endl;
		}
		std::cout << "\t----------------Done----------------\n\n";
	}
};


struct RendererSystem
{
	int Total{ 1 };
	std::array<TransformComponent*, 10> Components;

	RendererSystem() = default;

	void AddComponent(TransformComponent& tComp)
	{
		Components[Total++] = &tComp;
	}

	void RemoveEntity(const std::shared_ptr<Entity>& entity)
	{
		int index = 0;

		for (size_t i = 1; i < Total && index == 0; i++)
		{
			if (entity->Id == Components[i]->EntityPtr->Id)
			{
				index = i;
			}
		}

		if (index > 0)
		{
			Total--;
			Components[Total] = nullptr;
		}
	}

	void PrintInfo()
	{
		std::cout << "\t-----------System Info-----------\n";
		std::cout << "\tIndex\tComponent Address\tEntity Id\tEntity Address\t\tEntity Refs\n";
		for (size_t i = 1; i < Total; i++)
		{
			std::cout << "\t" << i << "\t" << Components[i] << "\t" << Components[i]->EntityPtr->Id << "\t\t" << Components[i]->EntityPtr << "\t\t" << Components[i]->EntityPtr.use_count() << std::endl;
		}
		std::cout << "\t----------------Done----------------\n\n";
	}
};

struct World
{
	EntityManager EManager;
	TransformManager TManager;
	RendererSystem RSystem;

	std::shared_ptr<Entity>& CreateEntity()
	{
		return EManager.CreateEntity();
	}

	void DestroyEntity(const std::shared_ptr<Entity>& entity)
	{
		RSystem.RemoveEntity(entity);
		TManager.Remove(entity);
		EManager.DestroyEntity(entity);
	}

	TransformComponent& AddTransform(const std::shared_ptr<Entity>& entity)
	{
		TransformComponent& tComp = TManager.Add(entity);
		RSystem.AddComponent(tComp);

		return tComp;
	}

	void PrintInfo()
	{
		std::cout << "-----------Printing Info-----------\n";
		EManager.PrintInfo();
		TManager.PrintInfo();
		RSystem.PrintInfo();
		std::cout << "----------------Done----------------\n\n";
	}
};

struct Foo
{
	TransformComponent Comp;

	TransformComponent* GetA() { return &Comp; }
	TransformComponent& GetB() { return Comp; }
};

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	srand((unsigned int)time(NULL));

	{
		std::shared_ptr<Entity> entity;
		entity.reset(FY_NEW Entity());
		std::map<std::shared_ptr<Entity>, int> eMap;
		eMap.emplace(entity, 4);
		int index = eMap[entity];
	}
	//{
	//	Foo foo;
	//	std::cout << foo.GetA() << std::endl;
	//	std::cout << &foo.GetB() << std::endl;
	//
	//}
	//{
	//	World world;
	//	auto& plane = world.CreateEntity();
	//	world.AddTransform(plane);
	//	auto& player = world.CreateEntity();
	//	world.AddTransform(player);
	//	auto& tree = world.CreateEntity();
	//	world.AddTransform(tree);
	//
	//	world.PrintInfo();
	//
	//	auto& enemy = world.CreateEntity();
	//	world.AddTransform(enemy);
	//
	//	world.PrintInfo();
	//
	//	world.DestroyEntity(player);
	//
	//	world.PrintInfo();
	//
	//	//player.reset();
	//}

	auto app = FY_NEW MCS::Game();
	app->Run();
	delete app;

	return 0;
}
/*
[22:31:52] FROSTY:
		-----------Entity Info-----------
		Index   Id      Address                 Refs
		0       1       000002900F698CB0        7
		1       2       000002900F697690        8
		2       3       000002900F6976F0        6
		3       4       000002900F697750        6
		4       5       000002900F698D70        20
		5       6       000002900F698A70        10
		6       7       000002900F697990        10
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Transform Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F97A330        1               000002900F698CB0                7
		2       000002900F97A3B0        2               000002900F697690                8
		3       000002900F97A430        3               000002900F6976F0                6
		4       000002900F97A4B0        4               000002900F697750                6
		5       000002900F97A530        5               000002900F698D70                20
		6       000002900F97A5B0        6               000002900F698A70                10
		7       000002900F97A630        7               000002900F697990                10
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Camera Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F99A428        1               000002900F698CB0                7
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Light Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900FA104A0        3               000002900F6976F0                6
		2       000002900FA104D0        4               000002900F697750                6
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Mesh Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F9DC3B8        2               000002900F697690                8
		2       000002900F9DC3E0        5               000002900F698D70                20
		3       000002900F9DC408        6               000002900F698A70                10
		4       000002900F9DC430        7               000002900F697990                10
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Material Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F9E64A8        2               000002900F697690                8
		2       000002900F9E6550        5               000002900F698D70                20
		3       000002900F9E65F8        6               000002900F698A70                10
		4       000002900F9E66A0        7               000002900F697990                10
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Player Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F69DFC0        5               000002900F698D70                20
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Physics Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F6AE040        5               000002900F698D70                20
		2       000002900F6AE090        6               000002900F698A70                10
		3       000002900F6AE0E0        7               000002900F697990                10
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Dash Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F6C2088        5               000002900F698D70                20
		----------------Done----------------


[22:31:52] FROSTY:
		-----------BasicAttack Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F6CC0F0        5               000002900F698D70                20
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Health Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F6D4168        5               000002900F698D70                20
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Inventory Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F6DE210        5               000002900F698D70                20
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Camera System Info-----------
				---------Entity Map---------
				Entity Id       Entity Address          Entity Refs     Array Index
				1               000002900F698CB0                7       1
				-----------Done-----------
				------Component Array(s)------

				Index   Component Address       Entity Id       Entity Address          Entity Refs
				1       000002900F97A330        1               000002900F698CB0                7
				1       000002900F99A428        1               000002900F698CB0                7

				-----------Done-----------
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Light System Info-----------
				---------Entity Map---------
				Entity Id       Entity Address          Entity Refs     Array Index
				3               000002900F6976F0                6       1
				4               000002900F697750                6       2
				-----------Done-----------
				------Component Array(s)------

				Index   Component Address       Entity Id       Entity Address          Entity Refs
				1       000002900F97A430        3               000002900F6976F0                6
				1       000002900FA104A0        3               000002900F6976F0                6

				2       000002900F97A4B0        4               000002900F697750                6
				2       000002900FA104D0        4               000002900F697750                6

				-----------Done-----------
		----------------Done----------------


[22:31:52] FROSTY:
		-----------Render System Info-----------
				---------Entity Map---------
				Entity Id       Entity Address          Entity Refs     Array Index
				2               000002900F697690                8       1
				7               000002900F697990                10      4
				6               000002900F698A70                10      3
				5               000002900F698D70                20      2
				-----------Done-----------
				------Component Array(s)------

				Index   Component Address       Entity Id       Entity Address          Entity Refs
				1       000002900F97A3B0        2               000002900F697690                8
				1       000002900F9DC3B8        2               000002900F697690                8
				1       000002900F9E64A8        2               000002900F697690                8

				2       000002900F97A530        5               000002900F698D70                20
				2       000002900F9DC3E0        5               000002900F698D70                20
				2       000002900F9E6550        5               000002900F698D70                20

				3       000002900F97A5B0        6               000002900F698A70                10
				3       000002900F9DC408        6               000002900F698A70                10
				3       000002900F9E65F8        6               000002900F698A70                10

				4       000002900F97A630        7               000002900F697990                10
				4       000002900F9DC430        7               000002900F697990                10
				4       000002900F9E66A0        7               000002900F697990                10

				-----------Done-----------
		----------------Done----------------
*/