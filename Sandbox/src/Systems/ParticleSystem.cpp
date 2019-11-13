#include "mcspch.hpp"
#include "ParticleSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"

namespace MCS
{
	const std::string ParticleSystem::NAME = "Particle";

	void ParticleSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CParticleSystem>(), true);
		m_editMode = false; //Unused for now
	}

	void ParticleSystem::OnUpdate() //Only if application is running
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			UpdateParticleSystem(i);
		}
	}

	void ParticleSystem::Render()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (!Frosty::Application::Get().GameIsRunning())
			{
				EditorUpdateParticleSystem(i);
			}

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Transform[i]->Position);
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.x), { 1.0f, 0.0f, 0.0f });
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.z), { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, m_Transform[i]->Scale);

			m_ParticleSystem[i]->texture->Bind();

			Frosty::Renderer::SubmitParticles(m_ParticleSystem[i]->shader, m_ParticleSystem[i]->computeShader, m_ParticleSystem[i]->particleVertArray, transform, m_ParticleSystem[i]->particleCount, 2.0f);

			m_ParticleSystem[i]->texture->Unbind();
		}
	}

	void ParticleSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Transform[p_Total]->Position = glm::vec3(0.0f, 7.0f, 0.0f); //Debug
			m_ParticleSystem[p_Total] = &world->GetComponent<Frosty::ECS::CParticleSystem>(entity);

			m_ParticleSystem[p_Total]->particleVertArray.reset(Frosty::VertexArray::Create());

			std::shared_ptr<Frosty::VertexBuffer> vertBuffer;
			vertBuffer.reset(Frosty::VertexBuffer::Create(m_ParticleSystem[p_Total]->gpuParticles, sizeof(m_ParticleSystem[p_Total]->gpuParticles), Frosty::BufferType::DYNAMIC));

			Frosty::BufferLayout layout =
			{
				{ Frosty::ShaderDataType::Float4,	"pos"		},
				{ Frosty::ShaderDataType::Float4,	"color"		},
				{ Frosty::ShaderDataType::Float,	"size"		},
			};

			vertBuffer->SetLayout(layout);
			vertBuffer->SetNrOfVertices(m_ParticleSystem[p_Total]->particleCount);
			m_ParticleSystem[p_Total]->particleVertArray->AddVertexBuffer(vertBuffer); //Add to array

			m_ParticleSystem[p_Total]->shader = Frosty::AssetManager::GetShader("Particles");
			m_ParticleSystem[p_Total]->texture = Frosty::AssetManager::GetTexture2D("particle");

			p_Total++;
		}
	}

	void ParticleSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_ParticleSystem[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void ParticleSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CParticleSystem* particleSystemPtr = world->GetComponentAddress<Frosty::ECS::CParticleSystem>(entity);

			m_Transform[it->second] = transformPtr;
			m_ParticleSystem[it->second] = particleSystemPtr;
		}
	}

	std::string ParticleSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_ParticleSystem[i] << "\t" << m_ParticleSystem[i]->EntityPtr->Id << "\t\t" << m_ParticleSystem[i]->EntityPtr << "\t\t" << m_ParticleSystem[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void ParticleSystem::UpdateParticleSystem(size_t systemIndex)
	{
		m_ParticleSystem[systemIndex]->timer -= Frosty::Time::DeltaTime(); //Update internal timer
		if (m_ParticleSystem[systemIndex]->timer <= 0)
		{
			for (uint32_t i = 0; i < m_ParticleSystem[systemIndex]->emitCount; i++)
			{
				ResetParticle(systemIndex, FindUnusedParticle(systemIndex));
			}
			m_ParticleSystem[systemIndex]->timer = m_ParticleSystem[systemIndex]->emitRate;
		}

		//Iterate all particles
		m_ParticleSystem[systemIndex]->particleCount = 0;
		for (size_t j = 0; j < Frosty::ECS::CParticleSystem::MAX_PARTICLE_COUNT; j++)
		{
			if (m_ParticleSystem[systemIndex]->particles[j].lifetime > 0.0f)
			{
				m_ParticleSystem[systemIndex]->particles[j].lifetime -= Frosty::Time::DeltaTime();
				if (m_ParticleSystem[systemIndex]->particles[j].lifetime > 0.0f)
				{
					UpdateParticle(systemIndex, j);
					UpdateGpuData(systemIndex, j, m_ParticleSystem[systemIndex]->particleCount);
				}
				else
				{
					//Particle is dead
					m_ParticleSystem[systemIndex]->particles[j].camDistance = -1.0f;
				}
				m_ParticleSystem[systemIndex]->particleCount += 1;
			}
		}

		SortParticles(systemIndex);
		UpdateBuffer(systemIndex);
	}

	void ParticleSystem::EditorUpdateParticleSystem(size_t systemIndex)
	{
		if (glm::vec3(m_ParticleSystem[systemIndex]->particles[0].color) != m_ParticleSystem[systemIndex]->particleSystemColor) //Workaround
		{
			for (uint32_t i = 0; i < Frosty::ECS::CParticleSystem::MAX_PARTICLE_COUNT; i++)
			{
				m_ParticleSystem[systemIndex]->particles[i].color[0] = m_ParticleSystem[systemIndex]->particleSystemColor.r;
				m_ParticleSystem[systemIndex]->particles[i].color[1] = m_ParticleSystem[systemIndex]->particleSystemColor.g;
				m_ParticleSystem[systemIndex]->particles[i].color[2] = m_ParticleSystem[systemIndex]->particleSystemColor.b;
				UpdateGpuData(systemIndex, i);
			}

			UpdateBuffer(systemIndex);
		}
		if (m_ParticleSystem[systemIndex]->particles[0].startSize != m_ParticleSystem[systemIndex]->startParticleSize) {
			for (uint32_t i = 0; i < Frosty::ECS::CParticleSystem::MAX_PARTICLE_COUNT; i++)
			{
				m_ParticleSystem[systemIndex]->particles[i].startSize = m_ParticleSystem[systemIndex]->startParticleSize;
			}
		}

		if (m_ParticleSystem[systemIndex]->preview)
		{
			m_ParticleSystem[systemIndex]->timer -= Frosty::Time::DeltaTime(); //Update internal timer
			if (m_ParticleSystem[systemIndex]->timer <= 0)
			{
				for (uint32_t i = 0; i < m_ParticleSystem[systemIndex]->emitCount; i++)
				{
					ResetParticle(systemIndex, FindUnusedParticle(systemIndex));
				}
				m_ParticleSystem[systemIndex]->timer = m_ParticleSystem[systemIndex]->emitRate;
			}

			//Iterate all particles
			m_ParticleSystem[systemIndex]->particleCount = 0;
			for (size_t j = 0; j < Frosty::ECS::CParticleSystem::MAX_PARTICLE_COUNT; j++)
			{
				if (m_ParticleSystem[systemIndex]->particles[j].lifetime > 0.0f)
				{
					m_ParticleSystem[systemIndex]->particles[j].lifetime -= Frosty::Time::DeltaTime();
					if (m_ParticleSystem[systemIndex]->particles[j].lifetime > 0.0f)
					{
						UpdateParticle(systemIndex, j);
						UpdateGpuData(systemIndex, j, m_ParticleSystem[systemIndex]->particleCount);
					}
					else
					{
						//Particle is dead
						m_ParticleSystem[systemIndex]->particles[j].camDistance = -1.0f;
					}
					m_ParticleSystem[systemIndex]->particleCount += 1;
				}
			}

			SortParticles(systemIndex);
			UpdateBuffer(systemIndex);
		}
	}

	void ParticleSystem::UpdateParticle(size_t systemIndex, size_t index)
	{
		Frosty::ECS::CParticleSystem::Particle& p = m_ParticleSystem[systemIndex]->particles[index];

		p.camDistance = glm::length2(glm::vec3(p.position) - m_CameraTransform->Position);
		p.position -= (p.direction * p.speed) * Frosty::Time::DeltaTime();
		
		//Fade in
		//if (p.color.a < 1.0 && p.lifetime > 1.0) { //TODO: Fix this temporary code
		//	p.color.a += 2.0 * Frosty::Time::DeltaTime();
		//}

		//Fade out
		if (m_ParticleSystem[systemIndex]->fadeTreshold > 0.0f)
		{
			if (p.lifetime < m_ParticleSystem[systemIndex]->fadeTreshold)
			{
				float t = p.lifetime / m_ParticleSystem[systemIndex]->fadeTreshold;
				p.color.a = lerp(0.0f, 1.0f, t); //TODO: use endAlpha and startAlpha perhaps
			}
		}

		if (m_ParticleSystem[systemIndex]->startParticleSize != m_ParticleSystem[systemIndex]->endParticleSize)
		{
			//Update particle size
			float t = p.lifetime / m_ParticleSystem[systemIndex]->maxLifetime;

			if (p.size > m_ParticleSystem[systemIndex]->endParticleSize)
			{
				p.size = lerp(m_ParticleSystem[systemIndex]->endParticleSize, m_ParticleSystem[systemIndex]->startParticleSize, t);
			}
			else if (p.size < m_ParticleSystem[systemIndex]->endParticleSize)
			{
				p.size = lerp(m_ParticleSystem[systemIndex]->endParticleSize, m_ParticleSystem[systemIndex]->startParticleSize, t);
			}
		}
	}

	void ParticleSystem::ResetParticle(size_t systemIndex, size_t index)
	{
		Frosty::ECS::CParticleSystem::Particle& p = m_ParticleSystem[systemIndex]->particles[index];
		p.lifetime = m_ParticleSystem[systemIndex]->maxLifetime;
		p.position = m_ParticleSystem[systemIndex]->particles[index].startPos;
		p.size = p.startSize;
		p.color.a = 1.0f; //TODO: set to startColor/startAlpha
	}

	void ParticleSystem::UpdateGpuData(size_t systemIndex, size_t index, uint32_t particleCount)
	{
		Frosty::ECS::CParticleSystem::Particle& p = m_ParticleSystem[systemIndex]->particles[index];
		m_ParticleSystem[systemIndex]->gpuParticles[particleCount].position = p.position;
		m_ParticleSystem[systemIndex]->gpuParticles[particleCount].color = p.color;
		m_ParticleSystem[systemIndex]->gpuParticles[particleCount].size = p.size;
	}

	void ParticleSystem::UpdateGpuData(size_t systemIndex, size_t index)
	{
		Frosty::ECS::CParticleSystem::Particle& p = m_ParticleSystem[systemIndex]->particles[index];
		m_ParticleSystem[systemIndex]->gpuParticles[index].position = p.position;
		m_ParticleSystem[systemIndex]->gpuParticles[index].color = p.color;
		m_ParticleSystem[systemIndex]->gpuParticles[index].size = p.size;
	}

	void ParticleSystem::UpdateBuffer(size_t systemIndex)
	{
		m_ParticleSystem[systemIndex]->particleVertArray->GetVertexBuffer()[0]->SetData(m_ParticleSystem[systemIndex]->gpuParticles, (sizeof(Frosty::ECS::CParticleSystem::GPUParticle) * m_ParticleSystem[systemIndex]->particleCount), Frosty::BufferType::DYNAMIC);
	}

	uint32_t ParticleSystem::FindUnusedParticle(size_t systemIndex)
	{
		//Linear search, but since we start at the last used index it will usually return immediately

		for (unsigned int i = m_ParticleSystem[systemIndex]->lastUsedParticle; i < Frosty::ECS::CParticleSystem::MAX_PARTICLE_COUNT; i++) {
			if (m_ParticleSystem[systemIndex]->particles[i].lifetime < 0.0f) {
				m_ParticleSystem[systemIndex]->lastUsedParticle = i;
				return i;
			}
		}

		for (unsigned int i = 0; i < m_ParticleSystem[systemIndex]->lastUsedParticle; i++) {
			if (m_ParticleSystem[systemIndex]->particles[i].lifetime < 0) {
				m_ParticleSystem[systemIndex]->lastUsedParticle = i;
				return i;
			}
		}

		return 0; // All particles taken, override the first one
	}

	void ParticleSystem::SortParticles(size_t systemIndex)
	{
		std::sort(&m_ParticleSystem[systemIndex]->particles[0], &m_ParticleSystem[systemIndex]->particles[Frosty::ECS::CParticleSystem::MAX_PARTICLE_COUNT]);
	}

	float ParticleSystem::lerp(float a, float b, float f)
	{
		return (a * (1.0 - f)) + (b * f);
	}
}