#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

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

		void OnGameOverEvent();
		void OnGameWinEvent();
	private:
		Frosty::Application* m_App = nullptr;
	};
}
#endif
