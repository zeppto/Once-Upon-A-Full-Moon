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
		//void InitiateMenuScreen();
		void InitiateSystems();
		void InitiateObjects();
		void InitiateButtons();
	private:
		Frosty::Application* m_App = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_MenuGui;
	};
}
#endif
