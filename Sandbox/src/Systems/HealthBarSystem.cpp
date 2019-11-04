#include <mcspch.hpp>
#include "HealthBarSystem.hpp"

void HealthBarSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealthBar>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
}

void HealthBarSystem::OnUpdate()
{

}

void HealthBarSystem::Render()
{
	Frosty::Renderer::GameCameraProps cam = Frosty::Renderer::GetCamera();
	auto& win = Frosty::Application::Get().GetWindow();
	glm::vec3 ndcSpacePos;
	for (size_t i = 1; i < p_Total; i++)
	{
		if (true)
		{
			float pivot = 0.5f;

			glm::vec3 offset = glm::vec3((m_Health[i]->CurrentHealth / m_Health[i]->MaxHealth) / 2, 0, 0);

			glm::vec3 TmaxHP = glm::vec3(m_Health[i]->MaxHealth, 1, 1);
			glm::vec3 TcurrHP = glm::vec3(m_Health[i]->CurrentHealth, 1, 1);

			//translate
			//world position to screen position
			glm::vec4 clipSpace = cam.ProjectionMatrix * (cam.ViewMatrix * glm::vec4(m_Transform[i]->Position + m_HealthBar[i]->BarOffset + glm::vec3(TcurrHP.x / TmaxHP.x - 1.0f, 0.0f, 0.0f), 1.0f));
			ndcSpacePos = glm::vec3(clipSpace.x / clipSpace.w, clipSpace.y / clipSpace.w, clipSpace.z / clipSpace.w);

			m_HealthBar[i]->hpTransform = glm::translate(glm::mat4{ 1.0f }, ndcSpacePos);


			//scale
			//scale calc
			float camDistance = glm::distance(Frosty::Renderer::GetCamera().CameraPosition, m_Transform[i]->Position + m_HealthBar[i]->BarOffset);
			glm::vec3 currHP = glm::vec3(m_Health[i]->CurrentHealth, 1, 1);

			m_HealthBar[i]->hpTransform = glm::scale(m_HealthBar[i]->hpTransform, (glm::vec3(((TcurrHP.x / TmaxHP.x) * 2), 1.0f, 1.0f) * (1 / camDistance)));


			//rotate
			m_HealthBar[i]->hpTransform = glm::rotate(m_HealthBar[i]->hpTransform, glm::radians(90.0f), { 1.0f, 0.0f, 0.0f });

		}

		if (false)
		{
			float pivot = 0.5f;

			//translate
			//world position to screen position
			float HP = 1.0f;
			glm::vec3 TmaxHP = glm::vec3(m_Health[i]->MaxHealth, 1, 1);
			glm::vec3 TcurrHP = glm::vec3(m_Health[i]->CurrentHealth, 1, 1);

			m_HealthBar[i]->hpTransform = glm::translate(glm::mat4{ 1.0f }, glm::vec3(TcurrHP.x / TmaxHP.x - 1.0f, 0.0f, 0.0f));


			m_HealthBar[i]->hpTransform = glm::scale(m_HealthBar[i]->hpTransform, glm::vec3((TcurrHP.x / TmaxHP.x) * 2, 1.0f, 1.0f));


			//rotate
			m_HealthBar[i]->hpTransform = glm::rotate(m_HealthBar[i]->hpTransform, glm::radians(90.0f), { 1.0f, 0.0f, 0.0f });
		}
		if (m_HealthBar[i]->Texture && m_HealthBar[i]->UseShader->GetName() == "UI")
		{
			m_HealthBar[i]->Texture->Bind(0);
		}

		Frosty::Renderer::Submit2d(m_HealthBar[i]->Texture.get(), m_HealthBar[i]->UseShader.get(), m_HealthBar[i]->Mesh, m_HealthBar[i]->hpTransform);

		if (m_HealthBar[i]->UseShader->GetName() == "UI" && m_HealthBar[i]->Texture) m_HealthBar[i]->Texture->Unbind();
	}
}

void HealthBarSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
		m_HealthBar[p_Total] = &world->GetComponent<Frosty::ECS::CHealthBar>(entity);
		m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);

		if (!m_HealthBar[p_Total]->Mesh)
		{
			m_HealthBar[p_Total]->Mesh = Frosty::AssetManager::GetMesh("Plane");
			m_HealthBar[p_Total]->UseShader = Frosty::AssetManager::GetShader("UI");
			m_HealthBar[p_Total]->Texture = Frosty::AssetManager::GetTexture2D("Red");
		}

		p_Total++;
	}
}

void HealthBarSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

	if (tempIndex > 0)
	{
		p_Total--;
		m_Transform[p_Total] = nullptr;
		m_HealthBar[p_Total] = nullptr;
		//m_Health[p_Total] = nullptr;

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