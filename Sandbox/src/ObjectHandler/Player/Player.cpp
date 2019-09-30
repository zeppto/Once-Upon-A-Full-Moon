#include "Player.hpp"


	Player::Player(glm::vec3 color, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
	{
		
		m_renderData.model_ptr = Frosty::Assetmanager::GetAssetmanager()->GetModeltemplateMetaData("tempPlayer")->GetData();
		m_renderData.material_Ptr = Frosty::Assetmanager::GetAssetmanager()->GetMaterialMetaData("Mat_0:tempPlayer")->GetData();
		


		m_renderData.worldPosition = glm::mat4(1.0f);

		Frosty::Application::Get().GetRenderEngine()->AddToRenderList(&m_renderData);

	}

	Player::~Player()
	{
	}

	void Player::UpdatePlayer()
	{
		m_Pos += m_PlayerController.PlayerControllerMovement();
		

		m_renderData.worldPosition = glm::mat4(1.0f);
		m_renderData.worldPosition = glm::translate(m_renderData.worldPosition, m_Pos);
		m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_renderData.worldPosition = glm::rotate(m_renderData.worldPosition, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		m_renderData.worldPosition = glm::scale(m_renderData.worldPosition, m_Scale);


		FY_INFO("x: {0}, y:{1}, z:{2}", m_Pos.x, m_Pos.y, m_Pos.z);






	}

