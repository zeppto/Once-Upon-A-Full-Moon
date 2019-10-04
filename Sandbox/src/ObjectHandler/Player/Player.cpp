#include "Player.hpp"


	Player::Player(glm::vec3 color, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, int maxHp, std::string modelFilename) : GameObject(color, position, rotation, scale, maxHp, modelFilename)
	{
		
		//m_renderData.model_ptr = Frosty::Assetmanager::GetAssetmanager()->GetModeltemplateMetaData("tempPlayer")->GetData();
		//m_renderData.material_Ptr = Frosty::Assetmanager::GetAssetmanager()->GetMaterialMetaData("Mat_0:tempPlayer")->GetData();
		//


		//m_renderData.worldPosition = glm::mat4(1.0f);

		//Frosty::Application::Get().GetRenderEngine()->AddToRenderList(&m_renderData);

	}

	Player::~Player()
	{
	}

	void Player::UpdatePlayer()
	{
		SetPos(GetPos() + m_PlayerController.PlayerControllerMovement());
		//m_Pos += m_PlayerController.PlayerControllerMovement();
		
		//UpdateWorldMatrix();

		//glm::mat4 tempWorld = glm::mat4(1.0f);
		//tempWorld = glm::translate(tempWorld, GetPos());
		//tempWorld = glm::rotate(tempWorld, glm::radians(GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
		//tempWorld = glm::rotate(tempWorld, glm::radians(GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
		//tempWorld = glm::rotate(tempWorld, glm::radians(GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
		//tempWorld = glm::scale(tempWorld, GetScale());
		//SetWorldMatrix(tempWorld);

		//FY_INFO("x: {0}, y:{1}, z:{2}", m_Pos.x, m_Pos.y, m_Pos.z);






	}

