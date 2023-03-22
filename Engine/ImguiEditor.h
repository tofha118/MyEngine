#pragma once
#include "imgui.h"

// ************************
//		 ImguiEditor
// ************************
class ImguiEditor
{
protected:
	//	Function ================================
	ImguiEditor() {}
	virtual ~ImguiEditor() {}

public:
	friend class ImguiManager;

public:
	virtual void Init() = 0;
	virtual void Show() = 0;
	virtual void End() {};

	void ShowAllWindow() { ImGui::Begin(title.c_str()); Show(); ImGui::End(); }
	// ==========================================
	//	Variable ================================
	string title = typeid(ImguiEditor).name();
	// ==========================================
};