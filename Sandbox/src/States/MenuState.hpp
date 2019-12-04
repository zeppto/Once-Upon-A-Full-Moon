#ifndef MENUSTATE_HPP
#define MENUSTATE_HPP
#include "Layers/MenuLayer.hpp"

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
		void InitiateLight();
		void InitiateTorch();
	private:
		Frosty::Application* m_App = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_MenuGui = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_Player = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_Weapon = nullptr;
		Frosty::UILayout m_UILayout;

		bool m_ButtonsLoaded = false;
	};
}
#endif
