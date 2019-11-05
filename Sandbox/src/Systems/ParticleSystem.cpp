#include "mcspch.hpp"
#include "ParticleSystem.hpp"

void ParticleSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CParticleSystem>(), true);
}

void ParticleSystem::OnUpdate()
{

}

void ParticleSystem::Render()
{
	for (size_t i = 1; i < p_Total; i++)
	{
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
			{ Frosty::ShaderDataType::Float4,	"pos" },
			{ Frosty::ShaderDataType::Float4,	"color" },
			{ Frosty::ShaderDataType::Float4,	"dir" },
			{ Frosty::ShaderDataType::Float4,	"startPos" },
			{ Frosty::ShaderDataType::Float,	"lifetime" },
			{ Frosty::ShaderDataType::Float,	"speed" },
			{ Frosty::ShaderDataType::Int2, "padding"}
		};

		vertBuffer->SetLayout(layout);
		vertBuffer->SetNrOfVertices(m_ParticleSystem[p_Total]->particleCount);
		m_ParticleSystem[p_Total]->particleVertArray->AddVertexBuffer(vertBuffer); //Add to array

		m_ParticleSystem[p_Total]->shader = Frosty::AssetManager::GetShader("Particles");
		m_ParticleSystem[p_Total]->computeShader = Frosty::AssetManager::GetShader("ParticleCompute");
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
