#include "DXManager.h"


DXManager::DXManager()
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_rasterState = nullptr;
	m_alphaEnabledBlendState = nullptr;
	m_alphaDisabledBlendState = nullptr;
	m_depthDisabledStencilState = nullptr;
}


DXManager::~DXManager()
{
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	ReleaseCom(m_swapChain);
	ReleaseCom(m_device);
	ReleaseCom(m_deviceContext);
	ReleaseCom(m_renderTargetView);
	ReleaseCom(m_depthStencilBuffer);
	ReleaseCom(m_depthStencilState);
	ReleaseCom(m_depthStencilView);
	ReleaseCom(m_rasterState);
	ReleaseCom(m_alphaEnabledBlendState);
	ReleaseCom(m_alphaDisabledBlendState);
	ReleaseCom(m_depthDisabledStencilState);
}

bool DXManager::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen)
{
	HRESULT result;
	IDXGIFactory *factory;
	IDXGIAdapter *adapter;
	IDXGIOutput *adapterOutput;
	unsigned int numModes, numerator = 0, denominator = 0, stringLength;
	DXGI_MODE_DESC *displayModeList;
	DXGI_ADAPTER_DESC  adapterDesc;
	int error;
	ID3D11Texture2D* backBufferPtr;

	//store the vsync settings
	m_vsync_enabled = vsync;

	//Create a DX graphic interface factory
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//Use the factory to create an adapter for the primary graphics interface
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//Enumerate the primary adapter output
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//Get the number of modes that fit the DXGI_Format_R8G8B8A8_UNORM display format for the adapter output
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//Create list to hold all the possible modes for this monitor/video card
	displayModeList = new DXGI_MODE_DESC[numModes];

	//Fill the displaymodes
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	for (size_t i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	if (numerator == 0 && denominator == 0)
	{
		return false;
	}

	//Get the adapter description
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	//Store the video card mem in mb
	m_videCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert name of video card to array
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//Release memory
	delete[] displayModeList;
	displayModeList = nullptr;
	
	ReleaseCom(adapterOutput);
	ReleaseCom(adapter);
	ReleaseCom(factory);

	if (!InitializeSwapChain(hwnd, fullscreen, screenWidth, screenHeight, numerator, denominator))
	{
		return false;
	}
	
	// get the pointer to the back buffer
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// create rendertarget view
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// release back buffer pointer
	ReleaseCom(backBufferPtr);

	if (!InitializeDepthBuffer(screenWidth, screenHeight))
	{
		return false;
	}

	if (!InitializeDepthStencilBuffer())
	{
		return false;
	}

	if (!InitializeStencilView())
	{
		return false;
	}
	// Bind rendertarget view and depth stencil to pipeline
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	if (!InitializeRasterizerState())
	{
		return false;
	}

	InitializeViewport(screenWidth, screenHeight);
	
	if (!InitializeAlphaBlending())
	{
		return false;
	}

	if (!InitializeZBuffer())
	{
		return false;
	}
	// Yay it worked
	return true;
}

void DXManager::BeginScene(float r, float g, float b, float a)
{
	float color[4];

	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	//clearing the backbuff
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// clear the dpeth buffer
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DXManager::EndScene()
{
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate
		m_swapChain->Present(1, 0);
	}
	else
	{
		// present as fast as poss
		m_swapChain->Present(0, 0);
	}
}

void DXManager::EnableAlphaBlending(bool enable)
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	if (enable)
	{
		m_deviceContext->OMSetBlendState(m_alphaEnabledBlendState, blendFactor, 0xFFFFFFFF);
	}
	else
	{
		m_deviceContext->OMSetBlendState(m_alphaDisabledBlendState, blendFactor, 0xFFFFFFFF);
	}
}

void DXManager::EnableZBuffer(bool enable)
{
	if (enable)
	{
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	}
	else
	{
		m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);

	}
}

bool DXManager::InitializeSwapChain(HWND hwnd, bool fullscreen, int screenWidth, int screenHeight, int numerator, unsigned int denominator)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featurelevel;
	HRESULT result;

	//Init swap chain
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	
	swapChainDesc.Windowed = fullscreen;
	
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Discard back buffer contents after presenting
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = 0;

	// Setting the feature level to dx11
	// Assumed that we are using dx11
	featurelevel = D3D_FEATURE_LEVEL_11_0;
	
	// create device and context
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, 
		&featurelevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	
	return true;
}

bool DXManager::InitializeDepthBuffer(int screenWidth, int screenHeight)
{
	D3D11_TEXTURE2D_DESC DepthBuffDesc;
	HRESULT result;

	// Init depthbuffdesc
	ZeroMemory(&DepthBuffDesc, sizeof(DepthBuffDesc));

	DepthBuffDesc.Width = screenWidth;
	DepthBuffDesc.Height = screenHeight;
	DepthBuffDesc.MipLevels = 1;
	DepthBuffDesc.ArraySize = 1;
	DepthBuffDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthBuffDesc.SampleDesc.Count = 1;
	DepthBuffDesc.SampleDesc.Quality = 0;
	DepthBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBuffDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthBuffDesc.CPUAccessFlags = 0;
	DepthBuffDesc.MiscFlags = 0;

	result = m_device->CreateTexture2D(&DepthBuffDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

bool DXManager::InitializeDepthStencilBuffer()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	HRESULT result;

	// Initialize desc
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// stencil operation if pixel is back facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	return true;
}

bool DXManager::InitializeStencilView()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	HRESULT result;

	// Initialize desc
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool DXManager::InitializeRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterDesc;
	HRESULT result;

	ZeroMemory(&rasterDesc, sizeof(rasterDesc));

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);

	if (FAILED(result))
	{
		return false;
	}

	m_deviceContext->RSSetState(m_rasterState);
	return true;
}

void DXManager::InitializeViewport(int screenWidth, int screenHeight)
{
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_deviceContext->RSSetViewports(1, &viewport);
}

bool DXManager::InitializeAlphaBlending()
{
	D3D11_BLEND_DESC blendDesc;
	HRESULT result;
		
	//Initialize
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	result = m_device->CreateBlendState(&blendDesc, &m_alphaEnabledBlendState);
	if (FAILED(result))
	{
		return false;
	}

	// modify to create disable blend
	blendDesc.RenderTarget[0].BlendEnable = false;
	result = m_device->CreateBlendState(&blendDesc, &m_alphaDisabledBlendState);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

bool DXManager::InitializeZBuffer()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	HRESULT result;

	//Initialize
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// stencil operation if pixel is back facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create disabled state
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

template <typename ptr>
void DXManager::ReleaseCom(ptr &item)
{
	if (item != NULL)
	{
		item->Release();
		item = NULL;
	}
}

ID3D11Device* DXManager::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* DXManager::GetDeviceContect()
{
	return m_deviceContext;
}