#include <mcspch.hpp>
#include "Game.hpp"

class CameraSystem : public Frosty::ECS::BaseSystem
{
public:
	CameraSystem() = default;
	virtual ~CameraSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCamera>(), true);

		Frosty::PrefabManager::GetPrefabManager()->SetPrefab("StorKlocka", "klocka","");
	}

	virtual void OnUpdate() override
	{
		auto& win = Frosty::Application::Get().GetWindow();
		glm::vec3 front;
		for (size_t i = 1; i < p_Total; i++)
		{
			// Follow player
			if (m_Cameras[i]->Target)
			{
				m_Transform[i]->Position = m_Cameras[i]->Target->Position + glm::vec3(0.0f, 20.0f, -20.0f);
			}
			else
			{
				front.x = cos(glm::radians(m_Transform[i]->Rotation.x)) * cos(glm::radians(m_Transform[i]->Rotation.y));
				front.y = sin(glm::radians(m_Transform[i]->Rotation.y));
				front.z = sin(glm::radians(m_Transform[i]->Rotation.x)) * cos(glm::radians(m_Transform[i]->Rotation.y));
				m_Cameras[i]->Front = glm::normalize(front);
			}
			m_Cameras[i]->ViewMatrix = glm::lookAt(m_Transform[i]->Position, m_Transform[i]->Position + m_Cameras[i]->Front, { 0.0f, 1.0f, 0.0f });
			m_Cameras[i]->ViewProjectionMatrix = m_Cameras[i]->ProjectionMatrix * m_Cameras[i]->ViewMatrix;
		}
	}

	virtual void BeginScene(bool editorCamera) override
	{
		if (editorCamera) return;

		if (p_Total > 1)
		{
			for (size_t i = 1; i < p_Total; i++)
			{
				Frosty::RenderCommand::SetClearColor(glm::vec4(m_Cameras[i]->Background, 1.0f));
				Frosty::RenderCommand::Clear();
				Frosty::Renderer::BeginScene(m_Cameras[i]->ViewProjectionMatrix);
			}
		}
		else
		{
			Frosty::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			Frosty::RenderCommand::Clear();
			Frosty::Renderer::BeginScene(glm::mat4(1.0f));
		}
	}

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Cameras[p_Total] = &world->GetComponent<Frosty::ECS::CCamera>(entity);

			// Initiate entity based stuff here
			auto& win = Frosty::Application::Get().GetWindow();
			glm::vec3 front;
			front.x = cos(glm::radians(m_Transform[p_Total]->Rotation.x)) * cos(glm::radians(m_Transform[p_Total]->Rotation.y));
			front.y = sin(glm::radians(m_Transform[p_Total]->Rotation.y));
			front.z = sin(glm::radians(m_Transform[p_Total]->Rotation.x)) * cos(glm::radians(m_Transform[p_Total]->Rotation.y));
			m_Cameras[p_Total]->Front = glm::normalize(front);

			m_Cameras[p_Total]->ProjectionMatrix = glm::perspective(glm::radians(m_Cameras[p_Total]->FieldOfView), (float)(win.GetViewport().z / win.GetViewport().w), m_Cameras[p_Total]->Near, m_Cameras[p_Total]->Far);
			m_Cameras[p_Total]->ViewMatrix = glm::lookAt(m_Transform[p_Total]->Position, m_Transform[p_Total]->Position + m_Cameras[p_Total]->Front, { 0.0f, 1.0f, 0.0f });
			m_Cameras[p_Total]->ViewProjectionMatrix = m_Cameras[p_Total]->ProjectionMatrix * m_Cameras[p_Total]->ViewMatrix;

			p_Total++;
		}
	}

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Transform[p_Total] = nullptr;
			m_Cameras[p_Total] = nullptr;

			//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}

			p_EntityMap.erase(entity);
		}
	}

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CCamera*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Cameras;

};

class LightSystem : public Frosty::ECS::BaseSystem
{
public:
	LightSystem() = default;
	virtual ~LightSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CLight>(), true);
	}

	virtual void OnUpdate() override
	{
		auto& win = Frosty::Application::Get().GetWindow();

		if (p_Total > 1)
		{
			for (size_t i = 1; i < p_Total; i++)
			{
				Frosty::Renderer::AddLight(m_Transform[i]->Position, m_Light[i]->Color);
			}
		}
	}

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Light[p_Total] = &world->GetComponent<Frosty::ECS::CLight>(entity);

			p_Total++;
		}
	}

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Transform[p_Total] = nullptr;
			m_Light[p_Total] = nullptr;

			//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}

			p_EntityMap.erase(entity);
		}
	}

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CLight*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Light;

};

