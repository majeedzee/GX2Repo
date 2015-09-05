// D3D LAB 1a "Line Land".
// Author: L.Norri CD DRX, FullSail University

// Introduction:
// Welcome to the first lab of the Direct3D Graphics Programming class.
// This is the ONLY guided lab in this course! 
// Future labs will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this lab and start from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <iostream>
#include <ctime>
#include "XTime.h"
using namespace std;

// BEGIN PART 1
// TODO: PART 1 STEP 1a
#include <Windows.h>
#include <d3d11.h>
#include <WinUser.h>

#pragma comment(lib, "d3d11.lib")

// TODO: PART 1 STEP 1b
//#include <DirectXMath.h>
#include <math.h>
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"
#include "Algorithm.h"
#include "Cube.h"
#include "crosshatch.h"
//using namespace DirectX;

// TODO: PART 2 STEP 6



template<typename DX>
void ReleaseCOM(DX& item);

bool yo = false;

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;


	ID3D11Device* device = nullptr;

	ID3D11DeviceContext* context = nullptr;

	ID3D11RenderTargetView* renderTargetView = nullptr;

	IDXGISwapChain* s_chain = nullptr;

	DXGI_SWAP_CHAIN_DESC swapChain;

	ID3D11Texture2D* m_texture = nullptr;
	ID3D11Texture2D* pDepthStencil = nullptr;

	D3D11_VIEWPORT viewPort;

	ID3D11Buffer* vram_Buffer = nullptr;
	ID3D11Buffer* constantBuffer = nullptr;
	ID3D11Buffer* VertBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;

	ID3D11Buffer* GridBuffer = nullptr;

	ID3D11VertexShader* VS_Shader = nullptr;
	ID3D11PixelShader* PS_Shader = nullptr;

	ID3D11InputLayout* layout = nullptr;

	ID3D11DepthStencilView* pDSV = nullptr;

	ID3D11SamplerState *m_sampler = nullptr;
	ID3D11ShaderResourceView *m_shaderResource = nullptr;
	struct SEND_TO_VRAM
	{
		float constantColor[4];
		float constantOffset[2];
		float padding[2];
	};
	struct Obj
	{
		float SV_WorldMatrix[4][4];
		float SV_ProjectionMatrix[4][4];
		float SV_ViewMatrix[4][4];
	};
	/*struct Scene
	{
	float SV_ProjectionMatrix[4][4];
	float SV_ViewMatrix[4][4];
	};*/

	SEND_TO_VRAM toShader;
	SEND_TO_VRAM verts[776];
	//Scene toShaderWorld;
	Obj toShaderWorld;
	float m_time = 0.0f;
	Matrix rotate;
	Matrix WorldSpaceCamera;
public:
	struct SIMPLE_VERTEX
	{
		float COORD[2];
	};
	struct VertexBuffer
	{
		float COORD[4];
		float m_color[4];
	};

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);

	bool Run();
	bool ShutDown();
	void Resize();
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"Lab 1a Line Land", WS_OVERLAPPEDWINDOW /*& ~(WS_THICKFRAME | WS_MAXIMIZEBOX)*/,
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);
	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//
	float Sw = BACKBUFFER_WIDTH;
	float Sh = BACKBUFFER_HEIGHT;
	float Zf = 100.0f;
	float Zn = 0.1f;


	ZeroMemory(&swapChain, sizeof(swapChain));
	swapChain.BufferCount = 1;
	swapChain.BufferDesc.Width = BACKBUFFER_WIDTH;
	swapChain.BufferDesc.Height = BACKBUFFER_HEIGHT;
	swapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChain.BufferDesc.RefreshRate.Denominator = 1;
	swapChain.BufferDesc.RefreshRate.Numerator = 60;
	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.OutputWindow = window;
	swapChain.SampleDesc.Count = 1;
	swapChain.SampleDesc.Quality = 0;
	swapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChain.Windowed = TRUE;
	UINT m_DeviceFlags = 0;
