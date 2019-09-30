#ifndef FROSTY_HPP
#define FROSTY_HPP

#include "Editor/InspectorLayer.hpp"
#include "imgui/imgui.h"

namespace MCS
{
	class Game : public Frosty::Application
	{
	public:
		Game();
		virtual ~Game();

	private:
		Frosty::Application* m_App;

	};
}

#endif // !FROSTY_HPP
