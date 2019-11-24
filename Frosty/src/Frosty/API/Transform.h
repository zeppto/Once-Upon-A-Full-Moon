#pragma once

namespace Frosty
{
	class Transform
	{
	public:
		Transform();
		Transform(const Transform& p);
		~Transform();

		Transform& operator= (const Transform& p);

		void SetAnchor(glm::vec3 anchor);
		void SetTranslate(glm::vec3 translate);
		void SetRotate(glm::vec3 rotate);
		void SetScale(glm::vec3 scale);

		glm::vec3 GetAnchor();
		glm::vec3 GetTranslate();
		glm::vec3 GetRotate();
		glm::vec3 GetScale();
				  
		glm::mat4 GetTransform();

	private:
		glm::vec3 m_Anchor_vec3;
		glm::vec3 m_Translate_vec3;
		glm::vec3 m_Rotate_vec3;
		glm::vec3 m_Scale_vec3;

		glm::mat4 m_Translate_mat4;
		glm::mat4 m_Rotate_mat4;
		glm::mat4 m_Scale_mat4;
	};
}
