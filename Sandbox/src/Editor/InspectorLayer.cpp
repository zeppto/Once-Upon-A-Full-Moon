#include <mcspch.hpp>
#include "InspectorLayer.hpp"
#include "Frosty/API/AssetManager.hpp"

#include "imgui/imgui.h"
#include <PugiXML/pugixml.hpp>

namespace MCS
{
	bool InspectorLayer::s_VSync = false;

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
			if (ImGui::Checkbox("VSync: ", &s_VSync)) m_App->GetWindow().SetVSync(s_VSync);
			if (ImGui::Button("Create Entity", ImVec2(100.0f, 20.0f))) world->CreateEntity();

			if (m_SelectedEntity)
			{
				auto& comp = m_App->GetWorld()->GetComponent<Frosty::ECS::CTransform>(m_SelectedEntity);
				glm::vec3 radRot = glm::radians(comp.Rotation);
				ImGui::InputFloat3("Rotations in rad", glm::value_ptr(radRot));
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
				if (ImGui::GetIO().MouseClicked) selection_mask = (1 << node_clicked);

				if (ImGui::GetIO().KeyCtrl) selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
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
					if (world->HasComponent<Frosty::ECS::CMotion>(m_SelectedEntity)) toggles[4] = true;
					if (world->HasComponent<Frosty::ECS::CController>(m_SelectedEntity)) toggles[5] = true;
					if (world->HasComponent<Frosty::ECS::CFollow>(m_SelectedEntity)) toggles[6] = true;
					if (world->HasComponent<Frosty::ECS::CLight>(m_SelectedEntity)) toggles[7] = true;
					if (world->HasComponent<Frosty::ECS::CCollision>(m_SelectedEntity)) toggles[8] = true;
					if (world->HasComponent<Frosty::ECS::CPlayerAttack>(m_SelectedEntity)) toggles[9] = true;
				}

				// Information
				ImGui::Text("Entity (%i)", m_SelectedEntity->Id);
				ImGui::SameLine();

