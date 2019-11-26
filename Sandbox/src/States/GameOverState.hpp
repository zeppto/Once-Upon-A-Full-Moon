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
	private:
		Frosty::Application* m_App = nullptr;
	};
}
#endif
