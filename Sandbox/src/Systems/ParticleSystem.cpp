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

			unsigned int test = sizeof(m_ParticleSystem[i]->particles); //Debug
			unsigned int test2 = sizeof(Frosty::ECS::CParticleSystem::Particle); //Debug

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
			vertBuffer.reset(Frosty::VertexBuffer::Create(m_ParticleSystem[p_Total]->particles, sizeof(m_ParticleSystem[p_Total]->particles), Frosty::BufferType::DYNAMIC));

			Frosty::BufferLayout layout =
			{
				{ Frosty::ShaderDataType::Float4,	"pos"		},
				{ Frosty::ShaderDataType::Float4,	"color"		},
				{ Frosty::ShaderDataType::Float4,	"dir"		},
				{ Frosty::ShaderDataType::Float4,	"startPos"	},
				{ Frosty::ShaderDataType::Float,	"lifetime"	},
				{ Frosty::ShaderDataType::Float,	"speed"		},
				{ Frosty::ShaderDataType::Float,	"startSize" },
				{ Frosty::ShaderDataType::Float,	"endSize"	},
				{ Frosty::ShaderDataType::Float,	"size"		}
				//{ Frosty::ShaderDataType::Int2, "padding"}
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

	void ParticleSystem::UpdateParticleSystem(uint32_t systemIndex)
	{
		m_ParticleSystem[systemIndex]->timer -= 1.0f * Frosty::Time::DeltaTime(); //Update internal timer
		if (m_ParticleSystem[systemIndex]->particleCount < m_ParticleSystem[systemIndex]->MAX_PARTICLE_COUNT && m_ParticleSystem[systemIndex]->timer <= 0)
		{
			m_ParticleSystem[systemIndex]->particleCount += 1;
			m_ParticleSystem[systemIndex]->timer = m_ParticleSystem[systemIndex]->emitRate;
		}

		for (size_t j = 0; j < m_ParticleSystem[systemIndex]->particleCount; j++)
		{
			if (m_ParticleSystem[systemIndex]->particles[j].lifetime <= 0.0f)
			{
				m_ParticleSystem[systemIndex]->particles[j].lifetime = m_maxLifetime;
				ResetParticle(systemIndex, j);
			}

			m_ParticleSystem[systemIndex]->particles[j].lifetime -= 1.0f * Frosty::Time::DeltaTime();
			UpdateParticle(systemIndex, j);
			UpdateBuffer(systemIndex);
		}
	}

	void ParticleSystem::EditorUpdateParticleSystem(uint32_t systemIndex)
	{
		if (glm::vec3(m_ParticleSystem[systemIndex]->particles[0].color) != m_ParticleSystem[systemIndex]->particleSystemColor) //Workaround
		{
			m_ParticleSystem[systemIndex]->SetParticlesColor(m_ParticleSystem[systemIndex]->particleSystemColor.r, m_ParticleSystem[systemIndex]->particleSystemColor.g, m_ParticleSystem[systemIndex]->particleSystemColor.b);
			UpdateBuffer(systemIndex);
		}
		if (m_ParticleSystem[systemIndex]->particles[0].startSize != m_ParticleSystem[systemIndex]->particleSize) { //This is pretty temporary. Right now the particle size is the start size
			m_ParticleSystem[systemIndex]->SetParticlesSize(m_ParticleSystem[systemIndex]->particleSize);
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

			for (size_t j = 0; j < m_ParticleSystem[systemIndex]->particleCount; j++)
			{
				if (m_ParticleSystem[systemIndex]->particles[j].lifetime <= 0.0f)
				{
					m_ParticleSystem[systemIndex]->particles[j].lifetime = m_maxLifetime;
					ResetParticle(systemIndex, j);
				}

				m_ParticleSystem[systemIndex]->particles[j].lifetime -= 1.0f * Frosty::Time::DeltaTime();
				UpdateParticle(systemIndex, j);
				UpdateBuffer(systemIndex);
			}
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

	void ParticleSystem::UpdateBuffer(uint32_t systemIndex)
	{
		m_ParticleSystem[systemIndex]->particleVertArray->GetVertexBuffer()[0]->SetData(m_ParticleSystem[systemIndex]->particles, sizeof(m_ParticleSystem[p_Total]->particles), Frosty::BufferType::DYNAMIC);
	}
}