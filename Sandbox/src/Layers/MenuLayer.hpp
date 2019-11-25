#ifndef MENULAYER_HPP
#define MENULAYER_HPP

namespace MCS
{
	class MenuLayer : public Frosty::Layer
	{
	public:
		MenuLayer() : Layer("MenuLayer") {}
		virtual ~MenuLayer() {};

		inline virtual void OnAttach() override { m_App = &Frosty::Application::Get(); }
		inline virtual void OnDetach() override {}
		inline virtual void OnUpdate() override {};

		virtual void OnImGuiRender() override {}
		inline virtual bool OnEvent(Frosty::BaseEvent& event) { return false; }
	private:
		Frosty::Application* m_App = nullptr;
	};
}
#endif
