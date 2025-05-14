#include <pch.h>
#include "Inspector.h"
#include "GUI/GUI.h"

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
	if (ImGui::Checkbox("CameraToggled", &comp.IsCameraInUserControl()))
		EventDispatcher<Camera&>::FireEvent(RX_EVENT_CAMERA_USER_TOGGLED, comp);
}

void Inspector::UpdateCompModel(std::string const& strHandle, Model& comp)
{
	ImGui::Text("Hewwo");
}

void Inspector::UpdateCompDirectionalLight(std::string const& strHandle, DirectionalLight& comp)
{
	ImGui::InputFloat3(("Direction##" + strHandle).c_str(), glm::value_ptr(comp.GetDirection()));
}

void Inspector::UpdateCompMaterial(std::string const& strHandle, Material& comp)
{
	ImGui::ColorPicker3(("AmbientColor##" + strHandle).c_str(), glm::value_ptr(comp.GetAmbientColor()));
	ImGui::DragFloat(("AmbientIntensity##" + strHandle).c_str(), &comp.GetAmbientIntensity());
	ImGui::ColorPicker3(("DiffuseColor##" + strHandle).c_str(), glm::value_ptr(comp.GetDiffuseColor()));
	ImGui::DragFloat(("DiffuseIntensity##" + strHandle).c_str(), &comp.GetDiffuseIntensity());
	ImGui::ColorPicker3(("SpecularColor##" + strHandle).c_str(), glm::value_ptr(comp.GetSpecularColor()));
	ImGui::DragFloat(("SpecularIntensity##" + strHandle).c_str(), &comp.GetSpecularIntensity());
	ImGui::DragFloat(("Shininess##" + strHandle).c_str(), &comp.GetShininess());
}

void Inspector::UpdateCompCollider(std::string const& strHandle, Collider& comp)
{
	ImGui::Text("Choose BV type");
	entt::entity handle = comp.GetEntityHandle();

#define _RX_X(Klass) case BV::Klass:										\
	{																		\
		RX_ASSERT(EntityManager::HasComponent<Klass##BV>(handle));			\
		Klass##BV& comp = EntityManager::GetComponent<Klass##BV>(handle);	\
		UpdateComp##Klass##BV(strHandle, comp);								\
	} break;
	switch (comp.GetBVType())
	{
		RX_DO_ALL_BV_ENUM;
	default:
		break;
	}
#undef _RX_X
}

void Inspector::UpdateCompPointBV(std::string const& strHandle, PointBV& comp)
{
	ImGui::Text(__FUNCSIG__);
}

void Inspector::UpdateCompRayBV(std::string const& strHandle, RayBV& comp)
{
	ImGui::Text(__FUNCSIG__);
}

void Inspector::UpdateCompTriangleBV(std::string const& strHandle, TriangleBV& comp)
{
	ImGui::Text(__FUNCSIG__);
}

void Inspector::UpdateCompPlaneBV(std::string const& strHandle, PlaneBV& comp)
{
	ImGui::Text(__FUNCSIG__);
}

void Inspector::UpdateCompAABBBV(std::string const& strHandle, AABBBV& comp)
{
	ImGui::Text(__FUNCSIG__);
}

void Inspector::UpdateCompSphereBV(std::string const& strHandle, SphereBV& comp)
{
	ImGui::Text(__FUNCSIG__);
}
