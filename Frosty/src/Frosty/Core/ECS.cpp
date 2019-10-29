#include <fypch.hpp>
#include "ECS.hpp"
#include "Frosty/Core/Application.hpp"
#include "Frosty/Core/InputManager.hpp"

namespace Frosty
{
	namespace ECS
	{
		std::string CTransform::NAME = "Transform";
		std::string CMesh::NAME = "Mesh";
		std::string CCamera::NAME = "Camera";
		std::string CMaterial::NAME = "Material";
		std::string CMotion::NAME = "Motion";
		std::string CController::NAME = "Controller";
		std::string CFollow::NAME = "Follow";
		std::string CLight::NAME = "Light";
		std::string CCollision::NAME = "Collision";
		std::string CPlayerAttack::NAME = "PlayerAttack";
		std::string CEnemyAttack::NAME = "EnemyAttack";
		std::string CHealth::NAME = "Health";
		std::string CTag::NAME = "Tag";
		std::string CConsumables::NAME = "Consumables";
		std::string CArrow::NAME = "Arrow";
		std::string CHealthBar::NAME = "HealthBar";
	}
}