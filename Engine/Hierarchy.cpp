#include "pch.h"
#include "Hierarchy.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Utils.h"
#include "imgui.h"

void Hierarchy::Init() /* Initialize */
{
	Objects.reserve(1000);
}

void Hierarchy::Show()
{
	Objects.clear();

	vector<shared_ptr<GameObject>>& objects = GET_OBJECTS;
	for (int32 i = 0; i < objects.size(); ++i) {
		if (objects[i]->GetTransform()->GetParent().lock() != nullptr) {
			continue;
		}
		Hierarchy_Tree(objects[i]);
	}
}


void Hierarchy::End()
{
	Objects.clear();
}


void Hierarchy::Hierarchy_Tree(const shared_ptr<GameObject>& obj)
{
	uint32 node_flags = 0;
	name = Utils::WstrToStr(obj->GetName()) + "##" + std::to_string(obj->GetID());

	if (obj->GetTransform()->GetChilds().size() == 0) { /* 계층 구조의 자식이 없다면 */
		node_flags |= ImGuiTreeNodeFlags_Leaf;
		if (ImGui::TreeNodeEx(name.data(), node_flags)) {
			if (ImGui::IsItemActivated()) {
				Selected_Obj = obj;
			}
			ImGui::TreePop();
		}
	}
	else { /* 자식이 있다면 */
		if (ImGui::TreeNodeEx(name.data(), node_flags)) {
			if (ImGui::IsItemActivated()) {
				Selected_Obj = obj;
			}
			for (weak_ptr<Transform>& child : obj->GetTransform()->GetChilds()) {
				Hierarchy_Tree(child.lock()->GetGameObject());
			}

			ImGui::TreePop();
		}
	}
}
