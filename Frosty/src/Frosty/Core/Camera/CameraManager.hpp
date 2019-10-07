#ifndef CAMERAMANAGER_HPP
#define CAMERAMANAGER_HPP
#include "Camera.hpp"

namespace Frosty
{
	class CameraManager
	{
	private:
		Camera* m_Camera;
	public:
		CameraManager();
		virtual ~CameraManager();

		void CameraPositionUpdate();
		void CameraRotationUpdate(double xpos, double ypos);
	};
}
#endif
