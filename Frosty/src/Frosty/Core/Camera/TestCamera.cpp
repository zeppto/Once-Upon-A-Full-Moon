#include "fypch.hpp"
#include "TestCamera.h"

namespace Frosty
{
	TestCamera::TestCamera()
	{
		this->FoV = glm::radians(90.0f);
		this->aspRatio = 1280 / 720;
		this->nearPlane = 0.1;
		this->farPlane = 100.0f;

		this->projection = glm::perspective(this->FoV, this->aspRatio, this->nearPlane, this->farPlane);

		this->camPos = glm::vec3(0.0f, 0.0f, 2.0f);
		this->lookAtVec = glm::vec3(0.0f, 0.0f, -1.0f);
		this->upVec = glm::vec3(0.0f, 1.0f, 0.0f);

		this->view = lookAt(this->camPos, this->camPos + this->lookAtVec, this->upVec);
	}
	TestCamera::~TestCamera()
	{
	}
	glm::mat4 TestCamera::GetView() const
	{
		return view;
	}
	glm::mat4 TestCamera::GetProjection() const
	{
		return projection;
	}	
}
