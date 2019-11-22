#ifndef LOADINGSTATE_HPP
#define LOADINGSTATE_HPP

namespace MCS
{
	class LoadingState : public Frosty::State
	{
	public:
		LoadingState();
		virtual ~LoadingState();

		virtual void Initiate() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;
		virtual void OnLastUpdate() override;
	private:
		void InitiateSystems();
		void InitiateLoadingScreen();
		void InitiateGui();
		void InitiateMedia();

		void InitiateNavigationSystem();
		void InitiateParticleSystem();
		void InitiateMapGenerator();
	private:
		Frosty::Application* m_App;
		Frosty::World* m_World = nullptr;
		bool m_IsInitialized = false;

		std::shared_ptr<Frosty::ECS::Entity> m_LoadGui;
	};
}
#endif
