#ifndef GAMEOVERSTATE_HPP
#define GAMEOVERSTATE_HPP

namespace MCS
{
	class GameOverState : public Frosty::State
	{
	public:
		GameOverState();
		virtual ~GameOverState();

		virtual void Initiate() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;
		void OnEvent(Frosty::BaseEvent& e);
	private:
		void InitiateGameOverGui();
		void InitiateGameWinGui();

		void DeleteGameOverGui();
		void DeleteGameWinGui();
	private:
		Frosty::Application* m_App = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_GameOverGui;
		std::shared_ptr<Frosty::ECS::Entity> m_GameWinGui;
	};
}
#endif
