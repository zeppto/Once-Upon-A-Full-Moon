#include <mcspch.hpp>
#include "InspectorLayer.hpp"
#include"Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

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
					if (world->HasComponent<Frosty::ECS::CPlayer>(m_SelectedEntity)) toggles[4] = true;
					if (world->HasComponent<Frosty::ECS::CFollow>(m_SelectedEntity)) toggles[5] = true;
					if (world->HasComponent<Frosty::ECS::CLight>(m_SelectedEntity)) toggles[6] = true;
					if (world->HasComponent<Frosty::ECS::CPhysics>(m_SelectedEntity)) toggles[7] = true;
					if (world->HasComponent<Frosty::ECS::CEnemy>(m_SelectedEntity)) toggles[8] = true;
					if (world->HasComponent<Frosty::ECS::CHealth>(m_SelectedEntity)) toggles[9] = true;
					if (world->HasComponent<Frosty::ECS::CInventory>(m_SelectedEntity)) toggles[10] = true;
					if (world->HasComponent<Frosty::ECS::CHealthBar>(m_SelectedEntity)) toggles[11] = true;
					if (world->HasComponent<Frosty::ECS::CParticleSystem>(m_SelectedEntity)) toggles[12] = true;
					if (world->HasComponent<Frosty::ECS::CBoss>(m_SelectedEntity)) toggles[13] = true;
					if (world->HasComponent<Frosty::ECS::CAnimController>(m_SelectedEntity)) toggles[17] = true;
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
							world->AddComponent<Frosty::ECS::CMesh>(m_SelectedEntity, Frosty::AssetManager::GetMesh("pCube1"));
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
					if (ImGui::MenuItem("Player", "", &toggles[4]))
					{
						if (!world->HasComponent<Frosty::ECS::CPlayer>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CPlayer>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CPlayer>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Follow", "", &toggles[5]))
					{
						if (!world->HasComponent<Frosty::ECS::CFollow>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CFollow>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CFollow>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Light", "", &toggles[6]))
					{
						if (!world->HasComponent<Frosty::ECS::CLight>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CLight>(m_SelectedEntity, Frosty::ECS::CLight::LightType::Point);
						else
							world->RemoveComponent<Frosty::ECS::CLight>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Physics", "", &toggles[7]))
					{
						if (!world->HasComponent<Frosty::ECS::CPhysics>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CPhysics>(m_SelectedEntity, Frosty::AssetManager::GetBoundingBox("pCube1"));
						else
							world->RemoveComponent<Frosty::ECS::CPhysics>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Player Attack", "", &toggles[8]))
					{
						if (!world->HasComponent<Frosty::ECS::CEnemy>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CEnemy>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CEnemy>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Health", "", &toggles[10]))
					{
						if (!world->HasComponent<Frosty::ECS::CHealth>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CHealth>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CHealth>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Inventory", "", &toggles[11]))
					{
						if (!world->HasComponent<Frosty::ECS::CInventory>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CInventory>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CInventory>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("HealthBar", "", &toggles[12]))
					{
						if (!world->HasComponent<Frosty::ECS::CHealthBar>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CHealthBar>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CHealthBar>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Particle System", "", &toggles[14])) {
						if (!world->HasComponent<Frosty::ECS::CParticleSystem>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CParticleSystem>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CParticleSystem>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Boss", "", &toggles[15])) {
						if (!world->HasComponent<Frosty::ECS::CBoss>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CBoss>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CBoss>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("GUI", "", &toggles[16])) {
						if (!world->HasComponent<Frosty::ECS::CGUI>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CGUI>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CGUI>(m_SelectedEntity);
					}
					if (ImGui::MenuItem("Anim Controller", "", &toggles[17])) {
						if (!world->HasComponent<Frosty::ECS::CAnimController>(m_SelectedEntity))
							world->AddComponent<Frosty::ECS::CAnimController>(m_SelectedEntity);
						else
							world->RemoveComponent<Frosty::ECS::CAnimController>(m_SelectedEntity);
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
						ImGui::DragFloat3("Position", glm::value_ptr(comp.Position), 0.1f, 0.0f, 0.0f, "%.2f");
						ImGui::DragFloat3("Rotation", glm::value_ptr(comp.Rotation), 0.1f, 0.0f, 0.0f, "%.2f");
						ImGui::DragFloat3("Scale", glm::value_ptr(comp.Scale), 0.1f, 0.0f, 0.0f, "%.2f");
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
						else if (comp.UseShader->GetName() == "UI") compHeight = 125.0f;
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
							ImGui::SetNextWindowSize(ImVec2(160, 370));
							if (ImGui::BeginPopupModal("diffuse_texture_selector", NULL))
							{
								size_t index = 0;
								ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
								uint32_t diffuseID = 0;
								int nrOfCols = 2;
								int col = 0;

								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									ImGui::SetCursorPos(ImVec2((col % nrOfCols) * 66.0f, ImGui::GetCursorPosY() - (col % nrOfCols) * 68.0f));
									col++;
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
							ImGui::SetNextWindowSize(ImVec2(160, 370));
							if (ImGui::BeginPopupModal("specular_texture_selector", NULL))
							{
								size_t index = 0;
								ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
								uint32_t glossID = 0;
								int nrOfCols = 2;
								int col = 0;

								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									ImGui::SetCursorPos(ImVec2((col % nrOfCols) * 66.0f, ImGui::GetCursorPosY() - (col % nrOfCols) * 68.0f));
									col++;
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
							ImGui::SetNextWindowSize(ImVec2(160, 370));
							if (ImGui::BeginPopupModal("normal_texture_selector", NULL))
							{
								size_t index = 0;
								ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
								uint32_t normalID = 0;
								int nrOfCols = 2;
								int col = 0;

								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									ImGui::SetCursorPos(ImVec2((col % nrOfCols) * 66.0f, ImGui::GetCursorPosY() - (col % nrOfCols) * 68.0f));
									col++;
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

						if (comp.UseShader->GetName() == "UI")
						{
							// Diffuse // 
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
							//uint32_t selDiffuseID = 0;
							//comp.DiffuseTexture ? selDiffuseID = comp.DiffuseTexture->GetRenderID() : selDiffuseID = Frosty::AssetManager::GetTexture2D("Checkerboard")->GetRenderID();
							ImGui::Image(comp.DiffuseTexture ? comp.DiffuseTexture->GetRenderID() : Frosty::AssetManager::GetTexture2D("Checkerboard")->GetRenderID(), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if (ImGui::IsItemClicked()) ImGui::OpenPopup("texture_selector");
							if (ImGui::BeginPopupModal("texture_selector", NULL))
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
							ImGui::Text("Texture");
						}

						if (comp.UseShader->GetName() == "Animation")
						{
							// Diffuse // 
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

							
				/*			world->AddComponent<Frosty::ECS::CAnimController>(m_SelectedEntity);

							
							Frosty::ECS::CAnimController& controlComp = world->GetComponent<Frosty::ECS::CAnimController>(m_SelectedEntity);
							ImGui::SliderFloat("AnimSpeed", &controlComp.animSpeed, 0.f, 100.f);*/

							//if (ImGui::CollapsingHeader("Animation Controller"))
							//{
							//	ImGui::SliderFloat("AnimSpeed", &controlComp.animSpeed, 0.f, 100.f);
							//	
							//	if (ImGui::Button("Pick Animation")) ImGui::OpenPopup("Animation menu");
							//	if (ImGui::BeginPopup("Animation menu"))
							//	{
							//		for (auto& it: *Frosty::AssetManager::GetAnimationMap())
							//		{
							//			if (ImGui::Button(it.first.c_str())) controlComp.currAnim = &it.second;
							//		}
							//		if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();

							//		ImGui::EndPopup();
							//	}
							//}
							//uint32_t selDiffuseID = 0;
							//comp.DiffuseTexture ? selDiffuseID = comp.DiffuseTexture->GetRenderID() : selDiffuseID = Frosty::AssetManager::GetTexture2D("Checkerboard")->GetRenderID();
							ImGui::Image(comp.DiffuseTexture ? comp.DiffuseTexture->GetRenderID() : Frosty::AssetManager::GetTexture2D("Checkerboard")->GetRenderID(), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if (ImGui::IsItemClicked()) ImGui::OpenPopup("diffuse_texture_selector");
							ImGui::SetNextWindowSize(ImVec2(160, 370));
							if (ImGui::BeginPopupModal("diffuse_texture_selector", NULL))
							{
								size_t index = 0;
								ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
								uint32_t diffuseID = 0;
								int nrOfCols = 2;
								int col = 0;

								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									ImGui::SetCursorPos(ImVec2((col % nrOfCols) * 66.0f, ImGui::GetCursorPosY() - (col % nrOfCols) * 68.0f));
									col++;
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
								int nrOfCols = 2;
								int col = 0;

								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									ImGui::SetCursorPos(ImVec2((col % nrOfCols) * 66.0f, ImGui::GetCursorPosY() - (col % nrOfCols) * 68.0f));
									col++;
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
								int nrOfCols = 2;
								int col = 0;

								for (auto& texture : Frosty::AssetManager::GetTextures2D())
								{
									ImGui::SetCursorPos(ImVec2((col % nrOfCols) * 66.0f, ImGui::GetCursorPosY() - (col % nrOfCols) * 68.0f));
									col++;
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
				if (world->HasComponent<Frosty::ECS::CPlayer>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Player"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CPlayer>(m_SelectedEntity);
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
							if (ImGui::MenuItem("Point", "", comp.Type == Frosty::ECS::CLight::LightType::Point ? true : false))
							{
								comp.Type = Frosty::ECS::CLight::LightType::Point;
							}
							if (ImGui::MenuItem("Directional", "", comp.Type == Frosty::ECS::CLight::LightType::Directional ? true : false))
							{
								comp.Type = Frosty::ECS::CLight::LightType::Directional;
							}
							ImGui::EndPopup();
						}

						ImGui::ColorEdit4("Color", glm::value_ptr(comp.Color));
						ImGui::SliderFloat("Strength", &comp.Strength, 0.0f, 1.0f, "%.2f");
						if (comp.Type == Frosty::ECS::CLight::LightType::Point)
						{
							ImGui::InputFloat("Radius", &comp.Radius, 1.0f, 5.0f, 0);
						}
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CPhysics>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Physics"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CPhysics>(m_SelectedEntity);
						ImGui::BeginChild("CPhysics", ImVec2(EDITOR_INSPECTOR_WIDTH, 70), true);
						ImGui::InputFloat("Speed", &comp.Speed, 1.0f, 10.0f, 0);
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
									world->GetComponent<Frosty::ECS::CPhysics>(m_SelectedEntity).BoundingBox = bb.second;
								}
							}

							if (ImGui::Button("Close"))
								ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
						}
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CEnemy>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Player Attack"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CEnemy>(m_SelectedEntity);
						ImGui::BeginChild("CPlayerAttack", ImVec2(EDITOR_INSPECTOR_WIDTH, 105), true);
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CHealth>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Health"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CHealth>(m_SelectedEntity);
						ImGui::BeginChild("CHealth", ImVec2(EDITOR_INSPECTOR_WIDTH, 70), true);
						ImGui::InputFloat("Max Health", &comp.MaxHealth, 1.0f, 10.0f, 0);
						ImGui::InputFloat("Current Health", &comp.CurrentHealth, 1.0f, 10.0f, 0);
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CInventory>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Inventory"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CInventory>(m_SelectedEntity);
						ImGui::BeginChild("CInventory", ImVec2(EDITOR_INSPECTOR_WIDTH, 150), true);
						ImGui::InputInt("Healing Potions", &comp.CurrentHealingPotions, 1, 10, 0);
						ImGui::InputInt("Increase Health Potions", &comp.CurrentIncreaseHPPotions, 1, 10, 0);
						ImGui::InputInt("Speed Potions", &comp.CurrentSpeedPotions, 1, 1, 0);
						ImGui::InputInt("Speed Boots", &comp.CurrentSpeedBoots, 1, 10, 0);
						ImGui::InputInt("Bait", &comp.CurrentBaitAmount, 1, 10, 0);
						ImGui::InputInt("Wolfsbane", &comp.CurrentWolfsbane, 1, 10, 0);
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CHealthBar>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("HealthBar"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CHealthBar>(m_SelectedEntity);
						ImGui::BeginChild("CHealthBar", ImVec2(EDITOR_INSPECTOR_WIDTH, 30), true);
						ImGui::Text("Health bar will now appear over entity");
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CParticleSystem>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Particle System"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CParticleSystem>(m_SelectedEntity);
						ImGui::BeginChild("CParticleSystem", ImVec2(EDITOR_INSPECTOR_WIDTH, 345), true);
						ImGui::Text("Active particles: %i", comp.ParticleCount);
						ImGui::Checkbox("Preview", &comp.Preview);
						ImGui::ColorEdit4("Color", glm::value_ptr(comp.ParticleSystemColor));
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
						ImGui::Image(comp.Texture ? comp.Texture->GetRenderID() : Frosty::AssetManager::GetTexture2D("Checkerboard")->GetRenderID(), ImVec2(64, 64));
						ImGui::PopStyleVar();
						if (ImGui::IsItemClicked()) ImGui::OpenPopup("particle_texture_selector");
						ImGui::SetNextWindowSize(ImVec2(160, 370));
						if (ImGui::BeginPopupModal("particle_texture_selector", NULL))
						{
							size_t index = 0;
							ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
							uint32_t diffuseID = 0;
							int nrOfCols = 2;
							int col = 0;

							for (auto& texture : Frosty::AssetManager::GetTextures2D())
							{
								ImGui::SetCursorPos(ImVec2((col % nrOfCols) * 66.0f, ImGui::GetCursorPosY() - (col % nrOfCols) * 68.0f));
								col++;
								ImGui::Image(texture.second->GetRenderID(), ImVec2(64, 64));
								if (ImGui::IsItemClicked())
								{
									if (texture.first == "Checkerboard")
									{
										comp.Texture->Unbind();
										comp.Texture.reset();
									}
									else
									{
										comp.Texture = texture.second;
									}
								}
							}

							if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
						}
						ImGui::SameLine();
						ImGui::Text("Texture");
						if (!Frosty::Application::Get().GameIsRunning())
						{
							ImGui::SliderInt("Particle count", (int*)&comp.MaxParticles, 1, comp.MAX_PARTICLE_COUNT);
						}
						ImGui::DragFloat3("Direction", glm::value_ptr(comp.ParticleSystemDirection), 0.1f, 0.0f, 0.0f, "%.2f");
						ImGui::InputFloat("Start size", &comp.StartParticleSize);
						ImGui::InputFloat("End size", &comp.EndParticleSize);
						ImGui::InputFloat("Emit rate", &comp.EmitRate);
						ImGui::InputFloat("Lifetime", &comp.MaxLifetime);
						ImGui::SliderFloat("Fade", &comp.FadeTreshold, 0.0f, comp.MaxLifetime);
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CBoss>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Boss"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CBoss>(m_SelectedEntity);
						ImGui::BeginChild("Boss", ImVec2(EDITOR_INSPECTOR_WIDTH, 45), true);
						ImGui::InputFloat("Distraction Time", &comp.DistractionTime, 1, 10, 0);
						ImGui::EndChild();
					}
				}
				if (world->HasComponent<Frosty::ECS::CGUI>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("GUI"))
					{
						auto& comp = world->GetComponent<Frosty::ECS::CGUI>(m_SelectedEntity);
						ImGui::BeginChild("CGUI", ImVec2(EDITOR_INSPECTOR_WIDTH, 45), true);
						ImGui::Text("Test text"); //TODO: Fill with info
						ImGui::EndChild();
					}
				}
				
				if (world->HasComponent<Frosty::ECS::CAnimController>(m_SelectedEntity))
				{
					if (ImGui::CollapsingHeader("Animation Controller"))
					{
						Frosty::ECS::CAnimController * comp = &world->GetComponent<Frosty::ECS::CAnimController>(m_SelectedEntity);
						ImGui::SliderFloat("AnimSpeed", &comp->animSpeed, 0.f, 50.f);

						if (ImGui::Button("Pick Animation")) ImGui::OpenPopup("Animation menu");
						if (ImGui::BeginPopup("Animation menu"))
						{
							for (auto& it : *Frosty::AssetManager::GetAnimationMap())
							{
								if (ImGui::Button(it.first.c_str())) comp->currAnim = &it.second;
							}
							if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
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
			//for fast creation of entitys
			if (ImGui::Button("Enemy", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(0));
			}
			ImGui::SameLine();
			if (ImGui::Button("Stone", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(1));
			}
			ImGui::SameLine();
			if (ImGui::Button("Tree", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(2));
			}
			ImGui::SameLine();
			if (ImGui::Button("Mushroom", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(3));
			}
			ImGui::SameLine();
			if (ImGui::Button("MushroomCirkel", ImVec2(110.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(4));
			}
			ImGui::SameLine();
			if (ImGui::Button("mushroomsAndStonesBig", ImVec2(170.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(5));
			}
			ImGui::SameLine();
			if (ImGui::Button("mushroomsAndStonesSmall", ImVec2(170.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(6));
			}
			ImGui::SameLine();
			if (ImGui::Button("mushrooms", ImVec2(90.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(7));
			}
			ImGui::SameLine();
			if (ImGui::Button("mushroomLong", ImVec2(100.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(8));
			}
			ImGui::SameLine();
			if (ImGui::Button("mushroomsFlat", ImVec2(100.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(9));
			}
			if (ImGui::Button("2stones", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(10));
			}
			ImGui::SameLine();
			if (ImGui::Button("3stones", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(11));
			}
			ImGui::SameLine();
			if (ImGui::Button("4stones", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(12));
			}
			ImGui::SameLine();
			if (ImGui::Button("treeBunch3", ImVec2(100.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(13));
			}
			ImGui::SameLine();
			if (ImGui::Button("treeBunch4", ImVec2(100.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(14));
			}
			ImGui::SameLine();
			if (ImGui::Button("treeBunch7", ImVec2(100.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(15));
			}
			ImGui::SameLine();
			if (ImGui::Button("treeBunchWall", ImVec2(100.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(16));
			}
			ImGui::SameLine();
			if (ImGui::Button("chest", ImVec2(80.0f, EDITOR_MAIN_MENU_BAR_HEIGHT)))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::CreatEntityEvent>(Frosty::CreatEntityEvent(17));
			}
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
				if (ImGui::MenuItem("Save Room", ""))
				{
					Frosty::EventBus::GetEventBus()->Publish<Frosty::SaveLevelEvent>(Frosty::SaveLevelEvent());
				}
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
			if (ImGui::BeginMenu("Level"))
			{
				if (ImGui::MenuItem("Create Deadend"))
				{
					Frosty::EventBus::GetEventBus()->Publish<Frosty::CreateLevelEvent>(Frosty::CreateLevelEvent(true, false, false, false));
				}
				if (ImGui::MenuItem("Create turning road"))
				{
					Frosty::EventBus::GetEventBus()->Publish<Frosty::CreateLevelEvent>(Frosty::CreateLevelEvent(true, false, true, false));
				}
				if (ImGui::MenuItem("Create straight road"))
				{
					Frosty::EventBus::GetEventBus()->Publish<Frosty::CreateLevelEvent>(Frosty::CreateLevelEvent(false, false, true, true));
				}
				if (ImGui::MenuItem("Create three way road"))
				{
					Frosty::EventBus::GetEventBus()->Publish<Frosty::CreateLevelEvent>(Frosty::CreateLevelEvent(true, false, true, true));
				}
				if (ImGui::MenuItem("Create crossroad"))
				{
					Frosty::EventBus::GetEventBus()->Publish<Frosty::CreateLevelEvent>(Frosty::CreateLevelEvent(true, true, true, true));
				}
				ImGui::InputText("level name", m_LevelName, IM_ARRAYSIZE(m_LevelName));
				if (ImGui::MenuItem("open existing level"))
				{
					Frosty::EventBus::GetEventBus()->Publish<Frosty::OpenLevelEvent>(Frosty::OpenLevelEvent(m_LevelName));
				}
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