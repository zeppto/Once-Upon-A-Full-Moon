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

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;



#pragma region ExplorerWindow

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
			}
		}
		ImGui::End();

#pragma endregion ExplorerWindow


#pragma region InspectorWindow

		ImGui::SetNextWindowPos(ImVec2((float)(m_App->GetWindow().GetPositionX() + m_App->GetWindow().GetWidth() - EDITOR_INSPECTOR_WIDTH), (float)(m_App->GetWindow().GetPositionY() + EDITOR_MAIN_MENU_BAR_HEIGHT)));
		ImGui::SetNextWindowSize(ImVec2((float)EDITOR_INSPECTOR_WIDTH, (float)(m_App->GetWindow().GetHeight() - EDITOR_MAIN_MENU_BAR_HEIGHT)));

		ImGui::Begin("Inspector", NULL, window_flags);
		{
			if (m_SelectedEntity)
			{
				// Declarations
				std::string names[Frosty::ECS::MAX_COMPONENTS];
				for (size_t i = 0; i < Frosty::ECS::MAX_COMPONENTS; i++)
				{
					names[i] = Frosty::ECS::GetComponentName(i);
				}

				bool toggles[Frosty::ECS::MAX_COMPONENTS] = { false };
				for (size_t i = 1; i < Frosty::ECS::MAX_COMPONENTS; i++)
				{
					if (scene->HasComponent<Frosty::ECS::CMesh>(m_SelectedEntity))
					{
						toggles[1] = true;
					}
					if (scene->HasComponent<Frosty::ECS::CCamera>(m_SelectedEntity))
					{
						toggles[2] = true;
					}
					if (scene->HasComponent<Frosty::ECS::CMaterial>(m_SelectedEntity))
					{
						toggles[3] = true;
					}
				}

				// Information
				ImGui::Text("Entity (%i)", m_SelectedEntity->Id);

				if (scene->HasComponent<Frosty::ECS::CTransform>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Transform"))
					{
						auto& comp = scene->GetComponent<Frosty::ECS::CTransform>(m_SelectedEntity);
						ImGui::BeginChild("CTransform", ImVec2(EDITOR_INSPECTOR_WIDTH, 80), true);
						ImGui::DragFloat3("Position", glm::value_ptr(comp.Position), 0.1f, 0.0f, 0.0f);
						ImGui::DragFloat3("Rotation", glm::value_ptr(comp.Rotation), 0.1f, 0.0f, 0.0f);
						ImGui::DragFloat3("Scale", glm::value_ptr(comp.Scale), 0.1f, 0.0f, 0.0f);
						ImGui::EndChild();
					}
				}
				if (scene->HasComponent<Frosty::ECS::CMesh>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Mesh"))
					{
						auto& comp = scene->GetComponent<Frosty::ECS::CMesh>(m_SelectedEntity);
						ImGui::BeginChild("CMesh", ImVec2(EDITOR_INSPECTOR_WIDTH, 30), true);
						if (ImGui::Button("Select mesh.."))
							ImGui::OpenPopup("Mesh selector");
						if (ImGui::BeginPopupModal("Mesh selector", NULL, ImGuiWindowFlags_MenuBar))
						{
							// Loop through all meshes
							int node_clicked = -1;

							for (size_t i = 0; i < 2; i++)
							{
								ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
								ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Mesh (%d)", i);
								if (ImGui::IsItemClicked())
								{
									node_clicked = (int)i;
									if (i == 0)		// Quad
									{
										scene->GetComponent<Frosty::ECS::CMesh>(m_SelectedEntity).Mesh = m_App->GetQuadMesh();
									}
									else // Triangle
									{
										scene->GetComponent<Frosty::ECS::CMesh>(m_SelectedEntity).Mesh = m_App->GetTriangleMesh();
									}
								}
							}

							if (ImGui::Button("Close"))
								ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
						}
						ImGui::EndChild();
					}
				}
				if (scene->HasComponent<Frosty::ECS::CCamera>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Camera"))
					{
						auto& comp = scene->GetComponent<Frosty::ECS::CCamera>(m_SelectedEntity);
						ImGui::BeginChild("Camera", ImVec2(EDITOR_INSPECTOR_WIDTH, 70), true);
						ImGui::DragFloat("Field of View", &comp.FieldOfView, 0.1f, 1.0f, 179.0f, "%1f");
						ImGui::DragFloat("Near", &comp.Near, 0.1f, 0.01f, 0.0f, "%2f");
						ImGui::DragFloat("Far", &comp.Far, 0.1f, 0.01f, 0.0f, "%2f");
						ImGui::EndChild();
					}
				}
				if (scene->HasComponent<Frosty::ECS::CMaterial>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Material"))
					{
						static int selected_shader = 0;
						const char* names[] = { "Standard" };
						static bool toggles[] = { true };
						auto& comp = scene->GetComponent<Frosty::ECS::CMaterial>(m_SelectedEntity);
						ImGui::BeginChild("Material", ImVec2(EDITOR_INSPECTOR_WIDTH, 100), true);

						if (ImGui::Button("Shader"))
							ImGui::OpenPopup("shader_select_popup");
						ImGui::SameLine();
						ImGui::TextUnformatted(names[selected_shader]);
						if (ImGui::BeginPopup("shader_select_popup"))
						{
							ImGui::Separator();
							for (int i = 0; i < IM_ARRAYSIZE(names); i++)
								if (ImGui::Selectable(names[i]))
									selected_shader = i;
							ImGui::EndPopup();
						}
						ImGui::ColorEdit4("Albedo", glm::value_ptr(comp.Albedo));

						ImGui::EndChild();
					}
				}

				// Add Component stuff
				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2.0f - 50.0f, ImGui::GetWindowSize().y - 100.0f));
				if (ImGui::Button("Add component..", ImVec2(120.0f, 20.0f)))
				{
					ImGui::OpenPopup("component_popup");
				}
				if (ImGui::BeginPopup("component_popup"))
				{
					if (ImGui::MenuItem("Mesh", "", &toggles[1]))
					{
						if (!scene->HasComponent<Frosty::ECS::CMesh>(m_SelectedEntity))
							scene->AddComponent<Frosty::ECS::CMesh>(m_SelectedEntity, m_App->GetTriangleMesh());
						else
							scene->RemoveComponent<Frosty::ECS::CMesh>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Camera", "", &toggles[2]))
					{
						if (!scene->HasComponent<Frosty::ECS::CCamera>(m_SelectedEntity))
							scene->AddComponent<Frosty::ECS::CCamera>(m_SelectedEntity);
						else
							scene->RemoveComponent<Frosty::ECS::CCamera>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Material", "", &toggles[3]))
					{
						if (!scene->HasComponent<Frosty::ECS::CMaterial>(m_SelectedEntity))
							scene->AddComponent<Frosty::ECS::CMaterial>(m_SelectedEntity, m_App->GetShader());
						else
							scene->RemoveComponent<Frosty::ECS::CMaterial>(m_SelectedEntity);
					}

					ImGui::EndPopup();
				}
			}
		}
		ImGui::End();

#pragma endregion InspectorWindow


#pragma region AssetsWindow

		// --------Assets Window ----------------------------------------------
		ImGui::SetNextWindowPos(ImVec2((float)m_App->GetWindow().GetPositionX(), (float)(m_App->GetWindow().GetPositionY() + m_App->GetWindow().GetHeight() - EDITOR_ASSETS_HEIGHT)));
		ImGui::SetNextWindowSize(ImVec2((float)(m_App->GetWindow().GetWidth() - EDITOR_INSPECTOR_WIDTH), (float)EDITOR_ASSETS_HEIGHT));

		ImGui::Begin("Assets", NULL, window_flags);
		{

		}
		ImGui::End();

#pragma endregion AssetsWindow


#pragma region MainMenuBar

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

#pragma endregion MainMenuBar
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