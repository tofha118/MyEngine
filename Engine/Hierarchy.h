#pragma once
#include "GameObject.h"
#include "ImguiEditor.h"
#include "ImguiManager.h"
class Hierarchy : public ImguiEditor
{

public:

	Hierarchy() {}
	virtual ~Hierarchy() {}

	void Init() override;
	void Show() override;
	void End() override;

	void Hierarchy_Tree(const shared_ptr<GameObject>& obj);

	weak_ptr<GameObject> GetSelectObj() { return Selected_Obj; }  //오브젝트 선택

	static Hierarchy* GetInstance() {

		if (m_pInstance == nullptr) {
			m_pInstance = GET_SINGLE(ImguiManager)->GetWindow<Hierarchy>();
		}
		return m_pInstance;

	}

	// ==========================================
private:
	//	Variable ================================
	inline static Hierarchy* m_pInstance = nullptr;

	string name = "";

	vector<weak_ptr<GameObject>> Objects;  //오브젝트 담아주는데
	weak_ptr<GameObject> Selected_Obj;  //선택된 오브젝트
	// ==========================================

};

