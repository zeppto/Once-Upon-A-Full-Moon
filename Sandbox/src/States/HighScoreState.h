#ifndef HIGHSCORESTATE_HPP
#define HIGHSCORESTATE_HPP

namespace MCS
{
	class HighscoreState : public Frosty::State
	{
	public:
		HighscoreState();
		virtual ~HighscoreState();

		virtual void Initiate() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;

	private:
		void InitiateGUI();

	private:
		Frosty::Application* m_App = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_HighscoreGUI = nullptr;
	};
}
#endif