				// Add Component stuff
				ImGui::SetCursorPos(ImVec2(150.0f, ImGui::GetCursorPosY()));
				if (ImGui::Button("Add component..")) ImGui::OpenPopup("component_popup");
				if (ImGui::BeginPopup("component_popup"))
				{
					if (ImGui::MenuItem("Mesh", "", &toggles[1]))
					{
						if (!world->HasComponent<Frosty::ECS::CMesh>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CMesh>(m_SelectedEntity, Frosty::AssetManager::GetMesh("3D"));
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
					if (ImGui::MenuItem("Motion", "", &toggles[4]))
					{
						if (!world->HasComponent<Frosty::ECS::CMotion>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CMotion>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CMotion>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Controller", "", &toggles[5]))
					{
						if (!world->HasComponent<Frosty::ECS::CController>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CController>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CController>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Follow", "", &toggles[6]))
					{
						if (!world->HasComponent<Frosty::ECS::CFollow>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CFollow>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CFollow>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Light", "", &toggles[7]))
					{
						if (!world->HasComponent<Frosty::ECS::CLight>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CLight>(m_SelectedEntity, Frosty::ECS::CLight::LightType::Point);
						else
							world->RemoveComponent<Frosty::ECS::CLight>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Collision", "", &toggles[8]))
					{
						if (!world->HasComponent<Frosty::ECS::CCollision>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CCollision>(m_SelectedEntity, Frosty::AssetManager::GetBoundingBox("Cube"));
						else
							world->RemoveComponent<Frosty::ECS::CCollision>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Player Attack", "", &toggles[9]))
					{
						if (!world->HasComponent<Frosty::ECS::CPlayerAttack>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CPlayerAttack>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CPlayerAttack>(m_SelectedEntity);
					}
					ImGui::EndPopup();
				}

				// List of components (Information)
				if (world->HasComponent<Frosty::ECS::CTransform>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Transform"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CTransform>(m_SelectedEntity);
						ImGui::BeginChild("CTransform", ImVec2(EDITOR_INSPECTOR_WIDTH, 85), true);
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
						ImGui::BeginChild("CMesh", ImVec2(EDITOR_INSPECTOR_WIDTH, 35), true);
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
						ImGui::BeginChild("Camera", ImVec2(EDITOR_INSPECTOR_WIDTH, 130), true);
						if (ImGui::Button("Target")) ImGui::OpenPopup("camera_target_select_popup");
						ImGui::SameLine();
						comp.Target ? ImGui::TextUnformatted(("Entity (" + std::to_string(comp.Target->EntityPtr->Id) + ")").c_str()) : ImGui::TextUnformatted("None");
						if (ImGui::BeginPopup("camera_target_select_popup"))
						{
							ImGui::Separator();
							for (auto& entity : *world->GetEntityManager())
							{
								if (entity != m_SelectedEntity)
								{
									if (ImGui::Selectable(std::string("Entity (" + std::to_string(entity->Id) + ")").c_str()))
									{
										comp.Target = &world->GetComponent<Frosty::ECS::CTransform>(entity);
									}
								}
							}
							ImGui::EndPopup();
						}
						ImGui::ColorEdit3("Background", glm::value_ptr(comp.Background));
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
						float compHeight = 0.0f;
						if (comp.UseShader->GetName() == "FlatColor") compHeight = 125.0f;
						else if (comp.UseShader->GetName() == "Texture2D") compHeight = 300.0f;
						ImGui::BeginChild("Material", ImVec2(EDITOR_INSPECTOR_WIDTH, compHeight), true);

						if (ImGui::Button("Shader")) ImGui::OpenPopup("shader_select_popup");
						ImGui::SameLine();
						ImGui::TextUnformatted(comp.UseShader->GetName().c_str());
						if (ImGui::BeginPopup("shader_select_popup"))
						{
							ImGui::Separator();
							for (auto& shader : Frosty::AssetManager::GetShaders())
							{
								if (ImGui::Selectable(shader.first.c_str()))
								{
									comp.UseShader = shader.second;
								}
							}
							ImGui::EndPopup();
						}
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.f);

						// Parameters
						if (comp.UseShader->GetName() == "FlatColor")
						{
							ImGui::ColorEdit4("Albedo", glm::value_ptr(comp.Albedo));
							ImGui::DragFloat("Specular Strength", &comp.SpecularStrength, 0.01f, 0.0f, 1.0f, "%.2f");
							ImGui::SliderInt("Shininess", &comp.Shininess, 2, 256);
						}
						if (comp.UseShader->GetName() == "Texture2D")
						{
							// Diffuse // 
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
							//uint32_t selDiffuseID = 0;
							//comp.DiffuseTexture ? selDiffuseID = comp.DiffuseTexture->GetRenderID() : selDiffuseID = Frosty::AssetManager::GetTexture2D("Checkerboard")->GetRenderID();
							ImGui::Image(comp.DiffuseTexture ? comp.DiffuseTexture->GetRenderID() : Frosty::AssetManager::GetTexture2D("Checkerboard")->GetRenderID(), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if (ImGui::IsItemClicked()) ImGui::OpenPopup("diffuse_texture_selector");
							if (ImGui::BeginPopupModal("diffuse_texture_selector", NULL))
							{
								size_t index = 0;
								ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
								uint32_t diffuseID = 0;

								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									ImGui::Image(texture.second->GetRenderID(), ImVec2(64, 64));
									if (ImGui::IsItemClicked())
									{
										if (texture.first == "Checkerboard")
										{
											comp.DiffuseTexture->Unbind();
											comp.DiffuseTexture.reset();
										}
										else
										{
											comp.DiffuseTexture = texture.second;
										}
									}
								}

								if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
								ImGui::EndPopup();
							}
							ImGui::SameLine();
							ImGui::Text("Diffuse");

							// Specular // 
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
							ImGui::Image(comp.SpecularTexture ? comp.SpecularTexture->GetRenderID() : Frosty::AssetManager::GetTexture2D("Checkerboard")->GetRenderID(), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if (ImGui::IsItemClicked()) ImGui::OpenPopup("specular_texture_selector");
							if (ImGui::BeginPopupModal("specular_texture_selector", NULL))
							{
								size_t index = 0;
								ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
								uint32_t glossID = 0;

								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									ImGui::Image(texture.second->GetRenderID(), ImVec2(64, 64));
									if (ImGui::IsItemClicked())
									{
										if (texture.first == "Checkerboard")
										{
											comp.SpecularTexture->Unbind();
											comp.SpecularTexture.reset();
										}
										else
										{
											comp.SpecularTexture = texture.second;
										}
									}
								}

								if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
								ImGui::EndPopup();
							}
							ImGui::SameLine();
							ImGui::Text("Specular");
							ImGui::SliderInt("Shininess", &comp.Shininess, 2, 256);

							// Normal // 
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.f);
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
							ImGui::Image(comp.NormalTexture ? comp.NormalTexture->GetRenderID() : Frosty::AssetManager::GetTexture2D("Checkerboard")->GetRenderID(), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if (ImGui::IsItemClicked()) ImGui::OpenPopup("normal_texture_selector");
							if (ImGui::BeginPopupModal("normal_texture_selector", NULL))
							{
								size_t index = 0;
								ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
								uint32_t normalID = 0;

								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									ImGui::Image(texture.second->GetRenderID(), ImVec2(64, 64));
									if (ImGui::IsItemClicked())
									{
										if (texture.first == "Checkerboard")
										{
											comp.NormalTexture->Unbind();
											comp.NormalTexture.reset();
										}
										else
										{
											comp.NormalTexture = texture.second;
										}
									}
								}

								if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
								ImGui::EndPopup();
							}
							ImGui::SameLine();
							ImGui::Text("Normal");
						}

						// Add more parameters like texture etc
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CMotion>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Motion"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CMotion>(m_SelectedEntity);
						ImGui::BeginChild("CMotion", ImVec2(EDITOR_INSPECTOR_WIDTH, 35), true);
						ImGui::InputFloat("Speed", &comp.Speed, 1.0f, 10.0f, 0);
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CController>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Controller"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CController>(m_SelectedEntity);
						ImGui::BeginChild("CController", ImVec2(EDITOR_INSPECTOR_WIDTH, 110), true);

						if (ImGui::Button(std::to_string(comp.MoveLeftKey).c_str(), ImVec2(100.0f, 0.0f)))
						{
							m_SelectedController = &comp;
							m_PreviousControllerHotkey = comp.MoveLeftKey;
						}
						ImGui::SameLine();
						ImGui::Text(m_PreviousControllerHotkey == comp.MoveLeftKey ? "Move Left (Press a key)" : "Move Left");

						if (ImGui::Button(std::to_string(comp.MoveForwardKey).c_str(), ImVec2(100.0f, 0.0f)))
						{
							m_SelectedController = &comp;
							m_PreviousControllerHotkey = comp.MoveForwardKey;
						}
						ImGui::SameLine();
						ImGui::Text(m_PreviousControllerHotkey == comp.MoveForwardKey ? "Move Forward (Press a key)" : "Move Forward");

						if (ImGui::Button(std::to_string(comp.MoveRightKey).c_str(), ImVec2(100.0f, 0.0f)))
						{
							m_SelectedController = &comp;
							m_PreviousControllerHotkey = comp.MoveRightKey;
						}
						ImGui::SameLine();
						ImGui::Text(m_PreviousControllerHotkey == comp.MoveRightKey ? "Move Right (Press a key)" : "Move Right");

						if (ImGui::Button(std::to_string(comp.MoveBackKey).c_str(), ImVec2(100.0f, 0.0f)))
						{
							m_SelectedController = &comp;
							m_PreviousControllerHotkey = comp.MoveBackKey;
						}
						ImGui::SameLine();
						ImGui::Text(m_PreviousControllerHotkey == comp.MoveBackKey ? "Move Back (Press a key)" : "Move Back");

						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CFollow>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Follow"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CFollow>(m_SelectedEntity);
						ImGui::BeginChild("CFollow", ImVec2(EDITOR_INSPECTOR_WIDTH, 60), true);
						if (ImGui::Button("Target")) ImGui::OpenPopup("follow_target_select_popup");
						ImGui::SameLine();
						comp.Target ? ImGui::TextUnformatted(("Entity (" + std::to_string(comp.Target->EntityPtr->Id) + ")").c_str()) : ImGui::TextUnformatted("None");
						if (ImGui::BeginPopup("follow_target_select_popup"))
						{
							ImGui::Separator();
							for (auto& entity : *world->GetEntityManager())
							{
								if (entity != m_SelectedEntity)
								{
									if (ImGui::Selectable(std::string("Entity (" + std::to_string(entity->Id) + ")").c_str()))
									{
										comp.Target = &world->GetComponent<Frosty::ECS::CTransform>(entity);
									}
								}
							}
							ImGui::EndPopup();
						}
						ImGui::InputFloat("Stop Distance", &comp.StopDistance, 1.0f, 10.0f, 0);
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CLight>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Light"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CLight>(m_SelectedEntity);
						ImGui::BeginChild("CLight", ImVec2(EDITOR_INSPECTOR_WIDTH, 105), true);
						if (ImGui::Button("Type")) ImGui::OpenPopup("light_type_popup");
						if (ImGui::BeginPopup("light_type_popup"))
						{
							if (ImGui::MenuItem("Point", "", comp.Type == Frosty::ECS::CLight::Point ? true : false))
							{
								comp.Type = Frosty::ECS::CLight::LightType::Point;
							}
							if (ImGui::MenuItem("Directional", "", comp.Type == Frosty::ECS::CLight::Directional ? true : false))
							{
								comp.Type = Frosty::ECS::CLight::LightType::Directional;
							}
							ImGui::EndPopup();
						}

						ImGui::ColorEdit4("Color", glm::value_ptr(comp.Color));
						ImGui::SliderFloat("Strength", &comp.Strength, 0.0f, 1.0f, "%.2f");
						if (comp.Type == Frosty::ECS::CLight::Point)
						{
							ImGui::InputFloat("Radius", &comp.Radius, 1.0f, 5.0f, 0);
						}
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CCollision>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Collision"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CCollision>(m_SelectedEntity);
						ImGui::BeginChild("CCollision", ImVec2(EDITOR_INSPECTOR_WIDTH, 35), true);
						if (ImGui::Button("Select bounding box.."))
							ImGui::OpenPopup("Bounding box selector");
						if (ImGui::BeginPopupModal("Bounding box selector", NULL, ImGuiWindowFlags_MenuBar))
						{
							size_t index = 0;
							ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

							for (auto& bb : Frosty::AssetManager::GetBoundingBoxes())
							{
								ImGui::TreeNodeEx((void*)(intptr_t)index, node_flags, "%s", bb.first.c_str());
								if (ImGui::IsItemClicked())
								{
									world->GetComponent<Frosty::ECS::CCollision>(m_SelectedEntity).BoundingBox = bb.second;
								}
							}

							if (ImGui::Button("Close"))
								ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
						}
						ImGui::EndChild();
					}
					if (world->HasComponent<Frosty::ECS::CPlayerAttack>(m_SelectedEntity))
					{
						if (ImGui::CollapsingHeader("Player Attack"))
						{
							auto& comp = world->GetComponent<Frosty::ECS::CPlayerAttack>(m_SelectedEntity);
							ImGui::BeginChild("CPlayerAttack", ImVec2(EDITOR_INSPECTOR_WIDTH, 105), true);
							ImGui::InputFloat("Damage", &comp.Damage, 1.0f, 10.0f, 0);
							ImGui::InputFloat("Reach", &comp.Reach, 1.0f, 10.0f, 0);
							ImGui::InputFloat("Width", &comp.Width, 1.0f, 10.0f, 0);
							ImGui::EndChild();
						}
					}
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
			ImGui::SetCursorPosX(m_App->GetWindow().GetWidth() - 250.0f);
			if (!m_App->GameIsRunning()) ImGui::Checkbox("Maximize on play", &m_MaximizeOnPlay);
			ImGui::SetCursorPosX(m_App->GetWindow().GetWidth() - 80.0f);
			if (m_App->GameIsRunning())
			{
				if (ImGui::Button("Stop", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
				{
					m_App->StopGame(m_MaximizeOnPlay);
					m_Active = true;
				}
			}
			else
			{
				if (ImGui::Button("Play", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
				{
					m_App->StartGame(m_MaximizeOnPlay);
					if (m_MaximizeOnPlay) m_Active = false;
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
		if (e.GetKeyCode() == FY_KEY_TAB)
		{
			if (m_App->GameIsRunning())
			{
				m_App->StopGame(m_MaximizeOnPlay);
				if (m_MaximizeOnPlay)
				{
					auto& camEntity = m_App->GetWorld()->GetSceneCamera();
					auto& gameCameraComp = m_App->GetWorld()->GetComponent<Frosty::ECS::CCamera>(camEntity);
					gameCameraComp.ProjectionMatrix = glm::perspective(glm::radians(gameCameraComp.FieldOfView), (float)(m_App->GetWindow().GetViewport().z / m_App->GetWindow().GetViewport().w), gameCameraComp.Near, gameCameraComp.Far);
					m_Active = true;
				}
			}
			else
			{
				m_App->StartGame(m_MaximizeOnPlay);
				if (m_MaximizeOnPlay)
				{
					auto& camEntity = m_App->GetWorld()->GetSceneCamera();
					auto& gameCameraComp = m_App->GetWorld()->GetComponent<Frosty::ECS::CCamera>(camEntity);
					gameCameraComp.ProjectionMatrix = glm::perspective(glm::radians(gameCameraComp.FieldOfView), (float)(m_App->GetWindow().GetViewport().z / m_App->GetWindow().GetViewport().w), gameCameraComp.Near, gameCameraComp.Far);
					m_Active = false;
				}
			}
			return true;
		}

		if (m_SelectedEntity && e.GetKeyCode() == FY_KEY_DELETE)
		{
			m_App->GetWorld()->RemoveEntity(m_SelectedEntity);
			m_SelectedEntity = nullptr;
		}
		else
		{
			if (m_SelectedController != nullptr)
			{
				if (m_SelectedController->MoveLeftKey == m_PreviousControllerHotkey)
				{
					m_SelectedController->MoveLeftKey = e.GetKeyCode();
				}
				if (m_SelectedController->MoveForwardKey == m_PreviousControllerHotkey)
				{
					m_SelectedController->MoveForwardKey = e.GetKeyCode();
				}
				if (m_SelectedController->MoveRightKey == m_PreviousControllerHotkey)
				{
					m_SelectedController->MoveRightKey = e.GetKeyCode();
				}
				if (m_SelectedController->MoveBackKey == m_PreviousControllerHotkey)
				{
					m_SelectedController->MoveBackKey = e.GetKeyCode();
				}
				m_PreviousControllerHotkey = -1;
				m_SelectedController = nullptr;
			}
		}

		return false;
	}
}
