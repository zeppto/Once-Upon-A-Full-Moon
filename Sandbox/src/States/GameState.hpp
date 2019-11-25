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

		//virtual void Pause();
		//virtual void Resume();
	private:
		void InitiateLight();

		// Temp
		void InitiateObjects();
	private:
		Frosty::Application* m_App = nullptr;
		Frosty::World* m_World = nullptr;
	};
}
#endif
