#include <mcspch.hpp>
#include "Game.hpp"
#include "Frosty/API/AssetManager.hpp"

class CameraSystem : public Frosty::ECS::BaseSystem
{
public:
	CameraSystem() = default;
	virtual ~CameraSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCamera>(), true);

		//Needed for testing
		//Frosty::PrefabManager::GetPrefabManager()->SetPrefab("StorKlocka", "klocka","Mat_0:newClock");
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
				m_Transform[i]->Position = m_Cameras[i]->Target->Position + glm::vec3(0.0f, 40.0f, 25.0f);
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

	virtual void BeginScene() override
	{
		if (p_Total > 1)
		{
			for (size_t i = 1; i < p_Total; i++)
			{
				Frosty::RenderCommand::SetClearColor(glm::vec4(m_Cameras[i]->Background, 1.0f));
				Frosty::RenderCommand::Clear();
				Frosty::Renderer::BeginScene();
				Frosty::Renderer::SetCamera(m_Transform[i]->Position, m_Cameras[i]->ViewProjectionMatrix);
			}
		}
		else
		{
			Frosty::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			Frosty::RenderCommand::Clear();
			Frosty::Renderer::BeginScene();
			Frosty::Renderer::SetCamera(glm::vec3(0.0f, 0.0f, -5.0f), glm::mat4(1.0f));
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
	}

	virtual void Render() override
	{

		auto& win = Frosty::Application::Get().GetWindow();

		if (p_Total > 1)
		{
			// Optimization: Could send in to Renderer how many point and directional lights we have to reserve that space in vectors.
			for (size_t i = 1; i < p_Total; i++)
			{
				if (m_Light[i]->Type == Frosty::ECS::CLight::Point)
				{
					Frosty::Renderer::AddLight(m_Light[i]->Color, m_Transform[i]->Position, m_Light[i]->Strength, m_Light[i]->Radius);
				}
				else if (m_Light[i]->Type == Frosty::ECS::CLight::Directional)
				{
					Frosty::Renderer::AddLight(m_Light[i]->Color, m_Transform[i]->Rotation, m_Light[i]->Strength);
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

			// Check if directional light
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Transform[i]->Position);
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.x), { 1.0f, 0.0f, 0.0f });
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.z), { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, m_Transform[i]->Scale);
			
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->DiffuseTexture)m_Materials[i]->DiffuseTexture->Bind();
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Bind(1);
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Bind(2);

			Frosty::Renderer::Submit(m_Materials[i], m_Meshes[i]->Mesh, transform);

			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Unbind();
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
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCollision>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTag>(), true);
	}

	inline virtual void OnUpdate() override
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			m_Transform[i]->Position += m_Motion[i]->Velocity * Frosty::Time::DeltaTime();
			//m_Transform[i]->Position += m_Motion[i]->Direction * m_Motion[i]->Speed * Frosty::Time::DeltaTime();

			for (size_t j = 1; j < p_Total; j++)
			{
				if (j != i)
				{
					if (m_Tag[i]->TagName != m_Tag[j]->TagName)
					{
						glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1], m_Collision[i]->BoundingBox->pos[2]);
						glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
						glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0], m_Collision[i]->BoundingBox->halfSize[1], m_Collision[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;
						glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1], m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;
						glm::vec3 offset = Frosty::CollisionDetection::AABBIntersecPushback(finalLengthA, finalCenterA, finalLengthB, finalCenterB);

						m_Transform[i]->Position -= offset;
					}
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
			m_Motion[p_Total] = &world->GetComponent<Frosty::ECS::CMotion>(entity);
			m_Collision[p_Total] = &world->GetComponent<Frosty::ECS::CCollision>(entity);
			m_Tag[p_Total] = &world->GetComponent<Frosty::ECS::CTag>(entity);

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
			m_Collision[p_Total] = nullptr;

			if (p_Total > 1)
			{
			//	std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

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
	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;
	//Temp
	std::array<Frosty::ECS::CTag*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Tag;

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
			// Reset
			m_Motion[i]->Velocity = glm::vec3(0.0f);

			// Rotate based on mouse to world position
			glm::vec4 viewport = Frosty::Application::Get().GetWindow().GetViewport();
			auto& gameCameraEntity = Frosty::Application::Get().GetWorld()->GetSceneCamera();
			auto& gameCameraComp = Frosty::Application::Get().GetWorld()->GetComponent<Frosty::ECS::CCamera>(gameCameraEntity);
			glm::vec2 mousePos = glm::vec2(Frosty::InputManager::GetMouseX(), Frosty::InputManager::GetMouseY());

			// Convert from viewport to NDC
			glm::vec2 NDC = glm::vec2(
				(2.0f * mousePos.x) / viewport.z - 1.0f,
				1.0f - (2.0f * mousePos.y) / viewport.w
			);

			// Convert from NDC to clip
			glm::vec4 clipRayCoords = glm::vec4(NDC.x, NDC.y, -1.0f, 1.0f);

			// Convert from clip to eye
			glm::vec4 eyeRayCoords = glm::inverse(gameCameraComp.ProjectionMatrix) * clipRayCoords;
			eyeRayCoords .z = -1.0f;
			eyeRayCoords .w = 0.0f;			// Was 1.0f

			// Convert from eye to world and then normalize)
			glm::vec4 worldRayCoords = glm::inverse(gameCameraComp.ViewMatrix) * eyeRayCoords;
			glm::vec3 mouseWorldRay = normalize(glm::vec3(worldRayCoords));
			FY_TRACE("({0}, {1}, {2})", mouseWorldRay.x, mouseWorldRay.y, mouseWorldRay.z);

			// Now we got a normalized vector from our screen position. Use this to find point in 3D space

			// Calculate direction based on rotation
			CalculateDirection(i);

			if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveForwardKey))
			{
				m_Motion[i]->Velocity = m_Motion[i]->Direction * m_Motion[i]->Speed;
			}
			else if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveBackKey))
			{
				m_Motion[i]->Velocity = -m_Motion[i]->Direction * m_Motion[i]->Speed;
			}
			if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveLeftKey))
			{
				m_Motion[i]->Velocity = -glm::cross(m_Motion[i]->Direction, glm::vec3(0.0f, 1.0f, 0.0f)) * m_Motion[i]->Speed;
			}
			else if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveRightKey))
			{
				m_Motion[i]->Velocity = glm::cross(m_Motion[i]->Direction, glm::vec3(0.0f, 1.0f, 0.0f)) * m_Motion[i]->Speed;
			}
		}
	}
	
	inline virtual void OnUpdate() override
	{

	}

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
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
			m_Transform[p_Total] = nullptr;
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
	void CalculateDirection(size_t index) 
	{
		glm::mat4 mat = glm::mat4(1.0f);
		mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.x), { 1.0f, 0.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.y), { 0.0f, 1.0f, 0.0f });
		mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.z), { 0.0f, 0.0f, 1.0f });

		m_Motion[index]->Direction = glm::normalize(mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0));
	}

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
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

