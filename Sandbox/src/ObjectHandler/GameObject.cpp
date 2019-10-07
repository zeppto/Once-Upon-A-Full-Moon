#include "mcspch.hpp"
#include "GameObject.hpp"

GameObject::GameObject(glm::vec3 color, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, int maxHp, std::string modelFilename)
{
	m_renderData.model_ptr = Frosty::Assetmanager::GetAssetmanager()->GetModeltemplateMetaData(modelFilename)->GetData();
	m_renderData.material_Ptr = Frosty::Assetmanager::GetAssetmanager()->GetMaterialMetaData("Mat_0:" + modelFilename)->GetData();
	m_BoundingBox = Frosty::Assetmanager::GetAssetmanager()->GetModeltemplateMetaData(modelFilename)->GetData()->GetMeshInfoConst(0).BoundingBox;

	//temp fix
	m_BoundingBox.halfSize[0] *= 2;
	m_BoundingBox.halfSize[1] *= 2;
	m_BoundingBox.halfSize[2] *= 2;

	m_Pos = position;
	m_Rotation = rotation;
	m_Scale = scale;
	UpdateWorldMatrix();

	/*Frosty::Application::Get().GetRenderEngine()->AddToRenderList(&m_renderData);

	m_renderData.m_Render_Colour = glm::vec3(0.0f, 0.0f, 0.0f);*/
	m_MaxHp = maxHp;
	m_Hp = maxHp;

}

GameObject::~GameObject()
{
}

void GameObject::SetPos(glm::vec3 newPos)
{
	m_Pos = newPos;
}

void GameObject::SetScale(glm::vec3 newScale)
{
	m_Scale = newScale;
}

void GameObject::SetRenderColour(glm::vec3 newColour)
{
	m_renderData.m_Render_Colour = newColour;
}

void GameObject::SetRotation(glm::vec3 newRotation)
{
	m_Rotation = newRotation;
}

void GameObject::SetHitBoxLength(glm::vec3 newHitBoxLength)
{
	for (int i = 0; i < 3; i++)
	{
		m_BoundingBox.halfSize[i] = newHitBoxLength[i];
	}
}

void GameObject::SetHitBoxCenter(glm::vec3 newHitBoxCenter)
{
	for (int i = 0; i < 3; i++)
	{
		m_BoundingBox.pos[i] = newHitBoxCenter[i];
	}
}

void GameObject::SetWorldMatrix(glm::mat4 newWorld)
{
	m_renderData.worldPosition = newWorld;
}

void GameObject::SetShouldRender(bool render)
{
	m_renderData.m_Render = render;
}

void GameObject::SetMaxHp(int newMaxHp, bool setHpToMax)
{
	m_MaxHp = newMaxHp;
	if (setHpToMax)
		m_Hp = newMaxHp;
}

void GameObject::SetHp(int newHp)
{
	m_Hp = newHp;
}

glm::vec3 GameObject::GetPos() const
{
	return m_Pos;
}

glm::vec3 GameObject::GetScale() const
{
	return m_Scale;
}

glm::vec3 GameObject::GetRotation() const
{
	return m_Rotation;
}

glm::vec3 GameObject::GetHitBoxLength() const
{
	return glm::vec3(m_BoundingBox.halfSize[0], m_BoundingBox.halfSize[1], m_BoundingBox.halfSize[2]) * m_Scale;
}

glm::vec3 GameObject::GetHitBoxCenter() const
{
	return glm::vec3(m_BoundingBox.pos[0], m_BoundingBox.pos[1], m_BoundingBox.pos[2]) + m_Pos;
}

int GameObject::GetMaxHp() const
{
	return m_MaxHp;
}

int GameObject::GetHp() const
{
	return m_Hp;
}



void GameObject::UpdateWorldMatrix()
{
	m_renderData.worldPosition = glm::mat4(1.0f);
	m_renderData.worldPosition = glm::translate(m_renderData.worldPosition, m_Pos);
	m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	m_renderData.worldPosition = glm::scale(m_renderData.worldPosition, m_Scale);
}

void GameObject::UpdateHp()
{
	m_renderData.m_Render_Colour.r = 1 - ((float)m_Hp / (float)m_MaxHp);

	if (m_Hp <= 0)
		m_renderData.m_Render = false;
	else
		m_renderData.m_Render = true;
}
