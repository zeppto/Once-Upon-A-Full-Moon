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
			if (m_Materials[i]->UseShader->GetName() == "UI" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Bind(0);

			Frosty::Renderer::Submit(m_Materials[i], m_Meshes[i]->Mesh, transform);

			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "UI" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Unbind();
		
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
			//for arrow
			if (m_Tag[i]->TagName == "arrow" || m_Tag[i]->TagName == "strongArrow")
			{
				m_Motion[i]->Velocity = m_Motion[i]->Direction * m_Motion[i]->Speed;
				m_Transform[i]->Position += m_Motion[i]->Velocity * Frosty::Time::DeltaTime();
			}
			else
			{


				if (m_Motion[i]->DashActive)
				{
					m_Motion[i]->DistanceDashed += glm::length(m_Motion[i]->Velocity * Frosty::Time::DeltaTime());
					if (m_Motion[i]->DistanceDashed >= m_Motion[i]->DASH_DISTANCE / 1000.0f)
					{
						m_Motion[i]->DashActive = false;
						m_Motion[i]->DistanceDashed = 0.0f;
					}
				}
				m_Transform[i]->Position += m_Motion[i]->Velocity * Frosty::Time::DeltaTime();

				for (size_t j = 1; j < p_Total; j++)
				{
					if (j != i)
					{
						//temp
						if (m_Tag[i]->TagName != m_Tag[j]->TagName || m_Tag[i]->TagName == "Enemy")
							//	if (m_Tag[i]->TagName != m_Tag[j]->TagName)
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
			if (m_Motion[i]->DashCurrentCooldown > 0.0) m_Motion[i]->DashCurrentCooldown -= Frosty::Time::DeltaTime();

			// Check dash status
			if (!m_Motion[i]->DashActive)
			{
				// Reset
				m_Motion[i]->Velocity = glm::vec3(0.0f);

				// Get the point on our terrain
				glm::vec3 point3D = ScreenToTerrainPoint();

				// Look at point
				LookAtPoint(point3D, i);

				// Calculate direction based on rotation
				//CalculateDirection(i);

				m_Motion[i]->Direction = glm::vec3(0.0f, 0.0f, 0.0f);
				if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveForwardKey))
				{
					m_Motion[i]->Direction.z = -1.0f;
					if (Frosty::InputManager::IsKeyPressed(FY_KEY_LEFT_SHIFT) && m_Motion[i]->DashCurrentCooldown <= 0.0f)
					{
						m_Motion[i]->DashActive = true;
					}
				}
				else if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveBackKey))
				{
					m_Motion[i]->Direction.z = 1.0f;
					if (Frosty::InputManager::IsKeyPressed(FY_KEY_LEFT_SHIFT) && m_Motion[i]->DashCurrentCooldown <= 0.0f)
					{
						m_Motion[i]->DashActive = true;
					}
				}
				if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveLeftKey))
				{
					m_Motion[i]->Direction.x = -1.0f;
					if (Frosty::InputManager::IsKeyPressed(FY_KEY_LEFT_SHIFT) && m_Motion[i]->DashCurrentCooldown <= 0.0f)
					{
						m_Motion[i]->DashActive = true;
					}
				}
				else if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveRightKey))
				{
					m_Motion[i]->Direction.x = 1.0f;
					if (Frosty::InputManager::IsKeyPressed(FY_KEY_LEFT_SHIFT) && m_Motion[i]->DashCurrentCooldown <= 0.0f)
					{
						m_Motion[i]->DashActive = true;
					}
				}

				if (glm::length(m_Motion[i]->Direction) > 0.0f)
				{
					m_Motion[i]->Velocity = m_Motion[i]->Direction * m_Motion[i]->Speed;
					if (m_Motion[i]->DashActive)
					{
						m_Motion[i]->Velocity *= m_Motion[i]->DashSpeedMultiplier;
						m_Motion[i]->DashCurrentCooldown = m_Motion[i]->DASH_COOLDOWN / 1000.0f;
					}
				}
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
	glm::vec3 ScreenToTerrainPoint()
	{
		glm::vec4 viewport = Frosty::Application::Get().GetWindow().GetViewport();
		auto& gameCameraEntity = Frosty::Application::Get().GetWorld()->GetSceneCamera();
		auto& gameCameraTransformComp = Frosty::Application::Get().GetWorld()->GetComponent<Frosty::ECS::CTransform>(gameCameraEntity);
		auto& gameCameraCamerComp = Frosty::Application::Get().GetWorld()->GetComponent<Frosty::ECS::CCamera>(gameCameraEntity);
		glm::vec2 mousePos = glm::vec2(Frosty::InputManager::GetMouseX(), Frosty::InputManager::GetMouseY());

		// Todo: Make it work even in editor mode

		// Convert from viewport to NDC
		glm::vec2 NDC = glm::vec2(
			(2.0f * mousePos.x) / viewport.z - 1.0f,
			(2.0f * mousePos.y) / viewport.w - 1.0f
		);

		// Convert from NDC to clip
		glm::vec4 clipRayCoords = glm::vec4(NDC.x, NDC.y, -1.0f, 1.0f);

		// Convert from clip to eye
		glm::vec4 eyeRayCoords = glm::inverse(gameCameraCamerComp.ProjectionMatrix) * clipRayCoords;
		eyeRayCoords.z = -1.0f;
		eyeRayCoords.w = 0.0f;

		// Convert from eye to world (and then normalize)
		glm::vec4 worldRayCoords = glm::inverse(gameCameraCamerComp.ViewMatrix) * eyeRayCoords;
		glm::vec3 mouseWorldRay = normalize(glm::vec3(worldRayCoords));

		// Now we got a normalized vector from our screen position. Use this to find point in 3D space
		float div = glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), mouseWorldRay);
		float t = (1.0f - glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), gameCameraTransformComp.Position)) / div;
		glm::vec3 point3D = t * mouseWorldRay + gameCameraTransformComp.Position;

		return point3D;
	}

	void LookAtPoint(const glm::vec3& point, size_t index)
	{

		// Rotate the player to look towards the mouse (point3D)
		glm::vec3 pointVector = glm::normalize(point - m_Transform[index]->Position);
		glm::vec3 originDirection = glm::vec3(0.0f, 0.0f, 1.0f);
		float extraRotation = 0.0f;
		if (point.x <= m_Transform[index]->Position.x)
		{
			originDirection.z = -1.0f;
			extraRotation = 180.0f;
		}
		float product = glm::dot(glm::normalize(originDirection), pointVector);

		float rotationOffset = glm::degrees(glm::acos(product)) + extraRotation;


		m_Transform[index]->Rotation.y = rotationOffset;
	}

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
					m_Motion[i]->Velocity = glm::normalize(m_Follow[i]->Target->Position - m_Transform[i]->Position) * m_Motion[i]->Speed;
				}
				else
				{
					m_Motion[i]->Velocity = glm::vec3(0.0f);
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

//ska �ndras senare !
class PlayerAttackSystem : public Frosty::ECS::BaseSystem
{
public:
	PlayerAttackSystem() = default;
	virtual ~PlayerAttackSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCollision>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPlayerAttack>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
	}

	inline virtual void OnInput() override
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			//FY_TRACE("playerAttack ({0})", i);

			//tips hoger ar fram utan rotation
			if (m_PlayerAttack[i]->IsPlayer)
			{
				m_PlayerAttack[i]->Cooldown += Frosty::Time::DeltaTime();
				//FY_TRACE("cooldown ({0})", m_PlayerAttack[i]->Cooldown);
				if (Frosty::InputManager::IsKeyReleased(FY_KEY_ENTER))
				{
					m_CanSwitchWepond = true;
				}
				if (Frosty::InputManager::IsKeyPressed(FY_KEY_ENTER) && m_CanSwitchWepond)
				{
					m_CanSwitchWepond = false;
					if (m_PlayerAttack[i]->IsMelee)
						m_PlayerAttack[i]->IsMelee = false;
					else
						m_PlayerAttack[i]->IsMelee = true;
				}

				if (m_PlayerAttack[i]->IsMelee)
				{
					//attack

					if (Frosty::InputManager::IsMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT))
					{
						m_CanAttackNormal = true;
					}
					if (Frosty::InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
					{
						for (size_t j = 1; j < p_Total; j++)
						{
							if (j != i && m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackNormal)
							{
								m_CanAttackNormal = false;
								m_PlayerAttack[i]->Cooldown = -0.20f;

								glm::mat4 rotationMat(1.0f);
								rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
								//rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_PlayerAttack[i]->Reach / 2)));
								rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach)));
								glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

								glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1],
									m_Collision[i]->BoundingBox->pos[2]) + hitboxPos;
								glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
								glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach,
									m_Collision[i]->BoundingBox->halfSize[1] * m_PlayerAttack[i]->Reach, m_Collision[i]->BoundingBox->halfSize[2] * m_PlayerAttack[i]->Reach) * m_Transform[i]->Scale;
								glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1],
									m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;
								if (Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB))
								{
									FY_TRACE("playerAttack ({0})", j);
									m_Health[j]->CurrentHealth -= m_PlayerAttack[i]->Damage;
									FY_TRACE("current health ({0})", m_Health[j]->CurrentHealth);
									//nockBack
									//enemy sak ta hand om sin egen push back, har sak bara skickas till enemyn att du ska push backa
									m_Transform[j]->Position.x -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).x * 3.0f;
									m_Transform[j]->Position.z -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).z * 3.0f;

								}
							}
						}
					}
					//area attack 
					if (Frosty::InputManager::IsMouseButtonReleased(GLFW_MOUSE_BUTTON_RIGHT))
					{
						m_CanAttackArea = true;
					}
					if (Frosty::InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
					{
						for (size_t j = 1; j < p_Total; j++)
						{
							if (j != i && m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackArea)
							{
								m_CanAttackArea = false;
								m_PlayerAttack[i]->Cooldown = -0.50f;

								//temp *2 make it better
								glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1],
									m_Collision[i]->BoundingBox->pos[2]);
								glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
								glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0],
									m_Collision[i]->BoundingBox->halfSize[1], m_Collision[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale * glm::vec3(2.50f);
								glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1],
									m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;
								if (Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB))
								{
									FY_TRACE("playerAttack ({0})", j);
									m_Health[j]->CurrentHealth -= m_PlayerAttack[i]->Damage / 2;
									FY_TRACE("current health ({0})", m_Health[j]->CurrentHealth);

									//nockBack
									//enemy sak ta hand om sin egen push back, har sak bara skickas till enemyn att du ska push backa
									m_Transform[j]->Position.x -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).x * 4.0f;
									m_Transform[j]->Position.z -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).z * 4.0f;

								}
							}
						}
					}
					//strong slim attack 
					if (Frosty::InputManager::IsKeyReleased(FY_KEY_SPACE))
					{
						m_CanAttackStrong = true;
					}
					if (Frosty::InputManager::IsKeyPressed(FY_KEY_SPACE))
					{
						for (size_t j = 1; j < p_Total; j++)
						{
							if (j != i && m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackStrong)
							{
								m_CanAttackStrong = false;
								m_PlayerAttack[i]->Cooldown = -1.00f;

								glm::mat4 rotationMat(1.0f);
								rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
								rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach)));
								glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

								glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
								glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1],
									m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;

								glm::vec3 rayLength = -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)) * glm::vec3(10);
								if (Frosty::CollisionDetection::AABBRayIntersect(m_Transform[i]->Position, rayLength, finalLengthB, finalCenterB))
								{
									FY_TRACE("playerAttack ({0})", j);
									m_Health[j]->CurrentHealth -= m_PlayerAttack[i]->Damage * 2;
									FY_TRACE("current health ({0})", m_Health[j]->CurrentHealth);

									//nockBack
									//enemy sak ta hand om sin egen push back, har sak bara skickas till enemyn att du ska push backa
									m_Transform[j]->Position.x -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).x * 4.0f;
									m_Transform[j]->Position.z -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).z * 4.0f;
								}
							}
						}
					}
				}
				else
				{
					//arrow
					if (Frosty::InputManager::IsMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT))
					{
						m_CanAttackNormal = true;
					}
					if (Frosty::InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
					{

						if (m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackNormal)
						{
							m_CanAttackNormal = false;
							m_PlayerAttack[i]->Cooldown = -0.30f;

							glm::mat4 rotationMat(1.0f);
							rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
							//rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_PlayerAttack[i]->Reach / 2)));
							rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach)));
							glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

							//test --- to create arrow (do a call to an other funktinon insted that adds and removes arrows)
							auto& world = Frosty::Application::Get().GetWorld();
							auto& arrow = world->CreateEntity();
							auto& arrowTransform = world->GetComponent<Frosty::ECS::CTransform>(arrow);
							arrowTransform.Position = m_Transform[i]->Position - glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)) * m_Collision[i]->BoundingBox->halfSize[0] * 2.0f * m_Transform[i]->Scale;
							arrowTransform.Scale *= 1.2f;
							arrowTransform.Rotation = m_Transform[i]->Rotation;
							world->AddComponent<Frosty::ECS::CMesh>(arrow, Frosty::AssetManager::GetMesh("Cube"));
							world->AddComponent<Frosty::ECS::CMaterial>(arrow, Frosty::AssetManager::GetShader("FlatColor"));
							auto& arrowMotion = world->AddComponent<Frosty::ECS::CMotion>(arrow, 20.0f);
							arrowMotion.Direction = -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos));
							arrowMotion.Direction.y = 0;
							FY_TRACE("playerAttack Direction({0}, {1}, {2})", -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).x, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).y, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).z);
							world->AddComponent<Frosty::ECS::CTag>(arrow, "arrow");
							world->AddComponent<Frosty::ECS::CCollision>(arrow, Frosty::AssetManager::GetBoundingBox("Cube"));
							world->AddComponent<Frosty::ECS::CHealth>(arrow, 1.0f);
							m_PlayerAttack[i]->CooldownArrow = 0;

						}
					}
					//3 arrows
					if (Frosty::InputManager::IsMouseButtonReleased(GLFW_MOUSE_BUTTON_RIGHT))
					{
						m_CanAttackArea = true;
					}
					if (Frosty::InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
					{
						if (m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackArea)
						{
							m_CanAttackArea = false;
							m_PlayerAttack[i]->Cooldown = -0.70f;

							for (int k = 0; k < 3; k++)
							{
								glm::mat4 rotationMat(1.0f);
								rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
								if (k == 0)
									rotationMat = glm::rotate(rotationMat, glm::radians(20.0f), { 0.0f, 1.0f, 0.0f });
								if (k == 2)
									rotationMat = glm::rotate(rotationMat, glm::radians(-20.0f), { 0.0f, 1.0f, 0.0f });
								//rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_PlayerAttack[i]->Reach / 2)));
								rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach)));
								glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

								//test --- to create arrow (do a call to an other funktinon insted that adds and removes arrows)
								auto& world = Frosty::Application::Get().GetWorld();
								auto& arrow = world->CreateEntity();
								auto& arrowTransform = world->GetComponent<Frosty::ECS::CTransform>(arrow);
								arrowTransform.Position = m_Transform[i]->Position - glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)) * m_Collision[i]->BoundingBox->halfSize[0] * 2.0f * m_Transform[i]->Scale;
								arrowTransform.Scale *= 1.2f;
								arrowTransform.Rotation = m_Transform[i]->Rotation;
								world->AddComponent<Frosty::ECS::CMesh>(arrow, Frosty::AssetManager::GetMesh("Cube"));
								world->AddComponent<Frosty::ECS::CMaterial>(arrow, Frosty::AssetManager::GetShader("FlatColor"));
								auto& arrowMotion = world->AddComponent<Frosty::ECS::CMotion>(arrow, 20.0f);
								arrowMotion.Direction = -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos));
								arrowMotion.Direction.y = 0;
								FY_TRACE("playerAttack Direction({0}, {1}, {2})", -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).x, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).y, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).z);
								world->AddComponent<Frosty::ECS::CTag>(arrow, "arrow");
								world->AddComponent<Frosty::ECS::CCollision>(arrow, Frosty::AssetManager::GetBoundingBox("Cube"));
								world->AddComponent<Frosty::ECS::CHealth>(arrow, 1.0f);
								m_PlayerAttack[i]->CooldownArrow = 0;
							}

						}
						m_CanAttackArea = false;
					}
					//strong arrow
					if (Frosty::InputManager::IsKeyReleased(FY_KEY_SPACE))
					{
						m_CanAttackStrong = true;
					}
					if (Frosty::InputManager::IsKeyPressed(FY_KEY_SPACE))
					{
						if (m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackStrong)
						{
							m_CanAttackStrong = false;
							m_PlayerAttack[i]->Cooldown = -1.20f;

							glm::mat4 rotationMat(1.0f);
							rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
							//rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_PlayerAttack[i]->Reach / 2)));
							rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach)));
							glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

							//test --- to create arrow (do a call to an other funktinon insted that adds and removes arrows)
							auto& world = Frosty::Application::Get().GetWorld();
							auto& arrow = world->CreateEntity();
							auto& arrowTransform = world->GetComponent<Frosty::ECS::CTransform>(arrow);
							arrowTransform.Position = m_Transform[i]->Position - glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)) * m_Collision[i]->BoundingBox->halfSize[0] * 2.0f * m_Transform[i]->Scale;
							arrowTransform.Scale *= 1.2f;
							arrowTransform.Rotation = m_Transform[i]->Rotation;
							world->AddComponent<Frosty::ECS::CMesh>(arrow, Frosty::AssetManager::GetMesh("Cube"));
							world->AddComponent<Frosty::ECS::CMaterial>(arrow, Frosty::AssetManager::GetShader("FlatColor"));
							auto& arrowMotion = world->AddComponent<Frosty::ECS::CMotion>(arrow, 20.0f);
							arrowMotion.Direction = -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos));
							arrowMotion.Direction.y = 0;
							FY_TRACE("playerAttack Direction({0}, {1}, {2})", -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).x, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).y, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).z);
							world->AddComponent<Frosty::ECS::CTag>(arrow, "strongArrow");
							world->AddComponent<Frosty::ECS::CCollision>(arrow, Frosty::AssetManager::GetBoundingBox("Cube"));
							world->AddComponent<Frosty::ECS::CHealth>(arrow, 10.0f);
							m_PlayerAttack[i]->CooldownArrow = 0;

						}
						m_CanAttackArea = false;
					}
				}
			}
		}
	}

	virtual void OnUpdate() override
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
			m_PlayerAttack[p_Total] = &world->GetComponent<Frosty::ECS::CPlayerAttack>(entity);
			m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);

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
			m_PlayerAttack[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;

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
	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;
	std::array<Frosty::ECS::CPlayerAttack*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_PlayerAttack;
	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
	bool m_CanAttackNormal = true;
	bool m_CanAttackArea = true;
	bool m_CanAttackStrong = true;
	bool m_CanSwitchWepond = false;

};

