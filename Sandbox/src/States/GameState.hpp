#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

namespace MCS
{
	class GameState : public Frosty::State
	{
	public:
		GameState();
		virtual ~GameState();

		virtual void Initiate() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;

		void DeletePlayer();
		std::shared_ptr<Frosty::ECS::Entity> GetPlayer();
	private:
		void InitiateLight();

		// Temp
		void InitiateObjects();
		void SetPlayer();
		//void DeletePlayer();
		void DeleteLight();	

	private:
		Frosty::Application* m_App = nullptr;
		std::shared_ptr<Frosty::ECS::Entity> m_Player;
		std::shared_ptr<Frosty::ECS::Entity> m_Light;
		std::shared_ptr<Frosty::ECS::Entity> m_Torch;
		//std::shared_ptr<Frosty::ECS::Entity> m_Weapon;
		//std::shared_ptr<Frosty::ECS::Entity> m_Enemy;

		// Temp
		//glm::vec3 m_PlayerFirstPos = glm::vec3(-104.0f, 0.0f, -15.4f);
		//glm::vec3 m_PlayerLastPos = glm::vec3(-104.0f, 0.0f, -15.4f);
	};
}
#endif
