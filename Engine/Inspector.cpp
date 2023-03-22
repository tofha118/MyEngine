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
	if (Hierarchy::GetInstance() == nullptr) { /* Hierarchy 검사 */
		return;
	}
	if (Hierarchy::GetInstance()->GetSelectObj().lock() == nullptr) { /* Hierarchy 오브젝트 검사 */
		return;
	}

	/* Imgui */
	shared_ptr<GameObject> go = Hierarchy::GetInstance()->GetSelectObj().lock();  //선택한 오브젝트

	static char strTemp[50] = {};
	::sprintf_s(strTemp, "%s", Utils::WstrToStr(go->GetName()));

	ImGui::Checkbox("##Check", &go->GetActive());  //체크박스 생성하고 go->GetActive()에 체크 박스 클릭 여부 체크
	ImGui::SameLine();   //같은 라인에
	static char _text[MAX_PATH / 2] = { 0 };
	::strcpy_s(_text, Utils::WstrToStr(go->GetName()).data());
	ImGui::InputText("##Nmae", _text, MAX_PATH / 2);  //텍스트 위에서 만든거 
	go->SetName(Utils::StrToWstr(_text));  

	shared_ptr<GameObject> pick = Hierarchy::GetInstance()->GetSelectObj().lock();  //pick에 선택된거 담아오고

	vector<shared_ptr<Component>> components = pick->GetAllComponent();  //모든 컴포넌트 정보 가져와서

	for (shared_ptr<Component>& component : components) {  //여기서 컴포넌트 업데이트
		component->ComponentUpdate();
	}

}

void Inspector::End()
{

}