class CombatSystem : public Frosty::ECS::BaseSystem
{
public:
	CombatSystem() = default;
	virtual ~CombatSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCollision>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTag>(), true);
	}

	inline virtual void OnInput() override
	{
		for (size_t i = 1; i < p_Total; i++)
		{			
		}
	}

	virtual void OnUpdate() override
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			for (size_t j = 1; j < p_Total; j++)
			{
				if (j != i && (m_Tag[i]->TagName == "arrow" || m_Tag[i]->TagName == "strongArrow"))
				{
					//temp
					if (m_Tag[j]->TagName == "Enemy")
						//	if (m_Tag[i]->TagName != m_Tag[j]->TagName)
					{
						glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1], m_Collision[i]->BoundingBox->pos[2]);
						glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
						glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0], m_Collision[i]->BoundingBox->halfSize[1], m_Collision[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;
						glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1], m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;
						if (Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB))
						{
							if (m_Tag[i]->TagName == "arrow")
							{
								FY_TRACE("playerAttack ({0})", j);
								m_Health[j]->CurrentHealth--;
								m_Health[i]->CurrentHealth--;
								FY_TRACE("current health ({0})", m_Health[j]->CurrentHealth);
							}
							else
							{
								FY_TRACE("playerAttack ({0})", j);
								m_Health[j]->CurrentHealth -= 3;
								m_Health[i]->CurrentHealth--;
								FY_TRACE("current health ({0})", m_Health[j]->CurrentHealth);
							}

							//nockBack
							//enemy sak ta hand om sin egen push back, har sak bara skickas till enemyn att du ska push backa
							m_Transform[j]->Position.x -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).x * 3.0f;
							m_Transform[j]->Position.z -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).z * 3.0f;
						}
					}
				}
			}
			//temp death timer
			if (m_Tag[i]->TagName == "arrow" || m_Tag[i]->TagName == "strongArrow")
			{
				m_Health[i]->MaxHealth++;
				if (m_Health[i]->MaxHealth > 70)
					m_Health[i]->CurrentHealth--;
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
			m_Collision[p_Total] = &world->GetComponent<Frosty::ECS::CCollision>(entity);
			m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);
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
			m_Collision[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;
			m_Tag[p_Total] = nullptr;

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
	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;
	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
	std::array<Frosty::ECS::CTag*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Tag;

};

