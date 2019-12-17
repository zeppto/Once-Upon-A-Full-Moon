#ifndef INSPECTOR_LAYER_HPP
#define INSPECTOR_LAYER_HPP


namespace MCS
{
	class InspectorLayer : public Frosty::Layer
	{
	public:
		InspectorLayer() = default;
		virtual ~InspectorLayer() = default;

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

		// Inspector
		Frosty::ECS::CPlayer* m_SelectedController{ nullptr };
		int m_PreviousControllerHotkey;

		// Main Menu
		bool m_MaximizeOnPlay{ false };

		// Static stuff (Mostly temporary)
		static bool s_VSync;
		static bool s_DistanceCulling;
		static bool s_LightCulling;
		static bool s_RenderShadows;
		static bool s_RenderFromPointLight;

		// GUI
		char m_textName[50] = "";
		char m_spriteName[50] = "";

		//FOR OPENING LEVEL
		char m_LevelName[50] = "name";

	};
}

#endif // !INSPECTOR_LAYER_HPP
