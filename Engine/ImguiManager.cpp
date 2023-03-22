#include "pch.h"
#include "EnginePch.h"
#include "Engine.h"
#include "ImguiManager.h"
#include "imgui.h"
#include "ImguiEditor.h"
#include "Utils.h"
#include "Hierarchy.h"
#include "Inspector.h"

void ImguiManager::Init()
{
	/* GUI를 생성할 DESCRIPTOR HEAP 생성 */
	D3D12_DESCRIPTOR_HEAP_DESC descSrv = {};
	descSrv.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descSrv.NumDescriptors = 3;
	descSrv.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DEVICE->CreateDescriptorHeap(&descSrv, IID_PPV_ARGS(&desc_Heap));

	/* ImGui */
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(WINDOW.hwnd);
	ImGui_ImplDX12_Init(DEVICE.Get(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, GetDescHeap().Get(),
		GetDescHeap().Get()->GetCPUDescriptorHandleForHeapStart(),
		GetDescHeap().Get()->GetGPUDescriptorHandleForHeapStart());

	Hierarchy::GetInstance();
	Inspector::GetInstance();
}

void ImguiManager::PushEditor(ImguiEditor* editor)
{
	Vec_Editors.push_back(editor);

	editor->Init();
}

void ImguiManager::Render_Before()
{
	/* Imgui Render 사전작업 */
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	/*ImGui::Begin("ss");
	ImGui::End();*/
	
	for (ImguiEditor* editor : Vec_Editors) {
		ImGui::Begin(editor->title.c_str());
		editor->Show();
		ImGui::End();
	}

	ImGui::EndFrame();
}

void ImguiManager::Render()
{
	/* Imgui Render */
	ID3D12DescriptorHeap* descriptorHeaps[] = { GET_SINGLE(ImguiManager)->GetDescHeap().Get() };
	GRAPHICS_CMD_LIST->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	ImGui::Render();

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GEngine->GetGraphicsCmdQueue()->GetGraphicsCmdList().Get());
}