class CollisionSystem : public Frosty::ECS::BaseSystem
{
public:
	CollisionSystem() = default;
	virtual ~CollisionSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCollision>(), true);
	}

	inline virtual void OnUpdate() override
	{
		for (size_t i = 1; i < p_Total; i++)
		{
		}
	}

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Collision[p_Total] = &world->GetComponent<Frosty::ECS::CCollision>(entity);

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
			m_Collision[p_Total] = nullptr;

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
	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;

};

float randValue(int max, int min);

namespace MCS
{


	void generateTrees();
	void generateBorders();
	void generatePlanes();
	void generateMapMarkers();
	void generateLights();

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
		world->AddSystem<CollisionSystem>();

		// Add components
		//world->InitiateComponent<Frosty::ECS::CTransform>();
		//world->InitiateComponent<Frosty::ECS::CCamera>();

		world->Start();

		//auto& plane = world->CreateEntity();
		//auto& planeTransform = world->GetComponent<Frosty::ECS::CTransform>(plane);
		//planeTransform.Scale = glm::vec3(100.0f, 1.0f, 100.0f);
		//world->AddComponent<Frosty::ECS::CMesh>(plane, Frosty::AssetManager::GetMesh("Plane"));
		//auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(plane, Frosty::AssetManager::GetShader("Texture2D"));
		//planeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Brown Mud Diffuse");
		
