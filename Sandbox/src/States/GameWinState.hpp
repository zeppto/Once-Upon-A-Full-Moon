#ifndef GAMEWINSTATE_HPP
#define GAMEWINSTATE_HPP

namespace MCS
{
	class GameWinState : public Frosty::State
	{
	public:
		GameWinState();
		virtual ~GameWinState();

		virtual void Initiate() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;

		void OnGameWinEvent();
	private:
		void InitiateGui();
	private:
		Frosty::Application* m_App = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_GameWinGui;
		Frosty::UILayout m_UILayout;

		bool m_ButtonsLoaded = false;
	};
}
#endif
