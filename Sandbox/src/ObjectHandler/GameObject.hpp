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

	Luna::BoundingBox m_BoundingBox;

	//kan flyttas sen
	int m_MaxHp = 10;
	int m_Hp = 10;


public:
	GameObject(glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), int maxHp = 10, std::string modelFilename = "tempGround");
	~GameObject();

	void SetPos(glm::vec3 newPos);
	void SetScale(glm::vec3 newScale);
	void SetRotation(glm::vec3 newRotation);
	void SetRenderColour(glm::vec3 newColour);
	void SetHitBoxLength(glm::vec3 newHitBoxLength);
	void SetHitBoxCenter(glm::vec3 newHitBoxCenter);
	void SetWorldMatrix(glm::mat4 newWorld);
	void SetShouldRender(bool render);
	void SetMaxHp(int newMaxHp, bool setHpToMax);
	void SetHp(int newHp);

	glm::vec3 GetPos() const;
	glm::vec3 GetScale() const;
	glm::vec3 GetRotation() const;
	glm::vec3 GetHitBoxLength() const;
	glm::vec3 GetHitBoxCenter() const;
	int GetMaxHp() const;
	int GetHp() const;

	void UpdateWorldMatrix();
	void UpdateHp();
	//void Update();

private:

};


#endif
