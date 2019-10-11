#include <mcspch.hpp>
#include "InspectorLayer.hpp"
#include "Frosty/API/AssetManager.hpp"

#include "imgui/imgui.h"
#include <GLFW/glfw3.h>
#include <PugiXML/pugixml.hpp>

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
		auto& world = m_App->GetWorld();

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
			ImGui::Checkbox("Editor Camera: ", m_App->GetEditorCamera().ActiveStatus());
			if (ImGui::Button("Create Entity", ImVec2(100.0f, 20.0f)))
			{
				world->CreateEntity();
			}

			static int selection_mask = 0;
			int node_clicked = -1;
			unsigned int counter = 0;
			for (auto& entity : *world->GetEntityManager())
			{
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ((selection_mask & (1 << counter)) ? ImGuiTreeNodeFlags_Selected : 0); // ImGuiTreeNodeFlags_Bullet
				Frosty::ECS::EntityID eid = entity->Id;
				ImGui::TreeNodeEx((void*)(intptr_t)counter, node_flags, "Entity (%d)", eid);
				if (ImGui::IsItemClicked())
				{
					node_clicked = (int)counter;
					m_SelectedEntity = entity;
				}
				counter++;
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
					if (world->HasComponent<Frosty::ECS::CMesh>(m_SelectedEntity)) toggles[1] = true;
					if (world->HasComponent<Frosty::ECS::CCamera>(m_SelectedEntity)) toggles[2] = true;
					if (world->HasComponent<Frosty::ECS::CMaterial>(m_SelectedEntity)) toggles[3] = true;
				}

				// Information
				ImGui::Text("Entity (%i)", m_SelectedEntity->Id);

				if (world->HasComponent<Frosty::ECS::CTransform>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Transform"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CTransform>(m_SelectedEntity);
						ImGui::BeginChild("CTransform", ImVec2(EDITOR_INSPECTOR_WIDTH, 80), true);
						if (ImGui::DragFloat3("Position", glm::value_ptr(comp.Position), 0.1f, 0.0f, 0.0f, "%.2f")) { comp.UpdateTransform = true; }
						if (ImGui::DragFloat3("Rotation", glm::value_ptr(comp.Rotation), 0.1f, 0.0f, 0.0f, "%.2f")) { comp.UpdateTransform = true; }
						if (ImGui::DragFloat3("Scale", glm::value_ptr(comp.Scale), 0.1f, 0.0f, 0.0f, "%.2f")) { comp.UpdateTransform = true; }
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CMesh>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Mesh"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CMesh>(m_SelectedEntity);
						ImGui::BeginChild("CMesh", ImVec2(EDITOR_INSPECTOR_WIDTH, 30), true);
						if (ImGui::Button("Select mesh.."))
							ImGui::OpenPopup("Mesh selector");
						if (ImGui::BeginPopupModal("Mesh selector", NULL, ImGuiWindowFlags_MenuBar))
						{
							//auto& meshes = Frosty::AssetManager::GetMeshes();
							size_t index = 0;
							ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

							for (auto& mesh : Frosty::AssetManager::GetMeshes())
							{
								ImGui::TreeNodeEx((void*)(intptr_t)index, node_flags, "%s", mesh.first.c_str());
								if (ImGui::IsItemClicked())
								{
									world->GetComponent<Frosty::ECS::CMesh>(m_SelectedEntity).Mesh = mesh.second;
								}
							}

							if (ImGui::Button("Close"))
								ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
						}
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CCamera>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Camera"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CCamera>(m_SelectedEntity);
						ImGui::BeginChild("Camera", ImVec2(EDITOR_INSPECTOR_WIDTH, 70), true);
						if (ImGui::DragFloat("Field of View", &comp.FieldOfView, 0.1f, 1.0f, 179.0f, "%.1f")) { comp.UpdateProjection = true; }
						if (ImGui::DragFloat("Near", &comp.Near, 0.1f, 0.1f, comp.Far - 0.1f, "%.2f")) { comp.UpdateProjection = true; }
						if (ImGui::DragFloat("Far", &comp.Far, 0.1f, comp.Near + 0.1f, 10000.0f, "%.2f")) { comp.UpdateProjection = true; }
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CMaterial>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Material"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CMaterial>(m_SelectedEntity);
						ImGui::BeginChild("Material", ImVec2(EDITOR_INSPECTOR_WIDTH, 100), true);

						if (ImGui::Button("Shader"))
							ImGui::OpenPopup("shader_select_popup");
						ImGui::SameLine();
						ImGui::TextUnformatted(comp.UseShader->GetName().c_str());
						if (ImGui::BeginPopup("shader_select_popup"))
						{
							ImGui::Separator();
							for (auto& shader : Frosty::AssetManager::GetShaders())
							{
								if (ImGui::Selectable(shader.first.c_str()))
								{
									//comp.UseShader->Bind();
									//comp.UseShader->UploadUniformInt("u_Texture", 0);
									comp.UseShader = shader.second;
								}
							}
							ImGui::EndPopup();
						}

						// Parameters
						if (comp.UseShader->GetName() == "FlatColor") ImGui::ColorEdit4("Albedo", glm::value_ptr(comp.Albedo));
						if (comp.UseShader->GetName() == "Texture2D")
						{
							// DIFFUSE // 
							if (ImGui::Button("Diffuse Texture"))
								ImGui::OpenPopup("diffuse_texture_select_popup");
							ImGui::SameLine();
							ImGui::TextUnformatted(comp.DiffuseTexture ? comp.DiffuseTexture->GetName().c_str() : "None");
							if (ImGui::BeginPopup("diffuse_texture_select_popup"))
							{
								ImGui::Separator();
								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									if (ImGui::Selectable(texture.first.c_str()))
									{
										comp.DiffuseTexture = texture.second;
									}
								}
								ImGui::EndPopup();
							}
							// Gloss // 
							if (ImGui::Button("Gloss Texture"))
								ImGui::OpenPopup("gloss_texture_select_popup");
							ImGui::SameLine();
							ImGui::TextUnformatted(comp.GlossTexture ? comp.GlossTexture->GetName().c_str() : "None");
							if (ImGui::BeginPopup("gloss_texture_select_popup"))
							{
								ImGui::Separator();
								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									if (ImGui::Selectable(texture.first.c_str()))
									{
										comp.GlossTexture = texture.second;
									}
								}
								ImGui::EndPopup();
							}
							// NORMAL // 
							if (ImGui::Button("Normal Texture"))
								ImGui::OpenPopup("normal_texture_select_popup");
							ImGui::SameLine();
							ImGui::TextUnformatted(comp.NormalTexture ? comp.NormalTexture->GetName().c_str() : "None");
							if (ImGui::BeginPopup("normal_texture_select_popup"))
							{
								ImGui::Separator();
								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									if (ImGui::Selectable(texture.first.c_str()))
									{
										comp.NormalTexture = texture.second;
									}
								}
								ImGui::EndPopup();
							}
						}

						// Add more parameters like texture etc

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
						if (!world->HasComponent<Frosty::ECS::CMesh>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CMesh>(m_SelectedEntity, Frosty::AssetManager::GetMesh("Table"));
						else
							world->RemoveComponent<Frosty::ECS::CMesh>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Camera", "", &toggles[2]))
					{
						if (!world->HasComponent<Frosty::ECS::CCamera>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CCamera>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CCamera>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Material", "", &toggles[3]))
					{
						if (!world->HasComponent<Frosty::ECS::CMaterial>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CMaterial>(m_SelectedEntity, Frosty::AssetManager::GetShader("FlatColor"));
						else
							world->RemoveComponent<Frosty::ECS::CMaterial>(m_SelectedEntity);
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
				if (ImGui::MenuItem("Save", "CTRL+S"))
				{

				}
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
			ImGui::SetCursorPosX(m_App->GetWindow().GetWidth() - 80.0f);
			if (m_App->GameIsRunning())
			{
				if (ImGui::Button("Stop", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
				{
					m_App->StopGame();
				}
			}
			else
			{
				if (ImGui::Button("Play", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
				{
					m_App->StartGame();
				}
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
		if (m_SelectedEntity && e.GetKeyCode() == GLFW_KEY_DELETE)
		{
			m_App->GetWorld()->RemoveEntity(m_SelectedEntity);
			m_SelectedEntity = nullptr;
		}

		return false;
	}

}
