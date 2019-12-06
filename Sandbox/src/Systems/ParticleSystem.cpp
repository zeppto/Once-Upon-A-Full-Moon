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
		m_EditMode = false; //Unused for now
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

			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::rotate(transform, glm::radians(m_ParticleSystem[i]->SystemRotation.x), { 1.0f, 0.0f, 0.0f });
			transform = glm::rotate(transform, glm::radians(m_ParticleSystem[i]->SystemRotation.y), { 0.0f, 1.0f, 0.0f });
			transform = glm::rotate(transform, glm::radians(m_ParticleSystem[i]->SystemRotation.z), { 0.0f, 0.0f, 1.0f });

			glm::mat4 parentTransform;
			if (!m_ParticleSystem[i]->HasGravity)
			{
				parentTransform = glm::translate(glm::mat4(1.0f), m_Transform[i]->Position);
				parentTransform = glm::rotate(parentTransform, glm::radians(m_Transform[i]->Rotation.x), { 1.0f, 0.0f, 0.0f });
				parentTransform = glm::rotate(parentTransform, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
				parentTransform = glm::rotate(parentTransform, glm::radians(m_Transform[i]->Rotation.z), { 0.0f, 0.0f, 1.0f });
				parentTransform = glm::scale(parentTransform, m_Transform[i]->Scale);
			}
			else
			{
				parentTransform = glm::mat4(1.0f);
			}

			transform = parentTransform * transform;

			m_ParticleSystem[i]->Texture->Bind();

			Frosty::Renderer::SubmitParticles(m_ParticleSystem[i]->UseShader, m_ParticleSystem[i]->ParticleVertArray, transform, m_ParticleSystem[i]->ParticleCount, 2.0f, (unsigned int)m_ParticleSystem[i]->RenderMode);

			m_ParticleSystem[i]->Texture->Unbind();
		}
	}

	void ParticleSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_ParticleSystem[p_Total] = &world->GetComponent<Frosty::ECS::CParticleSystem>(entity);

			m_ParticleSystem[p_Total]->ParticleVertArray.reset(Frosty::VertexArray::Create());

			std::shared_ptr<Frosty::VertexBuffer> vertBuffer;
			vertBuffer.reset(Frosty::VertexBuffer::Create(m_ParticleSystem[p_Total]->GpuParticles, sizeof(m_ParticleSystem[p_Total]->GpuParticles), Frosty::BufferType::DYNAMIC));

			Frosty::BufferLayout layout =
			{
				{ Frosty::ShaderDataType::Float4,	"pos"		},
				{ Frosty::ShaderDataType::Float4,	"color"		},
				{ Frosty::ShaderDataType::Float,	"size"		},
			};

			vertBuffer->SetLayout(layout);
			vertBuffer->SetNrOfVertices(m_ParticleSystem[p_Total]->ParticleCount);
			m_ParticleSystem[p_Total]->ParticleVertArray->AddVertexBuffer(vertBuffer); //Add to array

			m_ParticleSystem[p_Total]->UseShader = Frosty::AssetManager::GetShader(m_ParticleSystem[p_Total]->ShaderName);
			m_ParticleSystem[p_Total]->Texture = Frosty::AssetManager::GetTexture2D(m_ParticleSystem[p_Total]->TextureName);

			if (m_ParticleSystem[p_Total]->ShaderName == "ParticlesHorizontal")
			{
				m_ParticleSystem[p_Total]->AlwaysFaceCamera = false;
			}

			m_ParticleSystem[p_Total]->Particles.resize(m_ParticleSystem[p_Total]->MaxParticles);



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

	void ParticleSystem::ChangeParticlesStartColor(Frosty::ECS::CParticleSystem& particleSystem, glm::vec3 color)
	{
		particleSystem.SystemStartColor = color;
		for (unsigned int i = 0; i < particleSystem.MaxParticles; i++)
		{
			particleSystem.Particles[i].StartColor.r = color.r;
			particleSystem.Particles[i].StartColor.g = color.g;
			particleSystem.Particles[i].StartColor.b = color.b;
		}
	}

	void ParticleSystem::UpdateParticleSystem(size_t systemIndex)
	{
		m_ParticleSystem[systemIndex]->Timer -= Frosty::Time::DeltaTime(); //Update internal timer
		if (m_ParticleSystem[systemIndex]->Timer <= 0)
		{
			for (uint32_t i = 0; i < m_ParticleSystem[systemIndex]->EmitCount; i++)
			{
				uint32_t unusedParticle = FindUnusedParticle(systemIndex);
				if (!m_ParticleSystem[systemIndex]->Loop)
				{
					if (unusedParticle == 0)
					{
						m_ParticleSystem[systemIndex]->TimesPlayed += 1;
					}
					if (m_ParticleSystem[systemIndex]->TimesPlayed < 1)
					{
						ResetParticle(systemIndex, unusedParticle);
					}
				}
				else
				{
					ResetParticle(systemIndex, unusedParticle);
				}
			}
			m_ParticleSystem[systemIndex]->Timer = m_ParticleSystem[systemIndex]->EmitRate;
		}

		//Iterate all particles
		m_ParticleSystem[systemIndex]->ParticleCount = 0;
		for (size_t j = 0; j < m_ParticleSystem[systemIndex]->MaxParticles; j++)
		{
			if (m_ParticleSystem[systemIndex]->Particles[j].Lifetime > 0.0f)
			{
				m_ParticleSystem[systemIndex]->Particles[j].Lifetime -= Frosty::Time::DeltaTime();
				if (m_ParticleSystem[systemIndex]->Particles[j].Lifetime > 0.0f)
				{
					UpdateParticle(systemIndex, j);
					UpdateGpuData(systemIndex, j, m_ParticleSystem[systemIndex]->ParticleCount);
				}
				else
				{
					//Particle is dead
					m_ParticleSystem[systemIndex]->Particles[j].CamDistance = -1.0f;
				}
				m_ParticleSystem[systemIndex]->ParticleCount += 1;
			}
		}

		SortParticles(systemIndex);
		UpdateBuffer(systemIndex);
		if (m_ParticleSystem[systemIndex]->RotateOverLifetime == true)
		{
			m_ParticleSystem[systemIndex]->SystemRotation.y += 10 * Frosty::Time::DeltaTime();
		}
	}

	void ParticleSystem::EditorUpdateParticleSystem(size_t systemIndex)
	{
		if (m_ParticleSystem[systemIndex]->Particles.size() != m_ParticleSystem[systemIndex]->MaxParticles)
		{
			m_ParticleSystem[systemIndex]->Particles.resize(m_ParticleSystem[systemIndex]->MaxParticles);
			for (uint32_t i = 0; i < m_ParticleSystem[systemIndex]->MaxParticles; i++)
			{
				UpdateGpuData(systemIndex, i);
			}
			m_ParticleSystem[systemIndex]->LastUsedParticle = m_ParticleSystem[systemIndex]->MaxParticles; //To not read outside of buffer when downsizing
		}
		if (glm::vec3(m_ParticleSystem[systemIndex]->Particles[0].StartColor) != m_ParticleSystem[systemIndex]->SystemStartColor) //Still very temporary solution to determine if data needs updating
		{
			for (uint32_t i = 0; i < m_ParticleSystem[systemIndex]->MaxParticles; i++)
			{
				m_ParticleSystem[systemIndex]->Particles[i].StartColor.r = m_ParticleSystem[systemIndex]->SystemStartColor.r;
				m_ParticleSystem[systemIndex]->Particles[i].StartColor.g = m_ParticleSystem[systemIndex]->SystemStartColor.g;
				m_ParticleSystem[systemIndex]->Particles[i].StartColor.b = m_ParticleSystem[systemIndex]->SystemStartColor.b;
				UpdateGpuData(systemIndex, i);
			}

			UpdateBuffer(systemIndex);
		}
		if (m_ParticleSystem[systemIndex]->Particles[0].StartSize != m_ParticleSystem[systemIndex]->StartParticleSize) {
			for (uint32_t i = 0; i < m_ParticleSystem[systemIndex]->MaxParticles; i++)
			{
				m_ParticleSystem[systemIndex]->Particles[i].StartSize = m_ParticleSystem[systemIndex]->StartParticleSize;
			}
		}
		//if (m_ParticleSystem[systemIndex]->Particles[0].Speed != m_ParticleSystem[systemIndex]->Speed) //Temporary if we're gonna have physics, drag or random speeds
		//{
		//	for (uint32_t i = 0; i < m_ParticleSystem[systemIndex]->MaxParticles; i++)
		//	{
		//		m_ParticleSystem[systemIndex]->Particles[i].Speed = m_ParticleSystem[systemIndex]->Speed;
		//	}
		//}

		if (m_ParticleSystem[systemIndex]->Preview)
		{
			m_ParticleSystem[systemIndex]->Timer -= Frosty::Time::DeltaTime(); //Update internal timer
			if (m_ParticleSystem[systemIndex]->Timer <= 0)
			{
				for (uint32_t i = 0; i < m_ParticleSystem[systemIndex]->EmitCount; i++)
				{
					uint32_t unusedParticle = FindUnusedParticle(systemIndex);
					if (!m_ParticleSystem[systemIndex]->Loop)
					{
						if (unusedParticle == 0)
						{
							m_ParticleSystem[systemIndex]->TimesPlayed += 1;
						}
						if (m_ParticleSystem[systemIndex]->TimesPlayed < 1)
						{
							ResetParticle(systemIndex, unusedParticle);
						}
					}
					else
					{
						ResetParticle(systemIndex, unusedParticle);
					}
				}
				m_ParticleSystem[systemIndex]->Timer = m_ParticleSystem[systemIndex]->EmitRate;
			}

			//Iterate all particles
			m_ParticleSystem[systemIndex]->ParticleCount = 0;
			for (size_t j = 0; j < m_ParticleSystem[systemIndex]->MaxParticles; j++)
			{
				if (m_ParticleSystem[systemIndex]->Particles[j].Lifetime > 0.0f)
				{
					m_ParticleSystem[systemIndex]->Particles[j].Lifetime -= Frosty::Time::DeltaTime();
					if (m_ParticleSystem[systemIndex]->Particles[j].Lifetime > 0.0f)
					{
						UpdateParticle(systemIndex, j);
						UpdateGpuData(systemIndex, j, m_ParticleSystem[systemIndex]->ParticleCount);
					}
					else
					{
						//Particle is dead
						m_ParticleSystem[systemIndex]->Particles[j].CamDistance = -1.0f;
					}
					m_ParticleSystem[systemIndex]->ParticleCount += 1;
				}
			}

			SortParticles(systemIndex);
			UpdateBuffer(systemIndex);
		}
		if (m_ParticleSystem[systemIndex]->RotateOverLifetime == true)
		{
			m_ParticleSystem[systemIndex]->SystemRotation.y += 10 * Frosty::Time::DeltaTime();
		}
	}

	void ParticleSystem::UpdateParticle(size_t systemIndex, size_t index)
	{
		Frosty::ECS::CParticleSystem::Particle& p = m_ParticleSystem[systemIndex]->Particles[index];

		p.CamDistance = glm::length2(glm::vec3(p.Position) - m_CameraTransform->Position);
		if (m_ParticleSystem[systemIndex]->HasGravity)
		{
			p.Direction += m_ParticleSystem[systemIndex]->ParticleWeight * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f) * Frosty::Time::DeltaTime();
			glm::normalize(p.Direction);
			p.Position += (p.Direction * p.Speed) * Frosty::Time::DeltaTime();
		}
		else
		{
			p.Position += (p.Direction * p.Speed) * Frosty::Time::DeltaTime();
		}

		//Fade in
		if (m_ParticleSystem[systemIndex]->FadeInTreshold < p.MaxLifetime) {
			if (p.Lifetime > m_ParticleSystem[systemIndex]->FadeInTreshold)
			{
				float t = (p.Lifetime - m_ParticleSystem[systemIndex]->FadeInTreshold) / (p.MaxLifetime - m_ParticleSystem[systemIndex]->FadeInTreshold);
				p.Color.a = Lerp(1.0f, 0.0f, t);
			}
		}
		//Fade out
		if (m_ParticleSystem[systemIndex]->FadeTreshold > 0.0f)
		{
			if (p.Lifetime < m_ParticleSystem[systemIndex]->FadeTreshold)
			{
				float t = p.Lifetime / m_ParticleSystem[systemIndex]->FadeTreshold;
				p.Color.a = Lerp(0.0f, 1.0f, t);
			}
		}
		if (m_ParticleSystem[systemIndex]->StartParticleSize != m_ParticleSystem[systemIndex]->EndParticleSize)
		{
			//Update particle size
			float t = p.Lifetime / m_ParticleSystem[systemIndex]->MaxLifetime;
			p.Size = Lerp(m_ParticleSystem[systemIndex]->EndParticleSize, m_ParticleSystem[systemIndex]->StartParticleSize, t);
		}
		//Interpolate color
		if (glm::vec3(p.StartColor) != m_ParticleSystem[systemIndex]->SystemEndColor) //TODO: use an individual endColor for particles and interpolate that
		{
			float t = p.Lifetime / m_ParticleSystem[systemIndex]->MaxLifetime;
			glm::vec3 interpolatedColor = m_ParticleSystem[systemIndex]->SystemEndColor * (1 - t) + glm::vec3(p.StartColor) * t; //Interpolation
			p.Color.r = interpolatedColor.r;
			p.Color.g = interpolatedColor.g;
			p.Color.b = interpolatedColor.b;
		}
	}

	void ParticleSystem::ResetParticle(size_t systemIndex, size_t index)
	{
		Frosty::ECS::CParticleSystem::Particle& p = m_ParticleSystem[systemIndex]->Particles[index];
		if(m_ParticleSystem[systemIndex]->RandomLifetimes == false)
		{
			p.MaxLifetime = m_ParticleSystem[systemIndex]->MaxLifetime;
		}
		else
		{
			float randLifetime = RandomFloat(m_ParticleSystem[systemIndex]->MaxLifetime, m_ParticleSystem[systemIndex]->MinLifetime);
			p.MaxLifetime = randLifetime;
		}
		if (m_ParticleSystem[systemIndex]->HasGravity)
		{
			//m_ParticleSystem[systemIndex]->ParticleSystemStartPos = m_Transform[systemIndex]->Position;
			p.StartPos = glm::vec4(m_Transform[systemIndex]->Position, 1.0f);
		}
		else if (m_ParticleSystem[systemIndex]->RandomStartPos == false)
		{
			p.StartPos = glm::vec4(m_ParticleSystem[systemIndex]->ParticleSystemStartPos, 1.0f);
		}
		else
		{
			float randomNumX = ((rand() % 100) - 50) / 10.0f;
			float randomNumY = ((rand() % 100) - 50) / 10.0f;
			float randomNumZ = ((rand() % 100) - 50) / 10.0f;
			p.StartPos.x = randomNumX;
			p.StartPos.y = randomNumY;
			p.StartPos.z = randomNumZ;
		}
		p.Lifetime = p.MaxLifetime;
		p.Position = p.StartPos;
		p.Size = p.StartSize;
		if (m_ParticleSystem[systemIndex]->RandomDirection == false)
		{
			p.Direction = glm::vec4(m_ParticleSystem[systemIndex]->ParticleSystemDirection, 1.0f);
			//TODO: Start Direction?
		}
		else
		{
			//Not the best way to randomize direction but good enough for now

			glm::vec3 randDir;
			randDir.x = (rand()% 2000 - 1000.0f) / 1000.0f;
			randDir.y = (rand() % 2000 - 1000.0f) / 1000.0f;
			randDir.z = (rand() % 2000 - 1000.0f) / 1000.0f;

			randDir = m_ParticleSystem[systemIndex]->randMainDir + randDir * m_ParticleSystem[systemIndex]->randSpread;

			glm::normalize(randDir);

			p.Direction.x = randDir.x;
			p.Direction.y = randDir.y;
			p.Direction.z = randDir.z;
		}
		p.Color = p.StartColor;
		p.Speed = m_ParticleSystem[systemIndex]->Speed; //TODO: StartSpeed
	}

	void ParticleSystem::UpdateGpuData(size_t systemIndex, size_t index, uint32_t particleCount)
	{
		Frosty::ECS::CParticleSystem::Particle& p = m_ParticleSystem[systemIndex]->Particles[index];
		m_ParticleSystem[systemIndex]->GpuParticles[particleCount].Position = p.Position;
		m_ParticleSystem[systemIndex]->GpuParticles[particleCount].Color = p.Color;
		m_ParticleSystem[systemIndex]->GpuParticles[particleCount].Size = p.Size;
	}

	void ParticleSystem::UpdateGpuData(size_t systemIndex, size_t index)
	{
		Frosty::ECS::CParticleSystem::Particle& p = m_ParticleSystem[systemIndex]->Particles[index];
		m_ParticleSystem[systemIndex]->GpuParticles[index].Position = p.Position;
		m_ParticleSystem[systemIndex]->GpuParticles[index].Color = p.Color;
		m_ParticleSystem[systemIndex]->GpuParticles[index].Size = p.Size;
	}

	void ParticleSystem::UpdateBuffer(size_t systemIndex)
	{
		m_ParticleSystem[systemIndex]->ParticleVertArray->GetVertexBuffer()[0]->SetData(m_ParticleSystem[systemIndex]->GpuParticles, (sizeof(Frosty::ECS::CParticleSystem::GPUParticle) * m_ParticleSystem[systemIndex]->ParticleCount), Frosty::BufferType::DYNAMIC);
	}

	uint32_t ParticleSystem::FindUnusedParticle(size_t systemIndex)
	{
		//Linear search, but since we start at the last used index it will usually return immediately

		for (unsigned int i = m_ParticleSystem[systemIndex]->LastUsedParticle; i < m_ParticleSystem[systemIndex]->MaxParticles; i++) {
			if (m_ParticleSystem[systemIndex]->Particles[i].Lifetime <= 0.0f) {
				m_ParticleSystem[systemIndex]->LastUsedParticle = i;
				return i;
			}
		}

		for (unsigned int i = 0; i < m_ParticleSystem[systemIndex]->LastUsedParticle; i++) {
			if (m_ParticleSystem[systemIndex]->Particles[i].Lifetime <= 0.0f) {
				m_ParticleSystem[systemIndex]->LastUsedParticle = i;
				return i;
			}
		}

		return 0; // All particles taken, override the first one
	}

	void ParticleSystem::SortParticles(size_t systemIndex)
	{
		std::sort(&m_ParticleSystem[systemIndex]->Particles[0], &m_ParticleSystem[systemIndex]->Particles[m_ParticleSystem[systemIndex]->MaxParticles - 1]);
	}

	float ParticleSystem::Lerp(float a, float b, float f) //Lerp can probably be done on the gpu which would be a good optimization
	{
		return (a * (1.0f - f)) + (b * f);
	}

	float ParticleSystem::RandomFloat(float min, float max)
	{
		float random = ((float) rand()) / (float)RAND_MAX;
		float range = max - min;
		float r = random * range;
		return min + r;
	}
}