#ifndef LAYER_HPP
#define LAYER_HPP

namespace Frosty
{
	class Layer
	{
	public:
		Layer(const std::string& debugName = "Layer");
		virtual ~Layer();

		virtual void OnAttach() { }
		virtual void OnDetach() { }
		virtual void OnUpdate() { }
		virtual void OnImGuiRender() { }
		virtual bool OnEvent(BaseEvent& event) { return false; }

		inline bool IsActive() { return m_Active; }
		inline void SetActive(bool &active) { m_Active = active; }

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
		bool m_Active = true;
	};
}
#endif // !LAYER_HPP
