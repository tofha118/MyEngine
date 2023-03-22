#include "pch.h"
#include "Inspector.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Utils.h"
#include "Hierarchy.h"
#include "imgui.h"

void Inspector::Init() /* Initialize */
{
	title = "Inspector";
}

void Inspector::Show()
{
	if (Hierarchy::GetInstance() == nullptr) { /* Hierarchy �˻� */
		return;
	}
	if (Hierarchy::GetInstance()->GetSelectObj().lock() == nullptr) { /* Hierarchy ������Ʈ �˻� */
		return;
	}

	/* Imgui */
	shared_ptr<GameObject> go = Hierarchy::GetInstance()->GetSelectObj().lock();  //������ ������Ʈ

	static char strTemp[50] = {};
	::sprintf_s(strTemp, "%s", Utils::WstrToStr(go->GetName()));

	ImGui::Checkbox("##Check", &go->GetActive());  //üũ�ڽ� �����ϰ� go->GetActive()�� üũ �ڽ� Ŭ�� ���� üũ
	ImGui::SameLine();   //���� ���ο�
	static char _text[MAX_PATH / 2] = { 0 };
	::strcpy_s(_text, Utils::WstrToStr(go->GetName()).data());
	ImGui::InputText("##Nmae", _text, MAX_PATH / 2);  //�ؽ�Ʈ ������ ����� 
	go->SetName(Utils::StrToWstr(_text));  

	shared_ptr<GameObject> pick = Hierarchy::GetInstance()->GetSelectObj().lock();  //pick�� ���õȰ� ��ƿ���

	vector<shared_ptr<Component>> components = pick->GetAllComponent();  //��� ������Ʈ ���� �����ͼ�

	for (shared_ptr<Component>& component : components) {  //���⼭ ������Ʈ ������Ʈ
		component->ComponentUpdate();
	}

}

void Inspector::End()
{

}

