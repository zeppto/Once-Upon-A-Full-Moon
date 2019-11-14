#ifndef FROSTY_HPP
#define FROSTY_HPP

#include <imgui/imgui.h>

namespace MCS
{

	class Game : public Frosty::Application
	{
	public:
		Game();
		void RunGame();
		virtual ~Game();

	};
}

#endif // !FROSTY_HPP
