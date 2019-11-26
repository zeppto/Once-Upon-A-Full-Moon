#ifndef GAMEPAUSELAYER_HPP
#define GAMEPAUSELAYER_HPP

namespace MCS
{
	class GamePauseLayer : public Frosty::Layer
	{
	public:
		GamePauseLayer() : Layer("GamePauseLayer") {}
		virtual ~GamePauseLayer() {}

		virtual void OnAttach() override { m_App = &Frosty::Application::Get(); }
		virtual void OnDetach() override { }
		virtual void OnUpdate() override { }

		virtual void OnImGuiRender() override {};
		virtual bool OnEvent(Frosty::BaseEvent& event) { return false; }
	private:
		Frosty::Application* m_App;
	};
}
#endif 