		auto& light = world->CreateEntity();
		auto& lightTransform = world->GetComponent<Frosty::ECS::CTransform>(light);
		lightTransform.Rotation = glm::vec3(0.0f, 0.0f, 1.0f);
		//world->AddComponent<Frosty::ECS::CMesh>(light, Frosty::AssetManager::GetMesh("Cube"));
		//world->AddComponent<Frosty::ECS::CMaterial>(light, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional);
		
		auto& player = world->CreateEntity();
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		playerTransform.Position.y = 1.0f;
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("Cylinder"));
		world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CMotion>(player, 5.0f);
		world->AddComponent<Frosty::ECS::CController>(player);
		world->AddComponent<Frosty::ECS::CCollision>(player, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CTag>(player, "Player");

		std::srand((unsigned)std::time(0));


		generateTrees();
		generateBorders();
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

		auto& world = Frosty::Application::Get().GetWorld();
		uint8_t mapLength = 192;
		uint8_t mapDepth = 108;

		float HB_Y_pos = 1;
		float middleWidth = 0.5;
		float middleDepth = 5;
		float HitboxMapInflunce = 0.15;


		//Bot Hitbox
		auto& BorderBox_1 = world->CreateEntity();
		auto& BorderTransform_1 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_1);
		BorderTransform_1.Position = glm::vec3(mapLength / 2, 1.0f, mapDepth / 2);
		BorderTransform_1.Scale = glm::vec3(mapLength * 2, 1.0f, mapDepth * HitboxMapInflunce);

		//world->AddComponent<Frosty::ECS::CMesh>(BorderBox_1, Frosty::AssetManager::GetMesh("Cube"));
		//auto& BorderBoxMaterial_1 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_1, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_1, 0.0f);
		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_1, Frosty::AssetManager::GetBoundingBox("Cube"));
		//BorderBoxMaterial_1.Albedo = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_1, "Border");


		//Top Hitbox
		auto& BorderBox_2 = world->CreateEntity();
		auto& BorderTransform_2 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_2);
		BorderTransform_2.Position = glm::vec3(mapLength / 2, 1.0f, -mapDepth / 2);
		BorderTransform_2.Scale = glm::vec3(mapLength * 2, 1.0f, mapDepth * HitboxMapInflunce);

		//world->AddComponent<Frosty::ECS::CMesh>(BorderBox_2, Frosty::AssetManager::GetMesh("Cube"));
		//auto& BorderBoxMaterial_2 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_2, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_2, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_2, 0.0f);
		//BorderBoxMaterial_2.Albedo = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_2, "Border");

		//Left Hitbox
		auto& BorderBox_3 = world->CreateEntity();
		auto& BorderTransform_3 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_3);
		BorderTransform_3.Position = glm::vec3(-mapLength / 2, 1.0f, 0.0);
		BorderTransform_3.Scale = glm::vec3(mapLength * HitboxMapInflunce, 1.0f, mapDepth);

		//world->AddComponent<Frosty::ECS::CMesh>(BorderBox_3, Frosty::AssetManager::GetMesh("Cube"));
		//auto& BorderBoxMaterial_3 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_3, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_3, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_3, 0.0f);
		//BorderBoxMaterial_3.Albedo = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_3, "Border");


		//Right Hitbox
		auto& BorderBox_4 = world->CreateEntity();
		auto& BorderTransform_4 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_4);
		BorderTransform_4.Position = glm::vec3(mapLength * 1.5, 1.0f, 0.0);
		BorderTransform_4.Scale = glm::vec3(mapLength * HitboxMapInflunce, 1.0f, mapDepth);

		//world->AddComponent<Frosty::ECS::CMesh>(BorderBox_4, Frosty::AssetManager::GetMesh("Cube"));
		//auto& BorderBoxMaterial_4 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_4, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_4, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_4, 0.0f);
		//BorderBoxMaterial_4.Albedo = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_4, "Border");



		//Middle Top Hitbox
		auto& BorderBox_5 = world->CreateEntity();
		auto& BorderTransform_5 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_5);
		BorderTransform_5.Position = glm::vec3(mapLength / 2, 1.0f, mapDepth / 2);
		BorderTransform_5.Scale = glm::vec3(mapLength * HitboxMapInflunce * middleWidth, 1.0f, mapDepth * HitboxMapInflunce * middleDepth);

		//world->AddComponent<Frosty::ECS::CMesh>(BorderBox_5, Frosty::AssetManager::GetMesh("Cube"));
		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_5, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_5, 0.0f);
		//auto& BorderBoxMaterial_5 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_5, Frosty::AssetManager::GetShader("FlatColor"));
		//BorderBoxMaterial_5.Albedo = glm::vec4(0.3f, 0.4f, 0.8f, 1.0f);
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_5, "Border");


		//Middle Bot Hitbox
		auto& BorderBox_6 = world->CreateEntity();
		auto& BorderTransform_6 = world->GetComponent<Frosty::ECS::CTransform>(BorderBox_6);
		BorderTransform_6.Position = glm::vec3(mapLength / 2, 1.0f, -mapDepth / 2);
		BorderTransform_6.Scale = glm::vec3(mapLength * HitboxMapInflunce * middleWidth, 1.0f, mapDepth * HitboxMapInflunce * middleDepth);

		//	world->AddComponent<Frosty::ECS::CMesh>(BorderBox_6, Frosty::AssetManager::GetMesh("Cube"));
	//		auto& BorderBoxMaterial_6 = world->AddComponent<Frosty::ECS::CMaterial>(BorderBox_6, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CCollision>(BorderBox_6, Frosty::AssetManager::GetBoundingBox("Cube"));
		world->AddComponent<Frosty::ECS::CMotion>(BorderBox_6, 0.0f);
		//	BorderBoxMaterial_6.Albedo = glm::vec4(0.3f, 0.4f, 0.8f, 1.0f);
		world->AddComponent<Frosty::ECS::CTag>(BorderBox_6, "Border");


	}

	void generatePlanes()
	{
		auto& world = Frosty::Application::Get().GetWorld();

		uint8_t mapLength = 192;
		uint8_t mapDepth = 108;

		//Planes

		auto& PlaneOne = world->CreateEntity();
		auto& testTranform = world->GetComponent<Frosty::ECS::CTransform>(PlaneOne);
		testTranform.Scale = glm::vec3(mapLength, 0.0f, mapDepth);
		world->AddComponent<Frosty::ECS::CMesh>(PlaneOne, Frosty::AssetManager::GetMesh("Plane"));
		auto& testMaterial = world->AddComponent<Frosty::ECS::CMaterial>(PlaneOne, Frosty::AssetManager::GetShader("Texture2D"));
		testMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
		testMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Brown Mud Diffuse");



		auto& PlaneTwo = world->CreateEntity();
		auto& testTranformTwo = world->GetComponent<Frosty::ECS::CTransform>(PlaneTwo);
		testTranformTwo.Scale = glm::vec3(mapLength, 0.0f, mapDepth);
		testTranformTwo.Position = glm::vec3(mapLength, 0.0f, 0.0f);
		world->AddComponent<Frosty::ECS::CMesh>(PlaneTwo, Frosty::AssetManager::GetMesh("Plane"));
		auto& testMaterialTwo = world->AddComponent<Frosty::ECS::CMaterial>(PlaneTwo, Frosty::AssetManager::GetShader("Texture2D"));
		testMaterialTwo.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
		testMaterialTwo.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Brown Mud Diffuse");
		testMaterialTwo.NormalTexture = Frosty::AssetManager::GetTexture2D("Rusty Metal Normal");
		testMaterialTwo.SpecularTexture = Frosty::AssetManager::GetTexture2D("Rusty Metal Normal");





		auto& PlaneThree = world->CreateEntity();
		auto& testTranformThree = world->GetComponent<Frosty::ECS::CTransform>(PlaneThree);
		testTranformThree.Position = glm::vec3(0.0f, -0.01f, 0.0f);
		testTranformThree.Scale = glm::vec3(1000.0f, 0.0f, 1000.0f);
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

}

float randValue(int max, int min)
{



	int dif = max - min;

	float returnValue = std::rand() % dif;

	returnValue += min;


	return returnValue;
}
