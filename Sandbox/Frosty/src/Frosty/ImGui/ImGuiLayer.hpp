#ifndef IMGUI_LAYER_HPP
#define IMGUI_LAYER_HPP

#include "Frosty/Core/Layer.hpp"

namespace Frosty
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		ImGuiLayer(const std::string& name);
		virtual ~ImGuiLayer();

		void Begin();
		void End();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

	private:
		float m_Time = 0.0f;
	};
}

#endif // !IMGUI_LAYER_HPP
