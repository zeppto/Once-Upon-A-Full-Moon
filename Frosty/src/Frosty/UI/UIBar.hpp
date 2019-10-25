#ifndef UIBAR_HPP
#define UIBAR_HPP
#include "Frosty/UI/UIElement.h"
#include "Frosty/UI/UISprite.h"

namespace Frosty
{	
	class UiBar : public UIElement
	{
	private:
		TYPE m_Type;
		uint32_t m_Texture;
		Transform m_Transform;
	public:
		UiBar(std::string filename = "unknown", std::string name = "unknown");
		virtual ~UiBar();		

		void SetTexture(std::string filename);

		uint32_t GetTexture();
	};
}
#endif
