#include <mcspch.hpp>
#include "InspectorLayer.hpp"

#include "imgui/imgui.h"
#include <GLFW/glfw3.h>

namespace MCS
{
	bool InspectorLayer::s_VSync = false;

	InspectorLayer::InspectorLayer()
	{

	}

	void InspectorLayer::OnAttach()
	{
		m_App = &Frosty::Application::Get();
	}

	void InspectorLayer::OnDetach()
	{

	}

	void InspectorLayer::OnUpdate()
	{

	}

	void InspectorLayer::OnImGuiRender()
	{
		auto& scene = m_App->GetScene();

		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
			window_flags |= ImGuiWindowFlags_NoBackground;


		// --------Explorer Window --------------------------------------------
		ImGui::SetNextWindowPos(ImVec2((float)m_App->GetWindow().GetPositionX(), (float)(m_App->GetWindow().GetPositionY() + EDITOR_MAIN_MENU_BAR_HEIGHT)));
		ImGui::SetNextWindowSize(ImVec2((float)EDITOR_EXPLORER_WIDTH, (float)(m_App->GetWindow().GetHeight() - EDITOR_ASSETS_HEIGHT - EDITOR_MAIN_MENU_BAR_HEIGHT)));

		ImGui::Begin("Explorer", NULL, window_flags);
		{
			ImGui::Text("Delta Time: %f", Frosty::Time::DeltaTime());
			ImGui::Text("FPS: %i", Frosty::Time::FPS());
			if (ImGui::Checkbox("VSync: ", &s_VSync))
			{
				m_App->GetWindow().SetVSync(s_VSync);
			}
			if (ImGui::Button("Create Entity", ImVec2(100.0f, 20.0f)))
			{
				scene->CreateEntity();
			}

			static int selection_mask = 0;
			int node_clicked = -1;
			for (size_t i = 0; i < scene->GetTotalEntities(); i++)
			{
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0); // ImGuiTreeNodeFlags_Bullet
				Frosty::ECS::EntityID eid = scene->At(i)->Id;
				ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Entity (%d)", eid);
				if (ImGui::IsItemClicked())
				{
					node_clicked = (int)i;
					m_SelectedEntity = scene->At(i);
				}
			}
			if (node_clicked != -1)
			{
				// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
				if (ImGui::GetIO().MouseClicked)
					selection_mask = (1 << node_clicked);
			
				if (ImGui::GetIO().KeyCtrl)
					selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
				//else if (ImGui::GetIO().MouseClicked)
				//	selection_mask = (1 << node_clicked);
				//else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
				//	selection_mask = (1 << node_clicked);           // Click to single-select
			}
		}
		ImGui::End();



		// --------Inspector Window -------------------------------------------
		ImGui::SetNextWindowPos(ImVec2((float)(m_App->GetWindow().GetPositionX() + m_App->GetWindow().GetWidth() - EDITOR_INSPECTOR_WIDTH), (float)(m_App->GetWindow().GetPositionY() + EDITOR_MAIN_MENU_BAR_HEIGHT)));
		ImGui::SetNextWindowSize(ImVec2((float)EDITOR_INSPECTOR_WIDTH, (float)(m_App->GetWindow().GetHeight() - EDITOR_MAIN_MENU_BAR_HEIGHT)));
		
		ImGui::Begin("Inspector", NULL, window_flags);
		{
			if (m_SelectedEntity)
			{
				std::string names[] = { "CTransform" };
				static bool toggles[] = { true };

				ImGui::Text("Entity (%i)", m_SelectedEntity->Id);
				if (ImGui::Button("Add component.."))
					ImGui::OpenPopup("my_toggle_popup");
				if (ImGui::BeginPopup("my_toggle_popup"))
				{
					for (int i = 0; i < 1; i++)
						ImGui::MenuItem(names[i].c_str(), "", &toggles[i]);
	
					ImGui::EndPopup();
				}
			}
		}
		ImGui::End();



		// --------Assets Window ----------------------------------------------
		ImGui::SetNextWindowPos(ImVec2((float)m_App->GetWindow().GetPositionX(), (float)(m_App->GetWindow().GetPositionY() + m_App->GetWindow().GetHeight() - EDITOR_ASSETS_HEIGHT)));
		ImGui::SetNextWindowSize(ImVec2((float)(m_App->GetWindow().GetWidth() - EDITOR_INSPECTOR_WIDTH), (float)EDITOR_ASSETS_HEIGHT));

		ImGui::Begin("Assets", NULL, window_flags);
		{

		}
		ImGui::End();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save", "CTRL+S")) {}
				if (ImGui::MenuItem("Save All", "CTRL+SHIFT+S")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	bool InspectorLayer::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::KeyPressed:
			return OnKeyPressedEvent(static_cast<Frosty::KeyPressedEvent&>(e));
		default:
			return false;
		}

		return true;
	}

	bool InspectorLayer::OnKeyPressedEvent(Frosty::KeyPressedEvent & e)
	{
		return false;
	}

}