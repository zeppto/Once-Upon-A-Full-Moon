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

		void OnGameOverEvent();
	private:
		Frosty::Application* m_App = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_GameOverGui;
		Frosty::UILayout m_UILayout;

		bool m_ButtonsLoaded = false;
	};
}
#endif