class EnemyAttackSystem : public Frosty::ECS::BaseSystem
{
public:
	EnemyAttackSystem() = default;
	virtual ~EnemyAttackSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCollision>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CEnemyAttack>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
	}

	inline virtual void OnInput() override
	{		
	}

	virtual void OnUpdate() override
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			bool haveHit = false;
			m_EnemyAttack[i]->Cooldown += Frosty::Time::DeltaTime();

			for (size_t j = 1; j < p_Total; j++)
			{
				if (j != i && m_EnemyAttack[i]->IsPlayer && m_EnemyAttack[i]->Cooldown > 0.30f)
				{
					//glm::mat4 rotationMat(1.0f);
					//rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
					//rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + m_Collision[i]->BoundingBox->halfSize[0]));
					//glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

					glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1], m_Collision[i]->BoundingBox->pos[2]);
					glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
					glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0], m_Collision[i]->BoundingBox->halfSize[1], m_Collision[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;
					glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1], m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;

					if (Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB))
					{
						FY_TRACE("EnemyAttack ({0})", j);
						m_Health[i]->CurrentHealth -= m_EnemyAttack[j]->Damage / 10;
						FY_TRACE("Player health ({0})", m_Health[i]->CurrentHealth);
						haveHit = true;

						glm::vec3 knockback = m_Transform[j]->Position - m_Transform[i]->Position;
						m_Transform[i]->Position.x -= glm::normalize(knockback).x * 4.0f;
						m_Transform[i]->Position.z -= glm::normalize(knockback).z * 4.0f;
					}
				}
				else if (m_EnemyAttack[i]->Cooldown < 0.20f)
				{
					m_EnemyAttack[i]->Cooldown += Frosty::Time::DeltaTime();
				}
			}
			if (haveHit)
			{
				m_EnemyAttack[i]->Cooldown = 0;
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
			m_Collision[p_Total] = &world->GetComponent<Frosty::ECS::CCollision>(entity);
			m_EnemyAttack[p_Total] = &world->GetComponent<Frosty::ECS::CEnemyAttack>(entity);
			m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);

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
			m_EnemyAttack[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;

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
	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;
	std::array<Frosty::ECS::CEnemyAttack*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_EnemyAttack;
	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;

};