class RenderSystem : public Frosty::ECS::BaseSystem
{
public:
	RenderSystem() = default;
	virtual ~RenderSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMesh>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMaterial>(), true);
	}

	inline virtual void OnUpdate() override { }

	virtual void Render()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			// So the question is, calculate it every frame or save the result in the component
			// and update it only when Position/Rotation/Scale changes
			// Takes up performance if calculated every frame, but the other way will mean
			// 4 * 4 floats which is 4 * 4 * 4 = 64 bytes for EVERY entity
			// For now we calculate it every frame
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Transform[i]->Position);
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.x), { 1.0f, 0.0f, 0.0f });
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.z), { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, m_Transform[i]->Scale);

			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Bind();
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->GlossTexture) m_Materials[i]->GlossTexture->Bind(1);
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Bind(2);

			Frosty::Renderer::Submit(m_Materials[i], m_Meshes[i]->Mesh, transform);

			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->GlossTexture) m_Materials[i]->GlossTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Unbind();
		}
	}

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Meshes[p_Total] = &world->GetComponent<Frosty::ECS::CMesh>(entity);
			m_Materials[p_Total] = &world->GetComponent<Frosty::ECS::CMaterial>(entity);

			p_Total++;
		}
	}

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];


		if (tempIndex > 0)
		{
			p_Total--;
			m_Transform[p_Total] = nullptr;
			m_Meshes[p_Total] = nullptr;
			m_Materials[p_Total] = nullptr;

			if (p_Total > 1)
			{
				//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CMesh*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Meshes;
	std::array<Frosty::ECS::CMaterial*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Materials;

};

class MovementSystem : public Frosty::ECS::BaseSystem
{
public:
	MovementSystem() = default;
	virtual ~MovementSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMotion>(), true);
	}

	inline virtual void OnUpdate() override
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			m_Transform[i]->Position += m_Motion[i]->Direction * m_Motion[i]->Speed * Frosty::Time::DeltaTime();
		}
	}

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Motion[p_Total] = &world->GetComponent<Frosty::ECS::CMotion>(entity);

			p_Total++;
		}
	}

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];


		if (tempIndex > 0)
		{
			p_Total--;
			m_Transform[p_Total] = nullptr;
			m_Motion[p_Total] = nullptr;

			if (p_Total > 1)
			{
				//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CMotion*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Motion;

};

class PlayerControllerSystem : public Frosty::ECS::BaseSystem
{
public:
	PlayerControllerSystem() = default;
	virtual ~PlayerControllerSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CController>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMotion>(), true);
	}

	inline virtual void OnInput() override
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			m_Motion[i]->Direction = glm::vec3(0.0f);
			if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveNorthKey))
			{
				m_Motion[i]->Direction.z = 1.0f;
			}
			else if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveSouthKey))
			{
				m_Motion[i]->Direction.z = -1.0f;
			}
			if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveWestKey))
			{
				m_Motion[i]->Direction.x = 1.0f;
			}
			else if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveEastKey))
			{
				m_Motion[i]->Direction.x = -1.0f;
			}
		}
	}
	
	inline virtual void OnUpdate() override { }

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Controller[p_Total] = &world->GetComponent<Frosty::ECS::CController>(entity);
			m_Motion[p_Total] = &world->GetComponent<Frosty::ECS::CMotion>(entity);

			p_Total++;
		}
	}

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];


		if (tempIndex > 0)
		{
			p_Total--;
			m_Controller[p_Total] = nullptr;
			m_Motion[p_Total] = nullptr;

			if (p_Total > 1)
			{
				//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Controller[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}

private:
	std::array<Frosty::ECS::CController*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Controller;
	std::array<Frosty::ECS::CMotion*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Motion;

};

class FollowSystem : public Frosty::ECS::BaseSystem
{
public:
	FollowSystem() = default;
	virtual ~FollowSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CFollow>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMotion>(), true);
	}

	inline virtual void OnUpdate() override
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Follow[i]->Target)
			{
				float distance = glm::distance(m_Transform[i]->Position, m_Follow[i]->Target->Position);
				if (distance > m_Follow[i]->StopDistance)
				{
					m_Motion[i]->Direction = glm::normalize(m_Follow[i]->Target->Position - m_Transform[i]->Position);
				}
				else
				{
					m_Motion[i]->Direction = glm::vec3(0.0f);
				}
			}
		}
	}

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Follow[p_Total] = &world->GetComponent<Frosty::ECS::CFollow>(entity);
			m_Motion[p_Total] = &world->GetComponent<Frosty::ECS::CMotion>(entity);

			p_Total++;
		}
	}

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];


		if (tempIndex > 0)
		{
			p_Total--;
			m_Transform[p_Total] = nullptr;
			m_Follow[p_Total] = nullptr;
			m_Motion[p_Total] = nullptr;

			if (p_Total > 1)
			{
				//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CFollow*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Follow;
	std::array<Frosty::ECS::CMotion*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Motion;

};

namespace MCS
{
	Game::Game()
	{
		auto& world = Application::Get().GetWorld();

		// Add systems
		world->AddSystem<CameraSystem>();
		world->AddSystem<LightSystem>();
		world->AddSystem<RenderSystem>();
		world->AddSystem<MovementSystem>();
		world->AddSystem<PlayerControllerSystem>();
		world->AddSystem<FollowSystem>();

		// Add components
		//world->InitiateComponent<Frosty::ECS::CTransform>();
		//world->InitiateComponent<Frosty::ECS::CCamera>();

		world->Start();

		PushLayer(FY_NEW InspectorLayer());
	}

	Game::~Game()
	{

	}
}
