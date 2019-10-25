#ifndef UIELEMENT_H
#define UIELEMENT_H
#include "Frosty/API/Transform.h"

namespace Frosty
{
	class UIElement
	{
	public:
		enum TYPE 
		{
			TEXT,
			SPRITE,
			BUTTON,
			BAR,
			SLIDER
		};
	protected:
		TYPE m_type;
		std::vector<int32_t> m_quadPtrArray;
	private:
		Transform m_Transform;
	public:
		UIElement();
		virtual ~UIElement();

		inline void SetAnchor(glm::vec2 anchor) { m_Transform.SetAnchor(glm::vec3(anchor, 0)); };
		inline void SetPosition(glm::vec2 pos) { m_Transform.SetTranslate(glm::vec3(pos, 0)); };
		inline void SetScale(glm::vec2 scale) { m_Transform.SetScale(glm::vec3(scale, 0)); };
		inline void SetRotate(glm::vec2 rot) { m_Transform.SetRotate(glm::vec3(rot, 0)); };

		inline glm::vec2 GetAnchor() { return glm::vec2(m_Transform.GetAnchor().x, m_Transform.GetAnchor().y); };
		inline glm::vec2 GetPosition() { return glm::vec2(m_Transform.GetTranslate().x, m_Transform.GetTranslate().y); };
		inline glm::vec2 GetScale() { return glm::vec2(m_Transform.GetScale().x, m_Transform.GetScale().y); };
		inline glm::vec2 GetRotate() { return glm::vec2(m_Transform.GetRotate().x, m_Transform.GetRotate().y); };

		inline TYPE GetType() { return m_type; };
		inline std::vector<int32_t> GetQuadPtrArray() { return m_quadPtrArray; };
	private:
	};
}
#endif
