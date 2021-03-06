#ifndef GAMEPAUSESTATE_HPP
#define GAMEPAUSESTATE_HPP

namespace MCS
{
	class GamePauseState : public Frosty::State
	{
	public:
		GamePauseState();
		virtual ~GamePauseState();

		virtual void Initiate() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;
	private:
		void InitiateGui();
	private:
		Frosty::Application* m_App = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_Player = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_GamePauseGui = nullptr;
		Frosty::UILayout m_UILayout;

		bool m_ButtonsLoaded = false;
	};
}
#endif
