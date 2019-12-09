#ifndef MENUSTATE_HPP
#define MENUSTATE_HPP

namespace MCS
{
	class MenuState : public Frosty::State
	{
	public:
		MenuState();
		virtual ~MenuState();

		virtual void Initiate() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;
	private:
		void InitiateSystems();
		void InitiateButtons();
	private:
		Frosty::Application* m_App = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_MenuGui = nullptr;
		Frosty::UILayout m_UILayout;

		bool m_ButtonsLoaded = false;
		bool m_Controls = false;
	};
}
#endif