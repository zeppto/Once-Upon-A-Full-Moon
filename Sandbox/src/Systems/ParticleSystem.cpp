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

		m_ParticleSystem[i]->shader->Bind();
		m_ParticleSystem[i]->texture->Bind();
		Frosty::Renderer::SubmitParticles(m_ParticleSystem[i]->shader, m_ParticleSystem[i]->particleVertArray, transform, m_ParticleSystem[i]->particleCount);
		m_ParticleSystem[i]->shader->UnBind();
		m_ParticleSystem[i]->texture->Unbind();
	}

//m_particleShader->Bind();
//glEnable(GL_BLEND);
//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//m_particleSystem->LoadTexture();
//glBindTexture(GL_TEXTURE_2D, m_particleSystem->GetTextureID());
//Renderer::SubmitParticles(m_particleShader, m_particleSystem->GetVertexArray(), m_particleSystem->GetModelMatrix(), m_particleSystem->GetParticleCount());
//m_particleShader->UnBind();
//glDisable(GL_BLEND);
}

void ParticleSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
		m_ParticleSystem[p_Total] = &world->GetComponent<Frosty::ECS::CParticleSystem>(entity);

		m_ParticleSystem[p_Total]->particleVertArray.reset(Frosty::VertexArray::Create());

		std::shared_ptr<Frosty::VertexBuffer> vertBuffer;
		vertBuffer.reset(Frosty::VertexBuffer::Create(m_ParticleSystem[p_Total]->particles, sizeof(m_ParticleSystem[p_Total]->particles)));

		Frosty::BufferLayout layout =
		{
			{ Frosty::ShaderDataType::Float4,	"pos" },
			{ Frosty::ShaderDataType::Float4,	"color" },
			{ Frosty::ShaderDataType::Float4,	"startPos" },
			{ Frosty::ShaderDataType::Float4,	"dir" },
			{ Frosty::ShaderDataType::Float,	"lifetime" },
			{ Frosty::ShaderDataType::Float,	"speed" }
		};

		vertBuffer->SetLayout(layout);
		vertBuffer->SetNrOfVertices(m_ParticleSystem[p_Total]->particleCount);
		m_ParticleSystem[p_Total]->particleVertArray->AddVertexBuffer(vertBuffer); //Add to array

		m_ParticleSystem[p_Total]->shader = Frosty::AssetManager::GetShader("Particles");
		std::string test = m_ParticleSystem[p_Total]->shader->GetName();
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
