#include "Settings.h"
#include "GUI/GUI.h"
#include "GSM/SceneManager.h"
#include "GLFWWindow/GLFWWindow.h"
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Systems/BVHSystem.h"

void Settings::UpdateImpl(float dt)
{
	ImGuiTreeNodeFlags scnFlags =
		ImGuiTreeNodeFlags_SpanFullWidth;

	if (ImGui::TreeNodeEx("Bounding Volumes", scnFlags | ImGuiTreeNodeFlags_DefaultOpen))
	{
		int bvhTreeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (ImGui::TreeNodeEx("General", bvhTreeNodeFlags)) 
		{
			{
				if (ImGui::Button("Recalculate BVH"))
					BVHSystem::BuildBVH();

				ImGui::SameLine();
				ImGui::Text("|");
				ImGui::SameLine();

				if (ImGui::Button("Recalculate ALL BVs"))
				{
					for (auto [handle, boundingVolume] : EntityManager::View<BoundingVolume>().each())
						boundingVolume.SetDirty();
				}
			}

			{
				int* pBV = reinterpret_cast<int*>(&BVHSystem::GetCurrentTreeType());
				ImGui::SeparatorText("BVH Method");
				BVHSystem::BVHType prev = BVHSystem::GetCurrentTreeType();
				bool isRadiod = false;
				isRadiod |= ImGui::RadioButton("TopDown", pBV, static_cast<int>(BVHSystem::BVHType::TopDown)); ImGui::SameLine();
				isRadiod |= ImGui::RadioButton("BottomUp", pBV, static_cast<int>(BVHSystem::BVHType::BottomUp)); ImGui::SameLine();
				isRadiod |= ImGui::RadioButton("OctTree", pBV, static_cast<int>(BVHSystem::BVHType::OctTree)); ImGui::SameLine();
				isRadiod |= ImGui::RadioButton("KDTree", pBV, static_cast<int>(BVHSystem::BVHType::KDTree));

				if (isRadiod && ((BVHSystem::BVHType)*pBV != prev))
					BVHSystem::BuildBVH();
			}

			{
				ImGui::SeparatorText("BVH Layers");
				for (int i = 0; i <= BVHSystem::GetBVHHeight(); ++i)
				{
					bool b = (BVHSystem::GetDrawLayers() >> i) & 0x1;
					if (ImGui::Checkbox((std::to_string(i) + "##togglebvhlayer").c_str(), &b))
					{
						if (b)
							BVHSystem::GetDrawLayers() |= 0x1 << i;
						else
							BVHSystem::GetDrawLayers() &= ~(0x1 << i);
					}
					if (i != BVHSystem::GetBVHHeight())
						ImGui::SameLine();
				}
			}

			//{
			//	int* pBV = reinterpret_cast<int*>(&BVHSystem::GetGlobalBVType());
			//	ImGui::SeparatorText("CurrentBV Option");
			//	BV prev = BVHSystem::GetGlobalBVType();
			//	bool isRadiod = false;
			//	isRadiod |= ImGui::RadioButton("AABB", pBV, static_cast<int>(BV::AABB)); ImGui::SameLine();
			//	isRadiod |= ImGui::RadioButton("OBB", pBV, static_cast<int>(BV::OBB)); ImGui::SameLine();
			//	isRadiod |= ImGui::RadioButton("Sphere", pBV, static_cast<int>(BV::Sphere));
			//
			//	if (isRadiod && ((BV)*pBV != prev))
			//		BVHSystem::EnforceUniformBVs();
			//}
			//
			//{
			//	int* algorithm = reinterpret_cast<int*>(&SphereBV::Algorithm);
			//	ImGui::SeparatorText("SphereBV Options");
			//	bool isRadiod = false;
			//	isRadiod |= ImGui::RadioButton("Ritter", algorithm, static_cast<int>(SphereBV::Algo::Ritter)); ImGui::SameLine();
			//	isRadiod |= ImGui::RadioButton("Larsson", algorithm, static_cast<int>(SphereBV::Algo::Larsson)); ImGui::SameLine();
			//	isRadiod |= ImGui::RadioButton("PCA", algorithm, static_cast<int>(SphereBV::Algo::PCA));
			//
			//	if (isRadiod && BVHSystem::GetGlobalBVType() == BV::Sphere) // Set ALL spheres to be dirty, to be updated with new algorithm
			//	{
			//		auto view = EntityManager::View<const BoundingVolume, SphereBV>();
			//		for (auto [handle, boundingVolume, bv] : view.each())
			//			bv.SetDirtyBV();
			//	}
			//}

			ImGui::TreePop();
		}


		switch (BVHSystem::GetCurrentTreeType())
		{
			case BVHSystem::BVHType::TopDown:
			{
				if (ImGui::TreeNodeEx("TopDown Settings", bvhTreeNodeFlags))
				{
					{
						int* pBV = reinterpret_cast<int*>(&BVHSystem::GetCurrentLeafCondition());
						ImGui::SeparatorText("Leaf Termination Condition");
						BVHSystem::LeafCondition prev = BVHSystem::GetCurrentLeafCondition();
						bool isRadiod = false;
						isRadiod |= ImGui::RadioButton("OneEntity", pBV, static_cast<int>(BVHSystem::LeafCondition::OneEntity)); ImGui::SameLine();
						isRadiod |= ImGui::RadioButton("TwoEntitiesMax", pBV, static_cast<int>(BVHSystem::LeafCondition::TwoEntitiesMax)); ImGui::SameLine();
						isRadiod |= ImGui::RadioButton("TreeHeightTwo", pBV, static_cast<int>(BVHSystem::LeafCondition::TreeHeightTwo));

						if (isRadiod && ((BVHSystem::LeafCondition)*pBV != prev))
							BVHSystem::BuildBVH();
					}

					{
						int* pBV = reinterpret_cast<int*>(&BVHSystem::GetCurrentSplitPointStrat());
						ImGui::SeparatorText("Split Point Strategy");
						BVHSystem::SplitPointStrat prev = BVHSystem::GetCurrentSplitPointStrat();
						bool isRadiod = false;
						isRadiod |= ImGui::RadioButton("MedianCenters", pBV, static_cast<int>(BVHSystem::SplitPointStrat::MedianCenters)); ImGui::SameLine();
						isRadiod |= ImGui::RadioButton("MedianExtents", pBV, static_cast<int>(BVHSystem::SplitPointStrat::MedianExtents)); ImGui::SameLine();
						isRadiod |= ImGui::RadioButton("KEvenSplits", pBV, static_cast<int>(BVHSystem::SplitPointStrat::KEvenSplits));

						if (isRadiod && ((BVHSystem::SplitPointStrat)*pBV != prev))
							BVHSystem::BuildBVH();
					}

					ImGui::TreePop();
				}
				break;
			}
			case BVHSystem::BVHType::BottomUp:
			{
				if (ImGui::TreeNodeEx("BottomUp Settings", bvhTreeNodeFlags))
				{
					{
						int* pBV = reinterpret_cast<int*>(&BVHSystem::GetCurrentMergeStrat());
						ImGui::SeparatorText("Node Merging Strategy");
						BVHSystem::MergeStrat prev = BVHSystem::GetCurrentMergeStrat();
						bool isRadiod = false;
						isRadiod |= ImGui::RadioButton("NearestNeighbour", pBV, static_cast<int>(BVHSystem::MergeStrat::NearestNeighbour)); ImGui::SameLine();
						isRadiod |= ImGui::RadioButton("MinVolume", pBV, static_cast<int>(BVHSystem::MergeStrat::MinVolume)); ImGui::SameLine();
						isRadiod |= ImGui::RadioButton("MinSurfaceArea", pBV, static_cast<int>(BVHSystem::MergeStrat::MinSurfaceArea));

						if (isRadiod && ((BVHSystem::MergeStrat)*pBV != prev))
							BVHSystem::BuildBVH();
					}

					ImGui::TreePop();
				}
				break;
			}
			case BVHSystem::BVHType::OctTree:
			{
				if (ImGui::TreeNodeEx("OctTree Settings", bvhTreeNodeFlags))
				{
					{
						int& v = BVHSystem::GetObjsPerNode();

						float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

						ImGui::Text("Objs Per Node");
						ImGui::SameLine(0.0f, spacing);

						ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
						if (ImGui::ArrowButton("##objpernode_left", ImGuiDir_Left)) { if (v <= 1) v = 1; else --v; BVHSystem::BuildBVH(); }
						ImGui::PopItemFlag();

						ImGui::SameLine(0.0f, spacing);
						ImGui::Text("%d", v);
						ImGui::SameLine(0.0f, spacing);

						ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
						if (ImGui::ArrowButton("##objpernode_right", ImGuiDir_Right)) { ++v; BVHSystem::BuildBVH(); }
						ImGui::PopItemFlag();
					}
					{
						int& v = BVHSystem::GetMaxHeight();

						float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

						ImGui::Text("Max Tree Height");
						ImGui::SameLine(0.0f, spacing);

						ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
						if (ImGui::ArrowButton("##treeheight_left", ImGuiDir_Left)) { if (v <= 0) v = 0; else --v; BVHSystem::BuildBVH(); }
						ImGui::PopItemFlag();

						ImGui::SameLine(0.0f, spacing);
						ImGui::Text("%d", v);
						ImGui::SameLine(0.0f, spacing);

						ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
						if (ImGui::ArrowButton("##treeheight_right", ImGuiDir_Right)) { if (v >= 8) v = 8; else ++v; BVHSystem::BuildBVH(); }
						ImGui::PopItemFlag();
					}

					{
						int* pCurrent = reinterpret_cast<int*>(&BVHSystem::GetCurrentStraddleCondition());
						ImGui::SeparatorText("Object Node Straddle Condition");
						BVHSystem::StraddleCondition prev = BVHSystem::GetCurrentStraddleCondition();
						bool isRadiod = false;
						//isRadiod |= ImGui::RadioButton("ObjectCenter", pCurrent, static_cast<int>(BVHSystem::StraddleCondition::ObjectCenter)); ImGui::SameLine();
						isRadiod |= ImGui::RadioButton("AllOverlapping", pCurrent, static_cast<int>(BVHSystem::StraddleCondition::AllOverlapping)); ImGui::SameLine();
						isRadiod |= ImGui::RadioButton("CurrentLevel", pCurrent, static_cast<int>(BVHSystem::StraddleCondition::CurrentLevel)); //ImGui::SameLine();
						//isRadiod |= ImGui::RadioButton("SplitObject", pCurrent, static_cast<int>(BVHSystem::StraddleCondition::SplitObject));

						if (isRadiod && ((BVHSystem::StraddleCondition)*pCurrent != prev))
							BVHSystem::BuildBVH();
					}

					ImGui::TreePop();
				}
				break;
			}
			case BVHSystem::BVHType::KDTree:
			{
				if (ImGui::TreeNodeEx("KDTree Settings", bvhTreeNodeFlags))
				{
					{
						int& v = BVHSystem::GetObjsPerNode();

						float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

						ImGui::Text("Objs Per Node");
						ImGui::SameLine(0.0f, spacing);

						ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
						if (ImGui::ArrowButton("##objpernode_left", ImGuiDir_Left)) { if (v <= 1) v = 1; else --v; BVHSystem::BuildBVH(); }
						ImGui::PopItemFlag();

						ImGui::SameLine(0.0f, spacing);
						ImGui::Text("%d", v);
						ImGui::SameLine(0.0f, spacing);

						ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
						if (ImGui::ArrowButton("##objpernode_right", ImGuiDir_Right)) { ++v; BVHSystem::BuildBVH(); }
						ImGui::PopItemFlag();
					}
					{
						int& v = BVHSystem::GetMaxHeight();

						float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

						ImGui::Text("Max Tree Height");
						ImGui::SameLine(0.0f, spacing);

						ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
						if (ImGui::ArrowButton("##treeheight_left", ImGuiDir_Left)) { if (v <= 0) v = 0; else --v; BVHSystem::BuildBVH(); }
						ImGui::PopItemFlag();

						ImGui::SameLine(0.0f, spacing);
						ImGui::Text("%d", v);
						ImGui::SameLine(0.0f, spacing);

						ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
						if (ImGui::ArrowButton("##treeheight_right", ImGuiDir_Right)) { if (v >= 8) v = 8; else ++v; BVHSystem::BuildBVH(); }
						ImGui::PopItemFlag();
					}

					{
						int* pBV = reinterpret_cast<int*>(&BVHSystem::GetCurrentSplitPointStrat());
						ImGui::SeparatorText("Split Point Strategy");
						BVHSystem::SplitPointStrat prev = BVHSystem::GetCurrentSplitPointStrat();
						bool isRadiod = false;
						isRadiod |= ImGui::RadioButton("MedianCenters", pBV, static_cast<int>(BVHSystem::SplitPointStrat::MedianCenters)); ImGui::SameLine();
						isRadiod |= ImGui::RadioButton("MedianExtents", pBV, static_cast<int>(BVHSystem::SplitPointStrat::MedianExtents)); //ImGui::SameLine();
						//isRadiod |= ImGui::RadioButton("KEvenSplits", pBV, static_cast<int>(BVHSystem::SplitPointStrat::KEvenSplits));

						if (isRadiod && ((BVHSystem::SplitPointStrat)*pBV != prev))
							BVHSystem::BuildBVH();
					}

					ImGui::TreePop();
				}
				break;
			}
			default: break;
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("GSM", scnFlags | ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Restart Scene"))
			SceneManager::Restart();

		auto const& scenes = SceneManager::GetScenes();
		if (ImGui::BeginCombo("Select Scene", scenes[SceneManager::GetCurrScene()]->GetSceneName().c_str()))
		{
			for (size_t i = 0; i < scenes.size(); ++i)
			{
				auto const& pScene = scenes[i];
				if (!pScene)
					continue;

				bool selected = i == SceneManager::GetCurrScene();
				if (ImGui::Selectable(pScene->GetSceneName().c_str(), &selected))
				{
					SceneManager::SetNextScene(i);
				}
			}
			ImGui::EndCombo();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Graphics", scnFlags | ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SeparatorText("Toggle Passes");
		for (auto& pass : RenderSystem::GetRenderPasses())
		{
			ImGui::Checkbox(pass->GetDisplayName().c_str(), &pass->IsEnabled());
		}

		ImGui::SeparatorText("Global Lighting");
		ImGui::ColorEdit3("Ambience", glm::value_ptr(RenderSystem::GetGlobalIllumination()));
		ImGui::DragFloat("Ambiance Factor", &RenderSystem::GetGlobalIllumination().w, 0.05f, 0.f, 1.f, "%.2f");

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Window", scnFlags))
	{
		static int s_FPSIndex = 0; // Defaults at 30
		static std::array<const char*, 6> s_FPSs{ "30", "60", "120", "144", "240", "No Lim." };
		static std::array<uint32_t, 6> s_FPSsNum{ 30, 60, 120, 144, 240, std::numeric_limits<uint32_t>::infinity() };
		if (ImGui::SliderInt("FPS", &s_FPSIndex, 0, static_cast<int>(s_FPSs.size() - 1), s_FPSs[s_FPSIndex]))
			GLFWWindow::SetTargetFPS(s_FPSsNum[s_FPSIndex]);

		ImGui::BeginDisabled();
		if (ImGui::Checkbox("VSync", &GLFWWindow::IsVSync()))
			GLFWWindow::SetIsVSync(GLFWWindow::IsVSync());
		ImGui::EndDisabled();

		ImGui::TreePop();
	}
}