class SpawnSystem : public Frosty::ECS::BaseSystem
{
public:
	SpawnSystem() = default;
	virtual ~SpawnSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTag>(), true);
	}

	inline virtual void OnInput() override	// VERY temporary
	{
		if (Frosty::InputManager::IsKeyPressed(FY_KEY_P))	// Damage+ Player
		{
			bool found = false;
			for (size_t i = 1; i < p_Total && !found; i++)
			{
				if (m_Health[i]->EntityPtr == m_Player)
				{
					m_Health[i]->CurrentHealth -= 0.5f;
					found = true;
				}
			}
		}
		else if (Frosty::InputManager::IsKeyPressed(FY_KEY_O))	// Damage+ Opponent
		{
			bool found = false;
			for (size_t i = 1; i < p_Total && !found; i++)
			{
				if (m_Health[i]->EntityPtr != m_Player)
				{
					m_Health[i]->CurrentHealth -= 0.5f;
					found = true;
				}
			}
		}
	}

	inline virtual void OnUpdate() override
	{
		//// Initiate once, shouldn't be in update loop
		//if (m_Player == nullptr)
		//{
		//	for (size_t i = 1; i < p_Total; i++)
		//	{
		//		if (m_Health[i]->EntityPtr->Bitset[7] == 1)
		//		{
		//			m_Player = m_Health[i]->EntityPtr;
		//		}
		//	}
		//}

		// If dead --> respawn
		if (m_Player == nullptr && (std::clock() - m_PlayerSpawnTimer) >= 2000)
		{
			SpawnPlayer();
		}
		else if ((p_Total - 2) < m_NrOfEnemies && (std::clock() - m_EnemySpawnTimer) >= 6000)
		{
			SpawnEnemy();
		}

		// If health below 0 --> remove
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Health[i]->CurrentHealth <= 0.f && m_Health[i]->EntityPtr == m_Player)
			{
				RemovePlayer();
			}
			else if (m_Health[i]->CurrentHealth <= 0.f)
			{
				RemoveEnemyAt(i);
			}
		}
	}

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);
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
			m_Health[p_Total] = nullptr;

			if (p_Total > 1)
			{
				//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Health[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}

private:
	void SpawnPlayer()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		auto& player = world->CreateEntity();
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		playerTransform.Position = glm::vec3(0.0f, 1.0f, 0.0f);
		playerTransform.Scale *= 2.2f;
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("Cube"));
		world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CMotion>(player, 10.0f);
		world->AddComponent<Frosty::ECS::CController>(player);
		world->AddComponent<Frosty::ECS::CConsumables>(player);
		world->AddComponent<Frosty::ECS::CHealth>(player);
		world->AddComponent<Frosty::ECS::CTag>(player,"Player");
		world->AddComponent<Frosty::ECS::CPlayerAttack>(player, 1.5f, 1.0f, 2.0f, true);
		world->AddComponent<Frosty::ECS::CEnemyAttack>(player, 1.0f, 1.0f, 2.0f, true);

		world->AddComponent<Frosty::ECS::CCollision>(player, Frosty::AssetManager::GetBoundingBox("Cube"));
		auto& gameCameraEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(gameCameraEntity).Target = &playerTransform;

		m_Player = player;
	}

	void SpawnEnemy()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		auto& enemy = world->CreateEntity();
		auto& enemyTransform = world->GetComponent<Frosty::ECS::CTransform>(enemy);

		if (m_KillCount < 10)
		{
			enemyTransform.Position = GetSpawnPosition();
			enemyTransform.Scale *= 2.f;
			world->AddComponent<Frosty::ECS::CMesh>(enemy, Frosty::AssetManager::GetMesh("Cube"));
			world->AddComponent<Frosty::ECS::CMaterial>(enemy, Frosty::AssetManager::GetShader("FlatColor"));
			world->AddComponent<Frosty::ECS::CMotion>(enemy, 7.0f);
			world->AddComponent<Frosty::ECS::CFollow>(enemy);
			world->AddComponent<Frosty::ECS::CHealth>(enemy);
			world->AddComponent<Frosty::ECS::CCollision>(enemy, Frosty::AssetManager::GetBoundingBox("Cube"));
			world->AddComponent<Frosty::ECS::CTag>(enemy, "Enemy");
			//temp
			world->AddComponent<Frosty::ECS::CPlayerAttack>(enemy);
			world->AddComponent<Frosty::ECS::CEnemyAttack>(enemy);
		}
		else
		{
			enemyTransform.Position = glm::vec3((float)TERRAIN_WIDTH * 6.f, 2.f, 0.f);
			enemyTransform.Scale *= 6.f;
			world->AddComponent<Frosty::ECS::CMesh>(enemy, Frosty::AssetManager::GetMesh("Cube"));
			world->AddComponent<Frosty::ECS::CMaterial>(enemy, Frosty::AssetManager::GetShader("FlatColor"));
			world->AddComponent<Frosty::ECS::CMotion>(enemy, 9.0f);
			world->AddComponent<Frosty::ECS::CFollow>(enemy);
			world->AddComponent<Frosty::ECS::CHealth>(enemy, 30.0f);
			world->AddComponent<Frosty::ECS::CCollision>(enemy, Frosty::AssetManager::GetBoundingBox("Cube"));
			world->AddComponent<Frosty::ECS::CTag>(enemy, "Enemy");
			//temp
			world->AddComponent<Frosty::ECS::CPlayerAttack>(enemy);
			world->AddComponent<Frosty::ECS::CEnemyAttack>(enemy);

			m_NrOfEnemies = 1;
		}

		if (m_Player != nullptr)
		{
			auto& comp = world->GetComponent<Frosty::ECS::CFollow>(enemy);
			comp.Target = &world->GetComponent<Frosty::ECS::CTransform>(m_Player);
		}

		m_EnemySpawnTimer = float(std::clock());
	}

	void RemovePlayer()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		world->RemoveEntity(m_Player);
		m_Player = nullptr;

		RemoveAllEnemies();

		m_KillCount = 0;
		m_NrOfEnemies = 2;
		m_PlayerSpawnTimer = float(std::clock());
	}

	void RemoveEnemyAt(size_t position)
	{
		m_Entity = m_Health[position]->EntityPtr;
		auto& world = Frosty::Application::Get().GetWorld();
		world->RemoveEntity(m_Entity);

		if (m_Tag[position]->TagName == "Enemy")
		{
			m_EnemySpawnTimer = float(std::clock());
			m_KillCount++;
			m_NrOfEnemies--;

			if (m_NrOfEnemies == 0)
			{
				m_NrOfEnemies = m_KillCount + 1;
			}
		}
	}

	void RemoveAllEnemies()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Health[i]->EntityPtr)
			{
				m_Entity = m_Health[i]->EntityPtr;
				auto& world = Frosty::Application::Get().GetWorld();
				world->RemoveEntity(m_Entity);
				i = 0;
			}
		}
		m_EnemySpawnTimer = float(std::clock());
	}

	const glm::vec3 GetSpawnPosition()
	{
		float x = float(rand() % (2 * TERRAIN_WIDTH) - TERRAIN_WIDTH);
		float z = float(rand() % TERRAIN_HEIGHT - float(TERRAIN_HEIGHT * 0.5f));

		return glm::vec3(x, 1.5f, z);
	}

