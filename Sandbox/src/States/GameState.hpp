#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

namespace MCS
{
	class GameState : public Frosty::State
	{
	public:
		GameState();
		virtual ~GameState();

		virtual void Initiate() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;

		//virtual void Pause();
		//virtual void Resume();
	private:
		void InitiateGui();
		void InitiateLight();

		// Temp
		void InitiateObjects();
	private:
		Frosty::Application* m_App = nullptr;
		Frosty::World* m_World = nullptr;

		std::shared_ptr<Frosty::ECS::Entity> m_GameGui;
	};
}
#endif
