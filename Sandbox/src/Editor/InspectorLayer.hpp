#ifndef INSPECTOR_LAYER_HPP
#define INSPECTOR_LAYER_HPP


namespace MCS
{
	class InspectorLayer : public Frosty::Layer
	{
	public:
		InspectorLayer();
		virtual ~InspectorLayer() { }

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnImGuiRender() override;
		virtual bool OnEvent(Frosty::BaseEvent& e);

	private:
		bool OnKeyPressedEvent(Frosty::KeyPressedEvent& e);

	private:
		Frosty::Application* m_App = nullptr;

		// Explporer
		std::shared_ptr<Frosty::ECS::Entity> m_SelectedEntity;

		// Static stuff (Mostly temporary)
		static bool s_VSync;

	};
}

#endif // !INSPECTOR_LAYER_HPP