#if _DEBUG
	m_DeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, m_DeviceFlags, nullptr,
		NULL, D3D11_SDK_VERSION, &swapChain, &s_chain, &device, nullptr, &context);

	s_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_texture);
	device->CreateRenderTargetView(m_texture, nullptr, &renderTargetView);

	ZeroMemory(&viewPort, sizeof(viewPort));
	viewPort.Width = BACKBUFFER_WIDTH;
	viewPort.Height = BACKBUFFER_HEIGHT;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

#if Lab 8
	m_unique[0].COORD[0] = 0.0f;
	m_unique[0].COORD[1] = 0.0f;
	m_unique[0].COORD[2] = -0.25f;
	m_unique[0].COORD[3] = 1.0f;
	m_unique[0].m_color[0] = 1.0f;
	m_unique[0].m_color[1] = 0.0f;
	m_unique[0].m_color[2] = 0.0f;
	m_unique[0].m_color[3] = 1.0f;

	m_unique[11].COORD[0] = 0.0f;
	m_unique[11].COORD[1] = 0.0f;
	m_unique[11].COORD[2] = 0.25f;
	m_unique[11].COORD[3] = 1.0f;
	m_unique[11].m_color[0] = 1.0f;
	m_unique[11].m_color[1] = 0.0f;
	m_unique[11].m_color[2] = 0.0f;
	m_unique[11].m_color[3] = 1.0f;

	float num = 0;
	for (size_t i = 1; i < 6; i++)
	{
		m_unique[i].COORD[0] = cos(ConvertDegreestoRadians(num)) *0.8f;
		m_unique[i].COORD[1] = sin(ConvertDegreestoRadians(num)) *0.8f;
		m_unique[i].COORD[2] = 0;
		m_unique[i].COORD[3] = 1;

		m_unique[i].m_color[0] = 1.0f;
		m_unique[i].m_color[1] = 1.0f;
		m_unique[i].m_color[2] = 0.0f;
		m_unique[i].m_color[3] = 1.0f;

		num += 72;
	}
	num = 36;
	for (size_t i = 6; i < 11; i++)
	{
		m_unique[i].COORD[0] = cos(ConvertDegreestoRadians(num)) *0.4f;
		m_unique[i].COORD[1] = sin(ConvertDegreestoRadians(num)) *0.4f;
		m_unique[i].COORD[2] = 0;
		m_unique[i].COORD[3] = 1;

		m_unique[i].m_color[0] = 1.0f;
		m_unique[i].m_color[1] = 0.0f;
		m_unique[i].m_color[2] = 1.0f;
		m_unique[i].m_color[3] = 1.0f;

		num += 72;
	}
	m_verts[0] = 1;
	m_verts[1] = 10;
	m_verts[2] = 0;

	m_verts[3] = 10;
	m_verts[4] = 5;
	m_verts[5] = 0;

	m_verts[6] = 5;
	m_verts[7] = 9;
	m_verts[8] = 0;

	m_verts[9] = 9;
	m_verts[10] = 4;
	m_verts[11] = 0;

	m_verts[12] = 4;
	m_verts[13] = 8;
	m_verts[14] = 0;

	m_verts[15] = 8;
	m_verts[16] = 3;
	m_verts[17] = 0;

	m_verts[18] = 3;
	m_verts[19] = 7;
	m_verts[20] = 0;

	m_verts[21] = 7;
	m_verts[22] = 2;
	m_verts[23] = 0;

	m_verts[24] = 2;
	m_verts[25] = 6;
	m_verts[26] = 0;

	m_verts[27] = 6;
	m_verts[28] = 1;
	m_verts[29] = 0;

	m_verts[30] = 1;
	m_verts[31] = 11;
	m_verts[32] = 10;

	m_verts[33] = 10;
	m_verts[34] = 11;
	m_verts[35] = 5;

	m_verts[36] = 5;
	m_verts[37] = 11;
	m_verts[38] = 9;

	m_verts[39] = 9;
	m_verts[40] = 11;
	m_verts[41] = 4;

	m_verts[42] = 4;
	m_verts[43] = 11;
	m_verts[44] = 8;

	m_verts[45] = 8;
	m_verts[46] = 11;
	m_verts[47] = 3;

	m_verts[48] = 3;
	m_verts[49] = 11;
	m_verts[50] = 7;

	m_verts[51] = 7;
	m_verts[52] = 11;
	m_verts[53] = 2;

	m_verts[54] = 2;
	m_verts[55] = 11;
	m_verts[56] = 6;

	m_verts[57] = 6;
	m_verts[58] = 11;
	m_verts[59] = 1;
 // Lab 8
	//VertexBuffer m_grid[2];
	//m_grid[0].COORD[0] = -0.5;
	//m_grid[0].COORD[1] = 0.0;
	//m_grid[0].COORD[2] = 0.0;
	//m_grid[0].COORD[3] = 1;
	//m_grid[0].m_color[0] = 1.0f;
	//m_grid[0].m_color[1] = 1.0f;
	//m_grid[0].m_color[2] = 1.0f;
	//m_grid[0].m_color[3] = 1.0f;

	//m_grid[1].COORD[0] = -0.5;
	//m_grid[1].COORD[1] = 0.0;
	//m_grid[1].COORD[2] = 0.5;
	//m_grid[1].COORD[3] = 1;
	//m_grid[1].m_color[0] = 1.0f;
	//m_grid[1].m_color[1] = 1.0f;
	//m_grid[1].m_color[2] = 1.0f;
	//m_grid[1].m_color[3] = 1.0f;
	////float incrementor = -0.5;
	////float bob = -0.5;
	////for (size_t i = 0; i < 21; i++)
	////{
	////	m_grid[0][i].COORD[0] = bob;// incrementor;
	////	m_grid[0][i].COORD[1] = 0.0;// -0.5;
	////	m_grid[0][i].COORD[2] = incrementor;
	////	m_grid[0][i].COORD[3] = 1;
	////	m_grid[0][i].m_color[0] = 1.0f;
	////	m_grid[0][i].m_color[1] = 1.0f;
	////	m_grid[0][i].m_color[2] = 1.0f;
	////	m_grid[0][i].m_color[3] = 1.0f;
	////	//incrementor += 0.1;
	////	bob += 0.1;
	////}
	////incrementor = 0.5;
	////bob = -0.5;
	////for (size_t i = 0; i < 21; i++)
	////{
	////	m_grid[1][i].COORD[0] = bob;//incrementor;
	////	m_grid[1][i].COORD[1] = 0.0;//0.5;
	////	m_grid[1][i].COORD[2] = incrementor;
	////	m_grid[1][i].COORD[3] = 1;
	////	m_grid[1][i].m_color[0] = 1.0f;
	////	m_grid[1][i].m_color[1] = 1.0f;
	////	m_grid[1][i].m_color[2] = 1.0f;
	////	m_grid[1][i].m_color[3] = 1.0f;
	////	//incrementor -= 0.1;
	////	bob += 0.1;
	////}
	////incrementor = -0.5;
	////for (size_t i = 0; i < 21; i++)
	////{
	////	m_grid[2][i].COORD[0] = -0.5;
	////	m_grid[2][i].COORD[1] = 0.0;//incrementor;
	////	m_grid[2][i].COORD[2] = incrementor;
	////	m_grid[2][i].COORD[3] = 1;
	////	m_grid[2][i].m_color[0] = 1.0f;
	////	m_grid[2][i].m_color[1] = 1.0f;
	////	m_grid[2][i].m_color[2] = 1.0f;
	////	m_grid[2][i].m_color[3] = 1.0f;
	////	incrementor += 0.1;
	////}
	////incrementor = -0.5;
	////for (size_t i = 0; i < 21; i++)
	////{
	////	m_grid[3][i].COORD[0] = 0.5;
	////	m_grid[3][i].COORD[1] = 0.0;// incrementor;
	////	m_grid[3][i].COORD[2] = incrementor;
	////	m_grid[3][i].COORD[3] = 1;
	////	m_grid[3][i].m_color[0] = 1.0f;
	////	m_grid[3][i].m_color[1] = 1.0f;
	////	m_grid[3][i].m_color[2] = 1.0f;
	////	m_grid[3][i].m_color[3] = 1.0f;
	////	incrementor += 0.1;
	////}
	//D3D11_BUFFER_DESC gridBuffer;
	//ZeroMemory(&gridBuffer, sizeof(gridBuffer));
	//gridBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//gridBuffer.ByteWidth = sizeof(VertexBuffer) * 2;
	//gridBuffer.Usage = D3D11_USAGE_IMMUTABLE;


	//D3D11_SUBRESOURCE_DATA gridData;
	//ZeroMemory(&gridData, sizeof(gridData));
	//gridData.pSysMem = m_grid;

	//device->CreateBuffer(&gridBuffer, &gridData, &GridBuffer);
	//D3D11_BUFFER_DESC vertBuffer;
	//ZeroMemory(&vertBuffer, sizeof(vertBuffer));
	//vertBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vertBuffer.ByteWidth = sizeof(VertexBuffer) * 776;
	//vertBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	//D3D11_SUBRESOURCE_DATA vertData;
	//ZeroMemory(&vertData, sizeof(vertData));
	//vertData.pSysMem = m_unique;
	//device->CreateBuffer(&vertBuffer, &vertData, &VertBuffer);

	//D3D11_BUFFER_DESC indexBuffer;
	//ZeroMemory(&indexBuffer, sizeof(indexBuffer));
	//indexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//indexBuffer.ByteWidth = sizeof(UINT) * 1692;
	//indexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	//D3D11_SUBRESOURCE_DATA indexData;
	//ZeroMemory(&indexData, sizeof(indexData));
	//indexData.pSysMem = m_verts;
	//device->CreateBuffer(&indexBuffer, &indexData, &IndexBuffer);
