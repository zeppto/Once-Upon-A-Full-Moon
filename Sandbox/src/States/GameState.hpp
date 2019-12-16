#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

namespace Frosty { 
				   class GameoverEvent;
				 }

namespace MCS
{
	class GameState : public Frosty::State
	{
	public:
		GameState();
		virtual ~GameState() = default;

		virtual void Initiate() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;

		void OnEvent(Frosty::BaseEvent& e);

		void InitiateLight();

		void OnGameOverEvent(Frosty::GameoverEvent& e);
		void OnGameWinEvent();
	private:
		Frosty::Application* m_App = nullptr;

		//// Temp
		//bool HeatMap;
		//float ClickButtonTimer = Frosty::Time::CurrentTime();
	};
}
#endif
