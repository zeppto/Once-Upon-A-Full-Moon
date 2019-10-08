#include<fypch.hpp>
#include"CollisionMap.hpp"


namespace Frosty
{

	CollisionMap* CollisionMap::s_Instance = nullptr;


	CollisionMap* CollisionMap::Get() 
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new CollisionMap;
		}
		return s_Instance;

	}

	void CollisionMap::Initiate()
	{
		m_Camera = std::make_shared<Camera>(Camera());
		//m_CameraData.AspRatio = Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight();
		//m_CameraData.Projection = glm::perspective(m_CameraData.FoV, m_CameraData.AspRatio, m_CameraData.NearPlane, m_CameraData.FarPlane);

		m_Camera->GetCameraTranslationData().Pos = glm::vec3(((float)MAP_WITDH_PIXELS)/2,10.0f, ((float)MAP_HEIGHT_PIXELS) / 2);
		m_Camera->GetCameraTranslationData().LookAtVec = glm::vec3(0.0f, -1.0f, 0.0f);
		m_Camera->GetCameraTranslationData().UpVec = glm::vec3(0.0f, 0.0f, -1.0f);
		m_Camera->GetCameraTranslationData().CamSpeed = 10.0f;
				
		m_Camera->GetCameraData().FarPlane = 1000.0f;
		m_Camera->GetCameraData().NearPlane = 0.1f;
		m_Camera->GetCameraData().FoV = glm::radians(70.0f);
				
		m_Camera->GetCameraData().AspRatio = MAP_WITDH_PIXELS / MAP_HEIGHT_PIXELS;
				
				
		m_Camera->GetCameraData().Projection = glm::perspective(m_Camera->GetCameraData().FoV, m_Camera->GetCameraData().AspRatio, m_Camera->GetCameraData().NearPlane, m_Camera->GetCameraData().FarPlane);
		m_Camera->GetCameraData().OrthoGraphic = glm::ortho(0.0f, (float)MAP_WITDH_PIXELS/2, 0.0f, (float)MAP_HEIGHT_PIXELS/2,1.0f,1000.0f);
		m_Camera->GetCameraData().View = glm::lookAt(m_Camera->GetCameraTranslationData().Pos,glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		m_Camera->GetCameraData().Projection = m_Camera->GetCameraData().OrthoGraphic;


	}

	std::shared_ptr<Camera>& CollisionMap::GetCamera()
	{
		//return std::make_shared<Camera>(m_Camera);
		return m_Camera;
	}













}