#endif
	D3D11_BUFFER_DESC vertBuffer;
	ZeroMemory(&vertBuffer, sizeof(vertBuffer));
	vertBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBuffer.ByteWidth = sizeof(_OBJ_VERT_) * 776;
	vertBuffer.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vertData;
	ZeroMemory(&vertData, sizeof(vertData));
	vertData.pSysMem = Cube_data;

	device->CreateBuffer(&vertBuffer, &vertData, &VertBuffer);

	D3D11_BUFFER_DESC indexBuffer;
	ZeroMemory(&indexBuffer, sizeof(indexBuffer));
	indexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffer.ByteWidth = sizeof(const unsigned int) * 1692;
	indexBuffer.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = Cube_indicies;

	device->CreateBuffer(&indexBuffer, &indexData, &IndexBuffer);

	device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &VS_Shader);
	device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &PS_Shader);

	D3D11_INPUT_ELEMENT_DESC CircleVertLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(CircleVertLayout, 3, Trivial_VS, sizeof(Trivial_VS), &layout);

	D3D11_BUFFER_DESC constbuffDesc;
	ZeroMemory(&constbuffDesc, sizeof(constbuffDesc));
	constbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbuffDesc.ByteWidth = sizeof(Obj);
	constbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	constbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&constbuffDesc, nullptr, &constantBuffer);

	//toShaderWorld.SV_ProjectionMatrix[0][0] = (2* Zn) / Sw;
	toShaderWorld.SV_ProjectionMatrix[0][0] = 1;
	toShaderWorld.SV_ProjectionMatrix[0][1] = 0;
	toShaderWorld.SV_ProjectionMatrix[0][2] = 0;
	toShaderWorld.SV_ProjectionMatrix[0][3] = 0;

	toShaderWorld.SV_ProjectionMatrix[1][0] = 0;
	//toShaderWorld.SV_ProjectionMatrix[1][1] = (2 * Zn) / Sh;
	toShaderWorld.SV_ProjectionMatrix[1][1] = 1;
	toShaderWorld.SV_ProjectionMatrix[1][2] = 0;
	toShaderWorld.SV_ProjectionMatrix[1][3] = 0;

	toShaderWorld.SV_ProjectionMatrix[2][0] = 0;
	toShaderWorld.SV_ProjectionMatrix[2][1] = 0;
	toShaderWorld.SV_ProjectionMatrix[2][2] = Zf / (Zf - Zn);
	toShaderWorld.SV_ProjectionMatrix[2][3] = 1;

	toShaderWorld.SV_ProjectionMatrix[3][0] = 0;
	toShaderWorld.SV_ProjectionMatrix[3][1] = 0;
	toShaderWorld.SV_ProjectionMatrix[3][2] = (-Zf * Zn) / (Zf - Zn);
	toShaderWorld.SV_ProjectionMatrix[3][3] = 0;

	WorldSpaceCamera = BuildIdentityMatrix();
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			toShaderWorld.SV_ViewMatrix[i][z] = WorldSpaceCamera.vertex[i][z];
		}
	}

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = BACKBUFFER_WIDTH;
	descDepth.Height = BACKBUFFER_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	device->CreateTexture2D(&descDepth, NULL, &pDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(pDepthStencil,
		&descDSV,
		&pDSV);
	rotate = BuildIdentityMatrix();
	rotate.vertex[3][2] = 2;

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(TextureDesc));
	TextureDesc.Width = crosshatch_width;
	TextureDesc.Height = crosshatch_height;
	TextureDesc.MipLevels = 12;
	TextureDesc.ArraySize = 1;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.SampleDesc.Count = 1;

	D3D11_SUBRESOURCE_DATA TextureData[12];
	for (int i = 0; i < 12; i++)
	{
		TextureData[i].pSysMem = &crosshatch_pixels[crosshatch_leveloffsets[i]];
		TextureData[i].SysMemPitch = (crosshatch_width >> i) * sizeof(UINT);
		TextureData[i].SysMemSlicePitch = 0;
	}
	device->CreateTexture2D(&TextureDesc, TextureData, &m_texture);

	D3D11_SAMPLER_DESC SamplerDesc;
	//D3D11_TEXTURE_ADDRESS_CLAMP;
	ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	SamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&SamplerDesc, &m_sampler);

	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderViewDesc;
	ZeroMemory(&ShaderViewDesc, sizeof(ShaderViewDesc));
	ShaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderViewDesc.Texture2D.MostDetailedMip = 0;
	ShaderViewDesc.Texture2D.MipLevels = 12;
	ShaderViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	device->CreateShaderResourceView(m_texture, &ShaderViewDesc, &m_shaderResource);
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************
void DEMO_APP::Resize()
{
	if (s_chain)
	{
		context->OMSetRenderTargets(0, 0, 0);

		// Release all outstanding references to the swap chain's buffers.
		renderTargetView->Release();

		HRESULT hr;
		// Preserve the existing buffer count and format.
		// Automatically choose the width and height to match the client rect for HWNDs.
		hr = s_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		// Perform error handling here!

		// Get buffer and create a render-target-view.
		ID3D11Texture2D* pBuffer;
		hr = s_chain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			(void**)&pBuffer);
		// Perform error handling here!

		hr = device->CreateRenderTargetView(pBuffer, NULL,
			&renderTargetView);
		// Perform error handling here!
		pBuffer->Release();

		context->OMSetRenderTargets(1, &renderTargetView, NULL);

		// Set up the viewport.
		D3D11_VIEWPORT vp;
		vp.Width = GetSystemMetrics(SM_CXSCREEN);
		vp.Height = GetSystemMetrics(SM_CYSCREEN);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		context->RSSetViewports(1, &vp);


		/*DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)GetSystemMetrics(SM_CXSCREEN);
		dmScreenSettings.dmPelsHeight = (unsigned long)GetSystemMetrics(SM_CYSCREEN);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);*/
	}
	return;
}

