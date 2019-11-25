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
		void InitiateMenuScreen();
		void InitiateButtons();
	private:
		Frosty::Application* m_App = nullptr;
		//MenuLayer * m_MenuLayer;

		Frosty::World* m_World = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_MenuGui;

		// Temp
		std::shared_ptr<Frosty::ECS::Entity> m_TempMenuGui;
	};
}
#endif
