#pragma once
#include "Object.h"

// [일감 기술서] 외주 인력들이 뭘 해야할지 기술
enum class SHADER_TYPE : uint8
{
	DEFERRED,
	FORWARD,
	LIGHTING,
	PARTICLE,
	COMPUTE,
	SHADOW,

};

enum class RASTERIZER_TYPE : uint8
{
	CULL_NONE,  //무시하지 않고 모두 연산
	CULL_FRONT,  //시계방향 무시
	CULL_BACK,  //반시계방향 무시가 일반적인 옵션
	WIREFRAME,
};

enum class DEPTH_STENCIL_TYPE : uint8 //깊이와 관련있음.: uint8
{
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	NO_DEPTH_TEST, // 깊이 테스트(X) + 깊이 기록(O)
	NO_DEPTH_TEST_NO_WRITE, // 깊이 테스트(X) + 깊이 기록(X)
	LESS_NO_WRITE, // 깊이 테스트(O) + 깊이 기록(X)
};

//혼합할건지 (결과물을 섞어주는 옵션)
enum class BLEND_TYPE : uint8
{
	DEFAULT,
	ALPHA_BLEND,  //투명도 블렌딩
	ONE_TO_ONE_BLEND,
	END,
};

struct ShaderInfo  //쉐이더 옵션
{
	SHADER_TYPE shaderType = SHADER_TYPE::FORWARD;
	RASTERIZER_TYPE rasterizerType = RASTERIZER_TYPE::CULL_BACK;
	DEPTH_STENCIL_TYPE depthStencilType = DEPTH_STENCIL_TYPE::LESS;
	BLEND_TYPE blendType = BLEND_TYPE::DEFAULT;
	//얘가 이제 진짜 topology (어케 그릴건지)
	D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//이건 살짝 그룹 느낌
//	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

};

//만들어야 할 쉐이더 설정
struct ShaderArg
{
	const string vs = "VS_Main";
	const string hs;
	const string ds;
	const string gs;
	const string ps = "PS_Main";
};

class Shader : public Object
{
public:
	Shader();
	virtual ~Shader();

	void CreateGraphicsShader(const wstring& path, ShaderInfo info = ShaderInfo(), ShaderArg arg = ShaderArg());
	void CreateComputeShader(const wstring& path, const string& name, const string& version);

	void Update();

	SHADER_TYPE GetShaderType() { return _info.shaderType; }

	static D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTopologyType(D3D_PRIMITIVE_TOPOLOGY topology);


private:
	void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
	void CreateVertexShader(const wstring& path, const string& name, const string& version);
	void CreateHullShader(const wstring& path, const string& name, const string& version);
	void CreateDomainShader(const wstring& path, const string& name, const string& version);
	void CreatePixelShader(const wstring& path, const string& name, const string& version);
	void CreateGeometryShader(const wstring& path, const string& name, const string& version);

private:
	ShaderInfo _info;
	ComPtr<ID3D12PipelineState>			_pipelineState;

	// GraphicsShader

	ComPtr<ID3DBlob>					_vsBlob;
	ComPtr<ID3DBlob>					_hsBlob;
	ComPtr<ID3DBlob>					_dsBlob;
	ComPtr<ID3DBlob>					_gsBlob;
	ComPtr<ID3DBlob>					_psBlob;
	ComPtr<ID3DBlob>					_errBlob;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC  _graphicsPipelineDesc = {};  //생성하는거 이름이 다름

	// ComputeShader
	ComPtr<ID3DBlob>					_csBlob;
	D3D12_COMPUTE_PIPELINE_STATE_DESC   _computePipelineDesc = {};  //생성하는거 이름 달라


};