bool DEMO_APP::Run()
{
	timer.Signal();
	if (timer.TotalTime() > 360)
	{
		timer.Restart();
	}
	if (GetAsyncKeyState(VK_UP) & 0x1)
	{
		WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, Translate(0, 0, 0.05));
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x1)
	{
		WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, Translate(0, 0, -0.05));
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x1)
	{
		WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, Translate(0.05, 0, 0));
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x1)
	{
		WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, Translate(-0.05, 0, 0));
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x1)
	{
		WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, Translate(0, 0.05, 0));
	}

	if (GetAsyncKeyState(VK_NUMPAD4) & 0x1)
	{
		WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, BuildRotationMatrixOnAxisX(0.05));
	}
	if (GetAsyncKeyState(VK_NUMPAD6) & 0x1)
	{
		WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, BuildRotationMatrixOnAxisX(-0.05));
	}
	if (GetAsyncKeyState(VK_NUMPAD8) & 0x1)
	{
		WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, BuildRotationMatrixOnAxisY(0.05));
	}
	if (GetAsyncKeyState(VK_NUMPAD2) & 0x1)
	{
		WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, BuildRotationMatrixOnAxisY(-0.05));
	}
	Matrix inv = Inverse(WorldSpaceCamera);
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			toShaderWorld.SV_ViewMatrix[i][z] = inv.vertex[i][z];
		}
	}
	rotate = MatrixMatrixMultipy(BuildRotationMatrixOnAxisY(ConvertDegreestoRadians(timer.Delta() * 20)), rotate);
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			toShaderWorld.SV_WorldMatrix[i][z] = rotate.vertex[i][z];
		}
	}

	if (yo)
	{
		Resize();
	}
	else
	{
		context->OMSetRenderTargets(1, &renderTargetView, pDSV);
		context->RSSetViewports(1, &viewPort);
		float m_color[4] = { 0.0f, 0.0f, 1.0f, 1 };
		context->ClearRenderTargetView(renderTargetView, m_color);
		context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	context->PSSetShaderResources(0, 1, &m_shaderResource);
	context->PSSetSamplers(0, 1, &m_sampler);
#if 1
	D3D11_MAPPED_SUBRESOURCE subData;
	ZeroMemory(&subData, sizeof(subData));

	context->Map(constantBuffer, NULL, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &subData);
	memcpy(subData.pData, &toShaderWorld, sizeof(toShaderWorld));
	context->Unmap(constantBuffer, NULL);
	context->VSSetConstantBuffers(1, 1, &constantBuffer);

	UINT stride = sizeof(_OBJ_VERT_);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &VertBuffer, &stride, &offset);
	context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->VSSetShader(VS_Shader, 0, 0);
	context->PSSetShader(PS_Shader, 0, 0);
	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	context->DrawIndexed(1692, 0, 0);

	s_chain->Present(0, 0);
#endif
	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6
	ReleaseCOM(device);
	ReleaseCOM(context);
	ReleaseCOM(renderTargetView);
	ReleaseCOM(s_chain);
	ReleaseCOM(m_texture);
	ReleaseCOM(layout);
	ReleaseCOM(vram_Buffer);
	ReleaseCOM(VS_Shader);
	ReleaseCOM(PS_Shader);
	ReleaseCOM(constantBuffer);
	ReleaseCOM(VertBuffer);
	ReleaseCOM(IndexBuffer);
	ReleaseCOM(pDSV);
	ReleaseCOM(pDepthStencil);
	ReleaseCOM(m_shaderResource);
	ReleaseCOM(m_sampler);
	ReleaseCOM(GridBuffer);

	UnregisterClass(L"DirectXApplication", application);
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	myApp.ShutDown();
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_TAB)
		{
			yo = true;
		}
	}break;
	case (WM_DESTROY) : { PostQuitMessage(0); }
						break;
	}
						return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//

template<typename DX>
void ReleaseCOM(DX& item)
{
	if (item != NULL)
	{
		item->Release();
		item = NULL;
	}
}
