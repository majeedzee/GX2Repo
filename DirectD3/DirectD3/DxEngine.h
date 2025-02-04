#pragma once


#include "Defines.h"
#include "Algorithm.h"



class DxEngine
{
public:
	DxEngine();
	~DxEngine();

	bool Initialize();
	bool InitializeSwapChain(HWND window);
	bool InitializeViewports();
	bool InitializeVertexandIndexBuffers();
	bool InitializeVertexShaders();
	bool InitializePixelShaders();
	bool InitializeLayout();
	bool InitializeConstantBuffers();
	bool InitializeBlendState();
	bool InitializeRasterizerState();
	bool InitializeDepthStencilView();
	bool InitializeMatrixes();
	bool InitializeShaderResources();
	bool InitializeLight();

	void Draw();

	void TranslateCamera(UINT sub, float distance);
	void RotateCamera(UINT sub, float distance);
	void ShutDown();
	void SetValueX(float num) { valueX = num; }
	void SetValueY(float num) { valueY = num; }
	void SetChange() { change = !change; }
	void ChangeOnce() { once = !once; }
	void ChangeFull() { fullscreen = !fullscreen; }

private:
	template<typename DX>
	void ReleaseCOM(DX& item);

	

	struct Instance
	{
		XMFLOAT4X4 instance[60];
	};

	Instance ship;
	Light m_point;

	struct Model
	{
		float SV_WorldMatrix[4][4];
		float pos[3];
		float rotate[3];
	};

	struct SEND_TO_VRAM
	{
		float constantColor[4];
		float constantOffset[2];
		float padding[2];
	};

	vector<Model> m_model;
	SEND_TO_VRAM toShader;
	Obj toShaderWorld;
	float m_time = 0.0f;
	Matrix rotate;
	Matrix WorldSpaceCamera;
	Model m_camera;
	UINT numverts;
	UINT torchVerts;
	float valueX;
	float valueY;

	ID3D11Device*			device;

	ID3D11DeviceContext*	context;
	ID3D11DeviceContext*	defferedContext;
	ID3D11DeviceContext*	defferedContextObj1;
	ID3D11DeviceContext*	defferedContextObj2;
	
	ID3D11CommandList*		command;
	ID3D11CommandList*		commandObj1;
	ID3D11CommandList*		commandObj2;

	ID3D11RenderTargetView* renderTargetView;
	ID3D11RenderTargetView* m_renderTargetViewMap;

	IDXGISwapChain*			s_chain;

	DXGI_SWAP_CHAIN_DESC	swapChain;

	ID3D11Texture2D*		m_texture;
	ID3D11Texture2D*		pDepthStencil;
	ID3D11Texture2D*		m_renderTargetMap;

	D3D11_VIEWPORT			viewPort;
	D3D11_VIEWPORT			m_viewPort;

	ID3D11Buffer*			vram_Buffer;
	ID3D11Buffer*			constantBuffer;
	ID3D11Buffer*			deferredBuffer;
	ID3D11Buffer*			VertBuffer;
	ID3D11Buffer*			IndexBuffer;
	ID3D11Buffer*			m_VertBuffer;
	ID3D11Buffer*			m_torchVertBuffer;
	ID3D11Buffer*			m_IndexBuffer;
	ID3D11Buffer*			m_InstanceBuffer;
	ID3D11Buffer*			m_lightBuffer;
	ID3D11Buffer*			m_transBuffer;

	ID3D11Buffer*			plane_VertBuffer;
	ID3D11Buffer*			plane_IndexBuffer;

	ID3D11VertexShader*		VS_Shader;
	ID3D11VertexShader*		VS_InstanceShader;
	ID3D11VertexShader*		VS_SkyboxShader;
	ID3D11PixelShader*		PS_SkyboxShader;
	ID3D11PixelShader*		PS_PixelShader;
	ID3D11PixelShader*		PS_Shader;

	ID3D11InputLayout*		layout;
	ID3D11InputLayout*		m_layout;

	ID3D11DepthStencilView* pDSV;

	ID3D11SamplerState*		m_sampler;

	ID3D11ShaderResourceView*	m_shaderResource;
	ID3D11ShaderResourceView*	m_secondshaderResource;
	ID3D11ShaderResourceView*	m_shipResource;
	ID3D11ShaderResourceView*	m_secondshipResource;
	ID3D11ShaderResourceView*	m_shaderResourceMap;
	ID3D11ShaderResourceView*	m_transparentResource;

	ID3D11BlendState*			m_alphaEnabledBlendState;

	ID3D11RasterizerState*		m_RasterState;
	ID3D11RasterizerState*		m_FrontState;
	ID3D11RasterizerState*		m_BackState;

	bool fullscreen;
	bool once;
	bool FULL_SCREEN;
	bool change;
	XTime timer;

public:
	struct SIMPLE_VERTEX
	{
		float COORD[2];
	};
	struct VertexBuffer
	{
		float COORD[3];
		float Color[4];
		float UV[2];
		float norm[3];
	};

	void Resize();
	bool LoadObject(char* path, vector<VertexBuffer> *out_vertices, vector<unsigned int> *indicies);
};

