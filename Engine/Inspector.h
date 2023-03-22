#pragma once
#include "ImguiEditor.h"
#include "ImguiManager.h"

// ************************
//		 Inspector
// ************************
class Inspector : public ImguiEditor
{
public:
	//	Function ================================
	Inspector() {};
	virtual ~Inspector() {};

	void Init() override;
	void Show() override;
	void End() override;

	static Inspector* GetInstance() {

		if (m_pInstance == nullptr) {
			m_pInstance = GET_SINGLE(ImguiManager)->GetWindow<Inspector>();
		}
		return m_pInstance;

	}
	// ==========================================
private:
	//	Variable ================================
	inline static Inspector* m_pInstance = nullptr;
	// ==========================================
};