private:
	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
	std::array<Frosty::ECS::CTag*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Tag;

	std::shared_ptr<Frosty::ECS::Entity> m_Player = nullptr;
	std::shared_ptr<Frosty::ECS::Entity> m_Entity = nullptr;

	float m_PlayerSpawnTimer;
	float m_EnemySpawnTimer;

	// Temp
	int m_KillCount = 0;
	int m_NrOfEnemies = 2;			// numbers of enemies simultaniously on the terrain. Start with 2 enemies
	const int TERRAIN_WIDTH = 45;
	const int TERRAIN_HEIGHT = 89;
};

class ConsumablesSystem : public Frosty::ECS::BaseSystem
{
public:
	ConsumablesSystem() = default;
	virtual ~ConsumablesSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CConsumables>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMotion>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
	}

	inline virtual void OnInput() override
	{
		if (Frosty::InputManager::IsKeyPressed(FY_KEY_E))
		{
			for (size_t i = 1; i < p_Total; i++)
			{
				// If consumer has healing potion AND comsumer has not full health AND healing timer is bigger than cooldown--> drink healing potion
				if ((m_Consumables[i]->CurrentNrOfHealingPotions > 0) && (m_Health[i]->CurrentHealth != m_Health[i]->MaxHealth) && ((float(std::clock()) - m_Consumables[i]->HealingTimer) * 0.001 >= m_Consumables[i]->HealingCooldown))
				{
					if (m_Consumables[i]->Heal <= (m_Health[i]->MaxHealth - m_Health[i]->CurrentHealth))
					{
						m_Health[i]->CurrentHealth += m_Consumables[i]->Heal;
					}
					else
					{
						m_Health[i]->CurrentHealth += m_Health[i]->MaxHealth - m_Health[i]->CurrentHealth;
					}

					m_Consumables[i]->CurrentNrOfHealingPotions--;
					m_Consumables[i]->HealingTimer = float(std::clock());
				}
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
			m_Consumables[p_Total] = &world->GetComponent<Frosty::ECS::CConsumables>(entity);
			m_Motion[p_Total] = &world->GetComponent<Frosty::ECS::CMotion>(entity);
			m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);

			p_Total++;
		}
	}

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];


		if (tempIndex > 0)
		{
			p_Total--;
			m_Consumables[p_Total] = nullptr;
			m_Motion[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;

			if (p_Total > 1)
			{
				//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Consumables[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}

private:
	std::array<Frosty::ECS::CConsumables*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Consumables;
	std::array<Frosty::ECS::CMotion*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Motion;
	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;

};

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
		std::srand((unsigned)std::time(0));
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

		// Add components

		//world->InitiateComponent<Frosty::ECS::CTransform>();
		//world->InitiateComponent<Frosty::ECS::CCamera>();
		world->Start();
		
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
		lightTransform.Rotation = glm::vec3(60.0f, 0.0f, -10.0f);
		world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.6f, glm::vec3(0.8f, 0.9f, 1.f));

		generateTrees();
		generateBorders();
		generatePlanes();

		bool UI = true;
		if (UI)
		{

			for (size_t i = 0; i < 3; i++)
			{
				//Endast Sprites en s� l�nge
				auto& uiHeart1 = world->CreateEntity();
				auto& uiHeart1Transform = world->GetComponent<Frosty::ECS::CTransform>(uiHeart1);
				uiHeart1Transform.Position = glm::vec3(-0.9f + (i * 0.1), 0.9f, 0.0f);

				uiHeart1Transform.Rotation = glm::vec3(90.0f, 0.0f, 0.0f);
				uiHeart1Transform.Scale = glm::vec3(0.09f, 1.0f, 0.14f);
				world->AddComponent<Frosty::ECS::CMesh>(uiHeart1, Frosty::AssetManager::GetMesh("Plane"));
				auto& uiHeart1Mat = world->AddComponent<Frosty::ECS::CMaterial>(uiHeart1, Frosty::AssetManager::GetShader("UI"));
				uiHeart1Mat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("HeartFull");
				if (i == 2)
				{
					uiHeart1Mat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Heart");
				}
			}

			auto& uiHeart1 = world->CreateEntity();
			auto& uiHeart1Transform = world->GetComponent<Frosty::ECS::CTransform>(uiHeart1);
			uiHeart1Transform.Position = glm::vec3(-0.8f, -0.7f, 0.0f);

			uiHeart1Transform.Rotation = glm::vec3(90.0f, 0.0f, 0.0f);
			uiHeart1Transform.Scale = glm::vec3(0.2f, 1.0f, 0.3f);
			world->AddComponent<Frosty::ECS::CMesh>(uiHeart1, Frosty::AssetManager::GetMesh("Plane"));
			auto& uiHeart1Mat = world->AddComponent<Frosty::ECS::CMaterial>(uiHeart1, Frosty::AssetManager::GetShader("UI"));
			uiHeart1Mat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Sword");
		}

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

		float HB_Y_pos = 1;
		float HB_Y_scale = 100.0f;
		float middleWidth = 0.5;
		float middleDepth = 5;
		float HitboxMapInflunce = 0.15;


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

		uint8_t mapLength = 192;
		uint8_t mapDepth = 108;

		//Planes

		auto& PlaneOne = world->CreateEntity();
		auto& testTranform = world->GetComponent<Frosty::ECS::CTransform>(PlaneOne);
		testTranform.Scale = glm::vec3(mapLength, 1.0f, mapDepth);
		world->AddComponent<Frosty::ECS::CMesh>(PlaneOne, Frosty::AssetManager::GetMesh("Plane"));
		auto& testMaterial = world->AddComponent<Frosty::ECS::CMaterial>(PlaneOne, Frosty::AssetManager::GetShader("Texture2D"));
		testMaterial.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
		testMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Brown Mud Diffuse");



		auto& PlaneTwo = world->CreateEntity();
		auto& testTranformTwo = world->GetComponent<Frosty::ECS::CTransform>(PlaneTwo);
		testTranformTwo.Scale = glm::vec3(mapLength, 1.0f, mapDepth);
		testTranformTwo.Position = glm::vec3(mapLength, 0.0f, 0.0f);
		world->AddComponent<Frosty::ECS::CMesh>(PlaneTwo, Frosty::AssetManager::GetMesh("Plane"));
		auto& testMaterialTwo = world->AddComponent<Frosty::ECS::CMaterial>(PlaneTwo, Frosty::AssetManager::GetShader("Texture2D"));
		testMaterialTwo.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
		testMaterialTwo.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Brown Mud Diffuse");
		testMaterialTwo.NormalTexture = Frosty::AssetManager::GetTexture2D("Rusty Metal Normal");
		testMaterialTwo.SpecularTexture = Frosty::AssetManager::GetTexture2D("Rusty Metal Normal");





		auto& PlaneThree = world->CreateEntity();
		auto& testTranformThree = world->GetComponent<Frosty::ECS::CTransform>(PlaneThree);
		testTranformThree.Position = glm::vec3(0.0f, -0.05f, 0.0f);
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

