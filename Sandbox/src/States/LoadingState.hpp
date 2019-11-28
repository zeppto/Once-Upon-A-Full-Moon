#ifndef LOADINGSTATE_HPP
#define LOADINGSTATE_HPP

namespace MCS
{
	class LoadingState : public Frosty::State
	{
	public:
		LoadingState();
		virtual ~LoadingState() = default;

		virtual void Initiate() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;
	private:
		void InitiateSystems();
		void InitiateLoadingScreen();
		void InitiateMedia();
	private:
		Frosty::Application* m_App = nullptr;
		Frosty::World* m_World = nullptr;
		bool m_IsInitialized = false;
	};
}
#endif
