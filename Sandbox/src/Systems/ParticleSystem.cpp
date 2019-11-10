#include "mcspch.hpp"
#include "ParticleSystem.hpp"

namespace MCS
{
	void ParticleSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CParticleSystem>(), true);
		m_maxLifetime = 2.0f;
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
			//m_ParticleSystem[p_Total]->computeShader = Frosty::AssetManager::GetShader("ParticleCompute");
			m_ParticleSystem[p_Total]->texture = Frosty::AssetManager::GetTexture2D("particle");

			p_Total++;
		}
	}

	void ParticleSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Transform[p_Total] = nullptr;
			m_ParticleSystem[p_Total] = nullptr;

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}

			p_EntityMap.erase(entity);
		}
	}

	void ParticleSystem::UpdateParticleSystem(uint32_t systemIndex)
	{
		m_ParticleSystem[systemIndex]->timer -= 1.0f * Frosty::Time::DeltaTime(); //Update internal timer
		if (m_ParticleSystem[systemIndex]->particleCount < m_ParticleSystem[systemIndex]->MAX_PARTICLE_COUNT && m_ParticleSystem[systemIndex]->timer <= 0)
		{
			m_ParticleSystem[systemIndex]->particleCount += 1;
			m_ParticleSystem[systemIndex]->timer = m_ParticleSystem[systemIndex]->emitRate;
		}

		//Iterate all particles
		for (size_t j = 0; j < m_ParticleSystem[systemIndex]->particleCount; j++)
		{
			if (m_ParticleSystem[systemIndex]->particles[j].lifetime <= 0.0f)
			{
				m_ParticleSystem[systemIndex]->particles[j].lifetime = m_maxLifetime;
				ResetParticle(systemIndex, j);
			}

			m_ParticleSystem[systemIndex]->particles[j].lifetime -= 1.0f * Frosty::Time::DeltaTime();
			UpdateParticle(systemIndex, j);
			UpdateGpuData(systemIndex, j);
		}

		UpdateBuffer(systemIndex);
	}

	void ParticleSystem::EditorUpdateParticleSystem(uint32_t systemIndex)
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
		if (m_ParticleSystem[systemIndex]->particles[0].startSize != m_ParticleSystem[systemIndex]->particleSize) { //This is pretty temporary. Right now the particle size is the start size
			for (uint32_t i = 0; i < Frosty::ECS::CParticleSystem::MAX_PARTICLE_COUNT; i++)
			{
				m_ParticleSystem[systemIndex]->particles[i].startSize = m_ParticleSystem[systemIndex]->particleSize;
				UpdateGpuData(systemIndex, i);
			}

			UpdateBuffer(systemIndex);
		}

		if (m_ParticleSystem[systemIndex]->preview)
		{
			m_ParticleSystem[systemIndex]->timer -= 1.0f * Frosty::Time::DeltaTime(); //Update internal timer
			if (m_ParticleSystem[systemIndex]->particleCount < m_ParticleSystem[systemIndex]->MAX_PARTICLE_COUNT && m_ParticleSystem[systemIndex]->timer <= 0)
			{
				m_ParticleSystem[systemIndex]->particleCount += 1;
				m_ParticleSystem[systemIndex]->timer = m_ParticleSystem[systemIndex]->emitRate;
			}

			//Iterate all particles
			for (size_t j = 0; j < m_ParticleSystem[systemIndex]->particleCount; j++)
			{
				if (m_ParticleSystem[systemIndex]->particles[j].lifetime <= 0.0f)
				{
					m_ParticleSystem[systemIndex]->particles[j].lifetime = m_maxLifetime;
					ResetParticle(systemIndex, j);
				}

				m_ParticleSystem[systemIndex]->particles[j].lifetime -= 1.0f * Frosty::Time::DeltaTime();
				UpdateParticle(systemIndex, j);
				UpdateGpuData(systemIndex, j);
			}

			UpdateBuffer(systemIndex);
		}
	}

	void ParticleSystem::UpdateParticle(uint32_t systemIndex, uint32_t index)
	{
		m_ParticleSystem[systemIndex]->particles[index].position -= (m_ParticleSystem[systemIndex]->particles[index].direction * m_ParticleSystem[systemIndex]->particles[index].speed) * Frosty::Time::DeltaTime();
		
		//Fade in and fade out
		if (m_ParticleSystem[systemIndex]->particles[index].color.a < 1.0 && m_ParticleSystem[systemIndex]->particles[index].lifetime > 1.0) {
			m_ParticleSystem[systemIndex]->particles[index].color.a += 2.0 * Frosty::Time::DeltaTime();
		}
		if (m_ParticleSystem[systemIndex]->particles[index].lifetime < 1.1) {
			m_ParticleSystem[systemIndex]->particles[index].color.a -= 1.0 * Frosty::Time::DeltaTime();

			//Update particle size
			if (m_ParticleSystem[systemIndex]->particles[index].size > m_ParticleSystem[systemIndex]->endParticleSize)
			{
				m_ParticleSystem[systemIndex]->particles[index].size -= 1.0f * Frosty::Time::DeltaTime();
			}
			else if (m_ParticleSystem[systemIndex]->particles[index].size < m_ParticleSystem[systemIndex]->endParticleSize)
			{
				m_ParticleSystem[systemIndex]->particles[index].size += 1.0f * Frosty::Time::DeltaTime();
			}
		}
	}

	void ParticleSystem::ResetParticle(uint32_t systemIndex, uint32_t index)
	{
		m_ParticleSystem[systemIndex]->particles[index].position = m_ParticleSystem[systemIndex]->particles[index].startPos;
		m_ParticleSystem[systemIndex]->particles[index].size = m_ParticleSystem[systemIndex]->particles[index].startSize;
	}

	void ParticleSystem::UpdateGpuData(uint32_t systemIndex, uint32_t index)
	{
		m_ParticleSystem[systemIndex]->gpuParticles[index].position = m_ParticleSystem[systemIndex]->particles[index].position;
		m_ParticleSystem[systemIndex]->gpuParticles[index].color = m_ParticleSystem[systemIndex]->particles[index].color;
		m_ParticleSystem[systemIndex]->gpuParticles[index].size = m_ParticleSystem[systemIndex]->particles[index].size;
	}

	void ParticleSystem::UpdateBuffer(uint32_t systemIndex)
	{
		m_ParticleSystem[systemIndex]->particleVertArray->GetVertexBuffer()[0]->SetData(m_ParticleSystem[systemIndex]->gpuParticles, (sizeof(Frosty::ECS::CParticleSystem::GPUParticle) * m_ParticleSystem[systemIndex]->particleCount), Frosty::BufferType::DYNAMIC);
	}
}