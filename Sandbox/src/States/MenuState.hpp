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
		void InitiateObjects();
		void InitiateButtons();
		void InitiateInstructions();
	private:
		Frosty::Application* m_App = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_MenuGui = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_InstructionGui = nullptr;
		Frosty::UILayout m_UILayout;
		Frosty::UILayout m_UILayout2;

		bool m_ButtonsLoaded = false;
		bool m_Controls = false;

		bool m_Instructions = false;
		bool m_InstructionButtonsLoaded = false;
	};
}
#endif