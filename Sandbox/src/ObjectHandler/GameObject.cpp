#include "GameObject.hpp"

GameObject::GameObject(glm::vec3 color, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::string modelFilename)
{
	m_renderData.model_ptr = Frosty::Assetmanager::GetAssetmanager()->GetModeltemplateMetaData(modelFilename)->GetData();
	m_renderData.material_Ptr = Frosty::Assetmanager::GetAssetmanager()->GetMaterialMetaData("Mat_0:" + modelFilename)->GetData();
	m_BoundingBox = Frosty::Assetmanager::GetAssetmanager()->GetModeltemplateMetaData(modelFilename)->GetData()->GetMeshInfoConst(0).BoundingBox;
	m_renderData.m_Colour = color; 
	

	//temp fix
	m_BoundingBox.halfSize[0] *= 2;
	m_BoundingBox.halfSize[1] *= 2;
	m_BoundingBox.halfSize[2] *= 2;

	m_Pos = position;
	m_Rotation = rotation;
	m_Scale = scale;
	UpdateWorldMatrix();

	Frosty::Application::Get().GetRenderEngine()->AddToRenderList(&m_renderData);

}

GameObject::~GameObject()
{
}

void GameObject::SetPos(glm::vec3 newPos)
{
	m_Pos = newPos;
}

void GameObject::SetColour(const glm::vec3& Colour)
{
	m_renderData.m_Colour = Colour;
}

void GameObject::SetScale(glm::vec3 newScale)
{
	m_Scale = newScale;
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

Frosty::SphereHitbox& GameObject::GetSphereHitbox()
{
	return m_SphereHitBox;
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

void GameObject::UpdateWorldMatrix()
{
	m_renderData.worldPosition = glm::mat4(1.0f);
	m_renderData.worldPosition = glm::translate(m_renderData.worldPosition, m_Pos);
	m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	m_renderData.worldPosition = glm::scale(m_renderData.worldPosition, m_Scale);



	//test
	glm::mat4 TempMat;
	//TempMat = glm::mat4(1.0f);
	//TempMat = glm::translate(TempMat, m_Pos);
	//

	//m_SphereHitBox.m_Position = glm::vec3(TempMat * glm::vec4(m_SphereHitBox.m_Position,1.0));

	m_SphereHitBox.m_Position = m_Pos;

	TempMat = glm::mat4(1.0f);
	TempMat = glm::rotate(TempMat, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	TempMat = glm::rotate(TempMat, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	TempMat = glm::rotate(TempMat, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));









	glm::vec3 rotate = m_Rotation + glm::vec3(0.0f,0.0f,90.0f);

	glm::mat4 rotateX = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(glm::radians(rotate.x)), -sin(glm::radians(rotate.x)), 0.0f,
		0.0f, sin(glm::radians(rotate.x)), cos(glm::radians(rotate.x)), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	glm::mat4 rotateY = glm::mat4(
		cos(glm::radians(rotate.y)), 0.0f, sin(glm::radians(rotate.y)), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin(glm::radians(rotate.y)), 0.0f, cos(glm::radians(rotate.y)), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	glm::mat4 rotateZ = glm::mat4(
		cos(glm::radians(rotate.z)), -sin(glm::radians(rotate.z)), 0.0f, 0.0f,
		sin(glm::radians(rotate.z)), cos(glm::radians(rotate.z)), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	glm::mat4 temp_Rot = rotateY * rotateX * rotateZ;









	m_SphereHitBox.m_Direction = glm::normalize(glm::vec3(temp_Rot * glm::vec4(0.0f,1.0f,0.0f,1.0f)));


	//TempMat = glm::scale(m_renderData.worldPosition, m_Scale);



}
