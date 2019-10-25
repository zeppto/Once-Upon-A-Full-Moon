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
			
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Bind();
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Bind(1);
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Bind(2);
			if (m_Materials[i]->UseShader->GetName() == "UI" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Bind(0);

			if (m_Materials[i]->UseShader->GetName() == "AnimShader" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Bind();
			if (m_Materials[i]->UseShader->GetName() == "AnimShader" && m_Materials[i]->GlossTexture) m_Materials[i]->GlossTexture->Bind(1);
			if (m_Materials[i]->UseShader->GetName() == "AnimShader" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Bind(2);

			if (m_Materials[i]->UseShader->GetName() == "AnimShader")
			{
				Frosty::Renderer::animSubmit(m_Materials[i], m_Meshes[i]->Mesh, transform);
			}
			else
			{
				Frosty::Renderer::Submit(m_Materials[i], m_Meshes[i]->Mesh, transform);
			}

			if (m_Materials[i]->UseShader->GetName() == "AnimShader" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "AnimShader" && m_Materials[i]->GlossTexture) m_Materials[i]->GlossTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "AnimShader" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Unbind();

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
	}

	inline virtual void OnUpdate() override
	{
		for (size_t i = 1; i < p_Total; i++)
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

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Motion[p_Total] = &world->GetComponent<Frosty::ECS::CMotion>(entity);
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
			m_Motion[p_Total] = nullptr;
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
	std::array<Frosty::ECS::CMotion*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Motion;
	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;

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

//ska ï¿½ndras senare !
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
		//p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHp>(), true);
	}

	inline virtual void OnInput() override
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			//FY_TRACE("playerAttack ({0})", i);

			//tips hoger ar fram utan rotation
			if (Frosty::InputManager::IsKeyPressed(FY_KEY_SPACE))
			{
				for (size_t j = 1; j < p_Total; j++)
				{
					if (j != i && m_PlayerAttack[i]->IsPlayer)
					{
						glm::mat4 rotationMat(1.0f);
						rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
						rotationMat = glm::translate(rotationMat, glm::vec3(m_Collision[i]->BoundingBox->halfSize[0] + (m_PlayerAttack[i]->Reach / 2), 0, 0));
						glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

						glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1], m_Collision[i]->BoundingBox->pos[2]) +
							hitboxPos;
						glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
						glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0], m_Collision[i]->BoundingBox->halfSize[1], m_Collision[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;
						glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1], m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;
						if(Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB))
							FY_TRACE("playerAttack ({0})", j);

						//m_Transform[i]->Position -= offset;
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
			//m_Hp[p_Total] = &world->GetComponent<Frosty::ECS::CHp>(entity);

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
			//m_Hp[p_Total] = nullptr;

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
	//std::array<Frosty::ECS::CHp*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Hp;

};

class SpawnSystem : public Frosty::ECS::BaseSystem
{
public:
	SpawnSystem() = default;
	virtual ~SpawnSystem() = default;

