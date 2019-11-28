#ifndef STATE_HPP
#define STATE_HPP

namespace Frosty
{
	class State
	{
	public:
		virtual ~State() { }

		virtual void Initiate() = 0;
		virtual void OnInput() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnLastUpdate() { }
		virtual void OnEvent() { }

		virtual void Pause() { }
		virtual void Resume() { }
	};
}
#endif // !STATE_HPP
