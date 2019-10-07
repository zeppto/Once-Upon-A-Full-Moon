#include <fypch.hpp>
#include "ECS.hpp"

namespace Frosty
{
	namespace ECS
	{
		std::string CTransform::NAME = "Transform";
		std::string CMesh::NAME = "Mesh";
		std::string CCamera::NAME = "Camera";
		std::string CMaterial::NAME = "Material";

		//std::array<std::shared_ptr<BaseComponent>, MAX_COMPONENTS> BaseComponentManager::s_ComponentList = { nullptr };
		//std::shared_ptr<BaseComponent> BaseComponentManager::s_ComponentList = nullptr;



		void BaseComponentManager::Init()
		{
			//int tot = 0;
			//s_ComponentList[tot++].reset(FY_NEW CTransform());
			//s_ComponentList[tot++].reset(FY_NEW CMesh());
		}
	}
}