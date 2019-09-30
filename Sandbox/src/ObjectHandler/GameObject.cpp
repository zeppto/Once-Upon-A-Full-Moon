#include "GameObject.hpp"

GameObject::GameObject(glm::vec3 color, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	m_renderData.model_ptr = Frosty::Assetmanager::GetAssetmanager()->GetModeltemplateMetaData("tempGround")->GetData();
	m_renderData.material_Ptr = Frosty::Assetmanager::GetAssetmanager()->GetMaterialMetaData("Mat_0:tempGround")->GetData();


	m_renderData.worldPosition = glm::mat4(1.0f);

	Frosty::Application::Get().GetRenderEngine()->AddToRenderList(&m_renderData);
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

void GameObject::SetRotation(glm::vec3 newRotation)
{
	m_Rotation = newRotation;
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

void GameObject::UpdateWorldMatrix()
{
	m_renderData.worldPosition = glm::mat4(1.0f);
	m_renderData.worldPosition = glm::translate(m_renderData.worldPosition, m_Pos);
	m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	m_renderData.worldPosition = glm::scale(m_renderData.worldPosition, m_Scale);
}
