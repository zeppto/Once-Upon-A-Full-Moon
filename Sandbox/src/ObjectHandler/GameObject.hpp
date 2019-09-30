#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP
#include "Frosty.h"

class GameObject
{
public:


private:
	Frosty::TempRender m_renderData;

	glm::vec3 m_Pos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);


public:
	GameObject(glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
	~GameObject();


	void SetPos(glm::vec3 newPos);
	void SetScale(glm::vec3 newScale);
	void SetRotation(glm::vec3 newRotation);

	glm::vec3 GetPos() const;
	glm::vec3 GetScale() const;
	glm::vec3 GetRotation() const;

	void UpdateWorldMatrix();

private:

};


#endif
