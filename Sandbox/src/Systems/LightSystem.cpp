#include <mcspch.hpp>
#include "LightSystem.hpp"


namespace MCS
{
	const std::string LightSystem::NAME = "Light";

	void LightSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CLight>(), true);
	}

	void LightSystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Light[i]->Origin != nullptr)
			{
				auto& world = Frosty::Application::Get().GetWorld();

				if (world->HasComponent<Frosty::ECS::CPlayer>(m_Light[i]->Origin->EntityPtr))
				{
					//glm::vec3 point3D = ScreenToTerrainPoint();
					//ShiftTowadsPoint(point3D, i);
					m_Transform[i]->Position = m_Light[i]->Origin->Position + m_Light[i]->Offset;
				}

			}
			// Check if light has camera
			if (m_Light[i]->HasCamera)
			{
				UpdateLightCamera(i);
			}
		}
	}

	void LightSystem::BeginScene()
	{
		//Frosty::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
		//Frosty::RenderCommand::Clear();
		//Frosty::Renderer::BeginScene();
		//Frosty::Renderer::SetCamera(glm::vec3(0.0f, 0.0f, -5.0f), glm::mat4(1.0f), glm::mat4(1.0f));

		//for (size_t i = 1; i < p_Total; i++)
		//{
		//	if (m_Light[i]->Type == Frosty::ECS::CLight::LightType::Directional)
		//	{
		//		auto& win = Frosty::Application::Get().GetWindow();
		//		Frosty::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
		//		Frosty::RenderCommand::Clear();

		//		Frosty::Renderer::SetCamera(m_Transform[i]->Position, m_Light[i]->Cameras[0].ViewMatrix, m_Light[i]->Cameras[0].ProjectionMatrix);
		//	}
		//}
	}

	void LightSystem::Render()
	{
		auto& win = Frosty::Application::Get().GetWindow();

		// Optimization: Could send in to Renderer how many point and directional lights we have to reserve that space in vectors.
		for (size_t i = 1; i < p_Total; i++)
		{
			/*if (m_Light[i]->Type == Frosty::ECS::CLight::LightType::Point)
			{
				Frosty::Renderer::AddLight(m_Light[i]->Color, m_Transform[i]->Position, m_Light[i]->Strength, m_Light[i]->Radius);
			}
			else if (m_Light[i]->Type == Frosty::ECS::CLight::LightType::Directional)
			{
				Frosty::Renderer::AddLight(m_Light[i]->Color, m_Transform[i]->Rotation, m_Light[i]->Strength);
			}*/
		}
	}

	void LightSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Light[p_Total] = &world->GetComponent<Frosty::ECS::CLight>(entity);

			Frosty::Renderer::AddLight(m_Light[p_Total], m_Transform[p_Total]);


			/*if (m_Light[p_Total]->Type == Frosty::ECS::CLight::LightType::Point)
			{
				Frosty::Renderer::AddLight(m_Light[p_Total]->Color, m_Transform[p_Total]->Position, m_Light[p_Total]->Strength, m_Light[p_Total]->Radius);
			}
			else if (m_Light[p_Total]->Type == Frosty::ECS::CLight::LightType::Directional)
			{
				Frosty::Renderer::AddLight(m_Light[p_Total]->Color, m_Transform[p_Total]->Rotation, m_Light[p_Total]->Strength);
			}*/

			p_Total++;
		}
	}

	void LightSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			
			//UpdateEntityComponent(entity);
			Frosty::Renderer::RemoveLight(entity);

			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Light[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void LightSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CLight* lightPtr = world->GetComponentAddress<Frosty::ECS::CLight>(entity);

			m_Transform[it->second] = transformPtr;
			m_Light[it->second] = lightPtr;

			Frosty::Renderer::UpdateLight(m_Light[it->second], m_Transform[it->second]);

		}
	}

	std::string LightSystem::GetInfo() const
	{
		std::stringstream retInfo;
		retInfo << "\t-----------" << NAME << " System Info-----------\n";
		retInfo << "\t\t---------Entity Map---------\n";
		retInfo << "\t\tEntity Id\tEntity Address\t\tEntity Refs\tArray Index\n";
		for (auto& em : p_EntityMap)
		{
			retInfo << "\t\t" << em.first->Id << "\t\t" << em.first << "\t\t" << em.first.use_count() << "\t" << em.second << "\n";
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t\t------Component Array(s)------\n";
		retInfo << "\n\t\tIndex\tComponent Address\tEntity Id\tEntity Address\t\tEntity Refs\n";
		for (size_t i = 1; i < p_Total; i++)
		{
			retInfo << "\t\t" << i << "\t" << m_Transform[i] << "\t" << m_Transform[i]->EntityPtr->Id << "\t\t" << m_Transform[i]->EntityPtr << "\t\t" << m_Transform[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Light[i] << "\t" << m_Light[i]->EntityPtr->Id << "\t\t" << m_Light[i]->EntityPtr << "\t\t" << m_Light[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	glm::vec3 LightSystem::ScreenToTerrainPoint()
	{
		glm::vec4 viewport = Frosty::Application::Get().GetWindow().GetViewport();
		auto& gameCameraEntity = Frosty::Application::Get().GetWorld()->GetSceneCamera();
		auto& gameCameraTransformComp = Frosty::Application::Get().GetWorld()->GetComponent<Frosty::ECS::CTransform>(gameCameraEntity);
		auto& gameCameraCamerComp = Frosty::Application::Get().GetWorld()->GetComponent<Frosty::ECS::CCamera>(gameCameraEntity);
		glm::vec2 mousePos = glm::vec2(Frosty::InputManager::GetMouseX(), Frosty::InputManager::GetMouseY());


		// Convert from viewport to NDC
		glm::vec2 NDC = glm::vec2(
			(2.0f * (mousePos.x - viewport.x)) / viewport.z - 1.0f,
			(2.0f * (mousePos.y - viewport.y)) / viewport.w - 1.0f
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

	void LightSystem::ShiftTowadsPoint(const glm::vec3& point, size_t index)
	{
		// Shifts the light slightly towards the lookAt position

		m_Transform[index]->Position = m_Light[index]->Origin->Position + m_Light[index]->Offset;
		glm::vec3 pointVector = glm::normalize(point - m_Transform[index]->Position);

		m_Transform[index]->Position += pointVector * glm::vec3(2.f, 0.f, 2.f);
	}
	
	void LightSystem::UpdateLightCamera(size_t index)
	{
		auto& world = Frosty::Application::Get().GetWorld();
		auto& win = Frosty::Application::Get().GetWindow();

		for (size_t i = 0; i < m_Light[index]->Cameras.size(); i++)
		{
			if (m_Light[index]->Type == Frosty::ECS::CLight::LightType::Directional)
				m_Light[index]->Cameras[i].Front = m_Light[index]->Direction;
			if (!world->HasComponent<Frosty::ECS::CPlayer>(m_Light[index]->Origin->EntityPtr))
				m_Transform[index]->Position = m_Light[index]->Origin->Position + m_Light[index]->Offset;
			
			m_Light[index]->Cameras[i].ViewMatrix = glm::lookAt(m_Transform[index]->Position, m_Transform[index]->Position + m_Light[index]->Cameras[i].Front, { 0.0f, 1.0f, 0.0f });
			m_Light[index]->Cameras[i].ProjectionMatrix = glm::perspective(glm::radians(m_Light[index]->Cameras[i].FieldOfView), float(win.GetViewport().z / win.GetViewport().w), m_Light[index]->Cameras[i].Near, m_Light[index]->Cameras[i].Far);
			m_Light[index]->Cameras[i].ViewProjectionMatrix = m_Light[index]->Cameras[i].ProjectionMatrix * m_Light[index]->Cameras[i].ViewMatrix;
		}
	}
}