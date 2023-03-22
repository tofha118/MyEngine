#pragma once
#include "ImguiEditor.h"
#include "Utils.h"

// ************************
//		ImguiManager
// ************************
class ImguiManager
{
public:
	//	Function ================================
	DECLARE_SINGLE(ImguiManager);
public:
	void Init();

public:
	ComPtr<ID3D12DescriptorHeap> GetDescHeap() { return desc_Heap; }

	void PushEditor(ImguiEditor* editor);

	void Render_Before();
	void Render();

	template<typename T>
	T* GetWindow();
	// ==========================================
private:
	// Variable =================================
	ComPtr<ID3D12DescriptorHeap> desc_Heap;

	vector<ImguiEditor*>				Vec_Editors;
	// ==========================================
};

template<typename T>
T* ImguiManager::GetWindow()
{
	if (std::is_same_v<T, ImguiEditor>) {
		assert("error");
		return nullptr;
	}

	T* window = new T;
	window->title = Utils::Split(typeid(T).name(), ' ')[1];
	window->Init();

	PushEditor(window);  //매니저에 만들어진 에디터 넣기
	return window;
}