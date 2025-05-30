#include <pch.h>
#include "Inspector.h"
#include "GUI/GUI.h"
#include "ECS/Systems/RenderSystem.h"


void Draggy(std::string name, std::string strHandle, float* pp, int size, std::function<void()> func = nullptr)
{
	std::string label = name + "##" + strHandle;
	float step = 0.001f;
	switch (size)
	{
	case 1:
		if (ImGui::DragFloat(label.c_str(), pp, step) && func) func();
		break;
	case 2:
		if (ImGui::DragFloat2(label.c_str(), pp, step) && func) func();
		break;
	case 3:
		if (ImGui::DragFloat3(label.c_str(), pp, step) && func) func();
		break;
	case 4:
		if (ImGui::DragFloat4(label.c_str(), pp, step) && func) func();
		break;
	default:
		break;
	}
}

void PositionDrag(std::string const& strHandle, glm::vec3& pos)
{
	Draggy("Position", strHandle, glm::value_ptr(pos), 3);
}

void EulerOrientationDrag(std::string const& strHandle, glm::vec3& pos)
{
	Draggy("Euler", strHandle, glm::value_ptr(pos), 3);
}

void Inspector::UpdateImpl(float dt)
{
	entt::entity selectedEntityHandle = GUI::GetSelectedEntity();
	if (!EntityManager::HasEntity(selectedEntityHandle))
		return;

	std::string strHandle = std::to_string((uint32_t)selectedEntityHandle);
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
	
#define _RX_X(Klass)															\
	if (EntityManager::HasComponent<Klass>(selectedEntityHandle))				\
	{ 																			\
		ImGui::Separator(); 													\
		Klass& comp = EntityManager::GetComponent<Klass>(selectedEntityHandle);	\
		if (ImGui::TreeNodeEx((#Klass"##" + strHandle).c_str(), flags))			\
		{																		\
			UpdateComp##Klass(strHandle, comp);									\
			ImGui::TreePop();													\
		}																		\
	}

	RX_DO_MAIN_COMPONENTS;
#undef _RX_X
}

void Inspector::UpdateCompMetadata(std::string const& strHandle, Metadata& comp)
{

}

void Inspector::UpdateCompXform(std::string const& strHandle, Xform& comp)
{
	ImGui::DragFloat3(("Pos##xform" + strHandle).c_str(), glm::value_ptr(comp.GetTranslate()), 0.01f);
	ImGui::DragFloat3(("Scl##xform" + strHandle).c_str(), glm::value_ptr(comp.GetScale()), 0.01f);
	ImGui::DragFloat3(("Rot##xform" + strHandle).c_str(), glm::value_ptr(comp.GetEulerOrientation()), 0.01f);
}

void Inspector::UpdateCompCamera(std::string const& strHandle, Camera& comp)
{
	glm::vec3 camDir = comp.GetDirection();

	ImGui::BeginDisabled();
	ImGui::InputFloat3(("Direction Faced" + strHandle).c_str(), glm::value_ptr(camDir));
	ImGui::EndDisabled();

	if (comp.IsOrtho())
		ImGui::DragFloat("Ortho Zoom", &comp.GetOrthoSize(), 0.01f, 0.f, 10.f);

	//ImGui::Text("Cam [Pos]|X:% -4.1f |Y:% -4.1f |Z:% -4.1f", camPos.x, camPos.y, camPos.z);
	//ImGui::Text("    [Dir]|X:% -4.1f |Y:% -4.1f |Z:% -4.1f", camFace.x, camFace.y, camFace.z);
	ImGui::BeginDisabled(RenderSystem::GetActiveCamera() != comp.GetEntityHandle());
	if (ImGui::Checkbox("CameraToggled", &comp.IsCameraInUserControl()))
		EventDispatcher<Camera&>::FireEvent(RX_EVENT_CAMERA_USER_TOGGLED, comp);
	ImGui::EndDisabled();
}

void Inspector::UpdateCompModel(std::string const& strHandle, Model& comp)
{
	auto const& objs = RenderSystem::GetObjekts();
	std::string preview{};
	if (comp.GetMesh() != RX_INVALID_ID)
	{
		auto const& obj = objs.at(comp.GetMesh());
		preview = obj.GetName();
	}

	int i = 0;
	if (ImGui::BeginCombo("Choose Model", preview.c_str()))
	{
		for (auto const& obj : objs)
		{
			std::string name = obj.second.GetName() + "##" + std::to_string(i);
			bool isSelected = obj.first == comp.GetMesh();
			ImGuiSelectableFlags flags{};
			if (isSelected)
				flags |= ImGuiSelectableFlags_Highlight;

			if (ImGui::Selectable(name.c_str(), &isSelected, flags))
			{
				comp.SetMesh(obj.first);
			}

			++i;
		}
		ImGui::EndCombo();
	}
}

void Inspector::UpdateCompDirectionalLight(std::string const& strHandle, DirectionalLight& comp)
{
	ImGui::InputFloat3(("Direction##" + strHandle).c_str(), glm::value_ptr(comp.GetDirection()));
}

void Inspector::UpdateCompMaterial(std::string const& strHandle, Material& comp)
{
	ImGui::ColorPicker3(("Object Color##" + strHandle).c_str(), glm::value_ptr(comp.GetDiffuseColor()));
	ImGui::DragFloat(("Intensity##" + strHandle).c_str(), &comp.GetDiffuseIntensity(), 0.05f, 0.f, 1.f, "%.2f");
	//ImGui::ColorPicker3(("DiffuseColor##" + strHandle).c_str(), glm::value_ptr(comp.GetDiffuseColor()));
	//ImGui::DragFloat(("DiffuseIntensity##" + strHandle).c_str(), &comp.GetDiffuseIntensity());
	//ImGui::ColorPicker3(("SpecularColor##" + strHandle).c_str(), glm::value_ptr(comp.GetSpecularColor()));
	//ImGui::DragFloat(("SpecularIntensity##" + strHandle).c_str(), &comp.GetSpecularIntensity());
	//ImGui::DragFloat(("Shininess##" + strHandle).c_str(), &comp.GetShininess());
}

void Inspector::UpdateCompCollider(std::string const& strHandle, Collider& comp)
{
	ImGui::Text("Choose Primitive");
	entt::entity handle = comp.GetEntityHandle();

#define _RX_X(Klass) #Klass,
	std::vector<std::string> bvOptions{
		RX_DO_ALL_PRIMITIVE_ENUM_AND_NIL
	};
#undef _RX_X

	size_t currIndex = (size_t)comp.GetPrimitiveType();
	ImGuiComboFlags comboFlags = 0;
	if (ImGui::BeginCombo("Primitive", bvOptions[currIndex].c_str(), comboFlags))
	{
		for (size_t i = 0; i < bvOptions.size(); ++i)
		{
			auto const& c = bvOptions[i];
			bool currSelected = currIndex == i;
			if (ImGui::Selectable(c.c_str(), &currSelected))
			{
				comp.SetPrimitiveType((Primitive)i);
			}
		}
		ImGui::EndCombo();
	}

#define _RX_X(Klass) case Primitive::Klass:													\
	{																						\
		RX_ASSERT(EntityManager::HasComponent<Klass##Primitive>(handle));					\
		if (ImGui::TreeNodeEx(#Klass" Primitive")) {										\
			Klass##Primitive& comp = EntityManager::GetComponent<Klass##Primitive>(handle);	\
			PositionDrag(strHandle, comp.GetOffset());										\
			UpdateComp##Klass##Primitive(strHandle, comp);									\
			ImGui::TreePop();																\
		}																					\
	} break;
	switch (comp.GetPrimitiveType())
	{
		RX_DO_ALL_PRIMITIVE_ENUM;
	default:
		break;
	}
#undef _RX_X
}

void Inspector::UpdateCompPointPrimitive(std::string const& strHandle, PointPrimitive& comp)
{
}

void Inspector::UpdateCompRayPrimitive(std::string const& strHandle, RayPrimitive& comp)
{
	EulerOrientationDrag(strHandle, comp.GetOrientation());
	
	ImGui::Separator();
	ImGui::BeginDisabled();
	glm::vec3 dir = comp.GetDirection();
	Draggy("Dir", strHandle, glm::value_ptr(dir), 3);
	ImGui::EndDisabled();
}

void Inspector::UpdateCompTrianglePrimitive(std::string const& strHandle, TrianglePrimitive& comp)
{
	Draggy("P0", strHandle, glm::value_ptr(comp.GetP0()), 3, [&]() { comp.UpdateCentroid(); });
	Draggy("P1", strHandle, glm::value_ptr(comp.GetP1()), 3, [&]() { comp.UpdateCentroid(); });
	Draggy("P2", strHandle, glm::value_ptr(comp.GetP2()), 3, [&]() { comp.UpdateCentroid(); });

	ImGui::Separator();
	ImGui::BeginDisabled();
	glm::vec3 norm = comp.GetNormal();
	Draggy("Norm", strHandle, glm::value_ptr(norm), 3);
	ImGui::EndDisabled();
}

void Inspector::UpdateCompPlanePrimitive(std::string const& strHandle, PlanePrimitive& comp)
{
	EulerOrientationDrag(strHandle, comp.GetOrientation());

	ImGui::Separator();
	ImGui::BeginDisabled();
	glm::vec3 norm = comp.GetNormal();
	Draggy("Norm", strHandle, glm::value_ptr(norm), 3);
	float d = comp.GetD();
	Draggy("D", strHandle, &d, 1);
	ImGui::EndDisabled();
}

void Inspector::UpdateCompAABBPrimitive(std::string const& strHandle, AABBPrimitive& comp)
{
	Draggy("Half-Extents", strHandle, glm::value_ptr(comp.GetHalfExtents()), 3);

	ImGui::Separator();
	ImGui::BeginDisabled();
	glm::vec3 min = comp.GetMinPoint();
	glm::vec3 max = comp.GetMaxPoint();
	Draggy("Min", strHandle, glm::value_ptr(min), 3);
	Draggy("Max", strHandle, glm::value_ptr(max), 4);
	ImGui::EndDisabled();
}

void Inspector::UpdateCompSpherePrimitive(std::string const& strHandle, SpherePrimitive& comp)
{
	Draggy("Radius", strHandle, &comp.GetRadius(), 1);
}

void Inspector::UpdateCompBoundingVolume(std::string const& strHandle, BoundingVolume& comp)
{
	switch (comp.GetBVType())
	{
	case BV::Frustum:
		ImGui::Text("Frusttum");
		break;
	case BV::AABB:
	{
		AABBBV& bv = EntityManager::GetComponent<AABBBV>(comp.GetEntityHandle());
		ImGui::BeginDisabled();
		Draggy("Offset", strHandle, &bv.GetOffset()[0], 3);
		Draggy("Half Extent", strHandle, &bv.GetHalfExtents()[0], 3);
		ImGui::EndDisabled();
		break;
	}
	case BV::OBB:
		ImGui::Text("OBB");
		break;
	case BV::Sphere:
	{
		SphereBV& bv = EntityManager::GetComponent<SphereBV>(comp.GetEntityHandle());
		Draggy("Offset", strHandle, &bv.GetOffset()[0], 3);
		Draggy("Radius", strHandle, &bv.GetRadius(), 1);
		break;
	}
	default:
		break;
	}
}