	virtual void Init() override
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
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
		playerTransform.Scale *= 0.2f;
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("3D"));
		world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CMotion>(player, 5.0f);
		world->AddComponent<Frosty::ECS::CController>(player);
		world->AddComponent<Frosty::ECS::CHealth>(player);

		world->AddComponent<Frosty::ECS::CCollision>(player, Frosty::AssetManager::GetBoundingBox("3D"));
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
			world->AddComponent<Frosty::ECS::CMotion>(enemy, 3.5f);
			world->AddComponent<Frosty::ECS::CFollow>(enemy);
			world->AddComponent<Frosty::ECS::CHealth>(enemy);
			world->AddComponent<Frosty::ECS::CCollision>(enemy, Frosty::AssetManager::GetBoundingBox("Cube"));
		}
		else
		{
			enemyTransform.Position = glm::vec3((float)TERRAIN_WIDTH, 2.f, 0.f);
			enemyTransform.Scale *= 6.f;
			world->AddComponent<Frosty::ECS::CMesh>(enemy, Frosty::AssetManager::GetMesh("Cube"));
			world->AddComponent<Frosty::ECS::CMaterial>(enemy, Frosty::AssetManager::GetShader("FlatColor"));
			world->AddComponent<Frosty::ECS::CMotion>(enemy, 4.5f);
			world->AddComponent<Frosty::ECS::CFollow>(enemy);
			world->AddComponent<Frosty::ECS::CHealth>(enemy, 30);
			world->AddComponent<Frosty::ECS::CCollision>(enemy, Frosty::AssetManager::GetBoundingBox("Cube"));
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

		m_EnemySpawnTimer = float(std::clock());
		m_KillCount++;
		m_NrOfEnemies--;

		if (m_NrOfEnemies == 0)
		{
			m_NrOfEnemies = m_KillCount + 1;
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

	const glm::vec3& GetSpawnPosition()
	{
		float x = float(rand() % (2 * TERRAIN_WIDTH) - TERRAIN_WIDTH);
		float z = float(rand() % TERRAIN_HEIGHT - float(TERRAIN_HEIGHT * 0.5f));

		return glm::vec3(x, 1.5f, z);
	}

private:
	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;

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

namespace MCS
{
	Game::Game()
	{
		auto& world = Application::Get().GetWorld();

		// Add systems
		world->AddSystem<CameraSystem>();
		world->AddSystem<LightSystem>();
		world->AddSystem<RenderSystem>();
		world->AddSystem<PlayerControllerSystem>();
		world->AddSystem<MovementSystem>();
		world->AddSystem<FollowSystem>();
		world->AddSystem<CollisionSystem>();
		world->AddSystem<PlayerAttackSystem>();
		world->AddSystem<SpawnSystem>();

		// Add components

		//world->InitiateComponent<Frosty::ECS::CTransform>();
		//world->InitiateComponent<Frosty::ECS::CCamera>();
		world->Start();



		auto& plane = world->CreateEntity();
		auto& planeTransform = world->GetComponent<Frosty::ECS::CTransform>(plane);
		planeTransform.Scale = glm::vec3(100.0f, 1.0f, 100.0f);
		world->AddComponent<Frosty::ECS::CMesh>(plane, Frosty::AssetManager::GetMesh("Plane"));
		auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(plane, Frosty::AssetManager::GetShader("Texture2D"));
		planeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Brown Mud Diffuse");
		
		auto& light = world->CreateEntity();
		auto& lightTransform = world->GetComponent<Frosty::ECS::CTransform>(light);
		lightTransform.Position = glm::vec3(0.0f, 30.0f, -6.0f);
		lightTransform.Rotation = glm::vec3(180.0f, 0.0f, 10.0f);
		//world->AddComponent<Frosty::ECS::CMesh>(light, Frosty::AssetManager::GetMesh("Cube"));
		//world->AddComponent<Frosty::ECS::CMaterial>(light, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Point, 0.6f, 200.0f);
		
		//auto& player = world->CreateEntity();
		//auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		//playerTransform.Position.y = 1.0f;
		//playerTransform.Position.z = 3.0f;
		//world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("Cylinder"));
		//world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("FlatColor"));
		//world->AddComponent<Frosty::ECS::CMotion>(player, 8.0f);
		//world->AddComponent<Frosty::ECS::CController>(player);
		//world->AddComponent<Frosty::ECS::CCollision>(player, Frosty::AssetManager::GetBoundingBox("Cylinder"));
		//world->AddComponent<Frosty::ECS::CPlayerAttack>(player, 1.0f, 1.0f, 2.0f, true);

		//auto& Enemy = world->CreateEntity();
		//auto& EnemyTransform = world->GetComponent<Frosty::ECS::CTransform>(Enemy);
		//EnemyTransform.Position.y = 1.0f;
		//world->AddComponent<Frosty::ECS::CMesh>(Enemy, Frosty::AssetManager::GetMesh("Cube"));
		//world->AddComponent<Frosty::ECS::CMaterial>(Enemy, Frosty::AssetManager::GetShader("FlatColor"));
		//world->AddComponent<Frosty::ECS::CMotion>(Enemy, 8.0f);
		//world->AddComponent<Frosty::ECS::CCollision>(Enemy, Frosty::AssetManager::GetBoundingBox("Cube"));
		//world->AddComponent<Frosty::ECS::CPlayerAttack>(Enemy, 1.0f, 1.0f, 2.0f, false);

		//auto& Enemy2 = world->CreateEntity();
		//auto& Enemy2Transform = world->GetComponent<Frosty::ECS::CTransform>(Enemy2);
		//Enemy2Transform.Position.y = 1.0f;
		//Enemy2Transform.Position.x = 1.0f;
		//world->AddComponent<Frosty::ECS::CMesh>(Enemy2, Frosty::AssetManager::GetMesh("Cube"));
		//world->AddComponent<Frosty::ECS::CMaterial>(Enemy2, Frosty::AssetManager::GetShader("FlatColor"));
		//world->AddComponent<Frosty::ECS::CMotion>(Enemy2, 8.0f);
		//world->AddComponent<Frosty::ECS::CCollision>(Enemy2, Frosty::AssetManager::GetBoundingBox("Cube"));
		//world->AddComponent<Frosty::ECS::CPlayerAttack>(Enemy2, 1.0f, 1.0f, 2.0f, false);

		bool UI = true;
		if (UI)
		{

			for (size_t i = 0; i < 3; i++)
			{
				//Endast Sprites en så länge
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
}









