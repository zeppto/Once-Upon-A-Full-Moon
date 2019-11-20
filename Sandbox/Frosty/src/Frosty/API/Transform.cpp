#include <fypch.hpp>
#include "Transform.h"

namespace Frosty
{
	Transform::Transform()
	{
		m_Anchor_vec3 = glm::vec3(0.0f, 0.0f, 0.0f);
		m_Translate_vec3 = glm::vec3(0.0f, 0.0f, 0.0f);
		m_Rotate_vec3 = glm::vec3(0.0f, 0.0f, 0.0f);
		m_Scale_vec3 = glm::vec3(1.0f, 1.0f, 1.0f);

		this->m_Translate_mat4 = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			m_Translate_vec3.x, m_Translate_vec3.y, m_Translate_vec3.z, 1.0f
		);

		glm::mat4 rotateX = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, cos(glm::radians(m_Rotate_vec3.x)), -sin(glm::radians(m_Rotate_vec3.x)), 0.0f,
			0.0f, sin(glm::radians(m_Rotate_vec3.x)), cos(glm::radians(m_Rotate_vec3.x)), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 rotateY = glm::mat4(
			cos(glm::radians(m_Rotate_vec3.y)), 0.0f, sin(glm::radians(m_Rotate_vec3.y)), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-sin(glm::radians(m_Rotate_vec3.y)), 0.0f, cos(glm::radians(m_Rotate_vec3.y)), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		glm::mat4 rotateZ = glm::mat4(
			cos(glm::radians(m_Rotate_vec3.z)), -sin(glm::radians(m_Rotate_vec3.z)), 0.0f, 0.0f,
			sin(glm::radians(m_Rotate_vec3.z)), cos(glm::radians(m_Rotate_vec3.z)), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		m_Rotate_mat4 = rotateY * rotateX * rotateZ;

		m_Scale_mat4 = glm::mat4(
			m_Scale_vec3.x, 0.0f, 0.0f, 0.0f,
			0.0f, m_Scale_vec3.y, 0.0f, 0.0f,
			0.0f, 0.0f, m_Scale_vec3.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}


	Transform::~Transform()
	{
	}

	void Transform::SetAnchor(glm::vec3 anchor)
	{
	}


	void Transform::SetTranslate(glm::vec3 translate)
	{
		m_Translate_vec3 = translate;

		m_Translate_mat4 = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			translate.x, translate.y, translate.z, 1.0f
		);
	}

	void Transform::SetRotate(glm::vec3 rotate)
	{
		m_Rotate_vec3 = rotate;

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

		m_Rotate_mat4 = rotateY * rotateX * rotateZ;

	}

	void Transform::SetScale(glm::vec3 scale)
	{
		m_Scale_vec3 = scale;

		m_Scale_mat4 = glm::mat4(
			scale.x, 0.0f, 0.0f, 0.0f,
			0.0f, scale.y, 0.0f, 0.0f,
			0.0f, 0.0f, scale.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	glm::vec3 Transform::GetAnchor()
	{
		return m_Anchor_vec3;
	}

	glm::vec3 Transform::GetTranslate()
	{
		return m_Translate_vec3;
	}

	glm::vec3 Transform::GetRotate()
	{
		return m_Rotate_vec3;
	}

	glm::vec3 Transform::GetScale()
	{
		return m_Scale_vec3;
	}

	glm::mat4 Transform::GetModel()
	{
		return (m_Translate_mat4 * m_Rotate_mat4 * m_Scale_mat4);
	}
}