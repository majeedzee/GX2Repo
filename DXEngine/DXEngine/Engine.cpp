#include "Engine.h"

Engine* Engine::m_instance = NULL;

Engine::Engine()
{
	m_graphics = NULL;
	m_vertexBuffer = NULL;
	m_textureShader = NULL;
	m_texture = NULL;
}


Engine::~Engine()
{
	if (m_graphics)
	{
		delete m_graphics;
		m_graphics = NULL;
	}

	delete m_vertexBuffer;
	delete m_texture;
	delete m_textureShader;
}

bool Engine::InitializeGraphics(HWND hwnd)
{
	m_graphics = new Graphics();

	return m_graphics->InitializeDX(hwnd);
}

bool Engine::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	m_graphics->Initialize();

	m_textureShader = new TextureShader(m_graphics->GetDevice(), hwnd, "texture", "TextureVertexShader", "TexturePixelShader");

	m_texture = new Texture();
	//m_texture->Initialize(m_graphics->GetDevice(), )

	m_vertexBuffer = new VertexBuffer();
	m_vertexBuffer->Initialize(m_graphics->GetDevice(), m_textureShader, 100.0f, false);
	
	return true;
}

void Engine::Run()
{
	Update();
	Render();
}

void Engine::Release()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

Engine* Engine::GetEngine()
{
	if (m_instance == NULL)
	{
		m_instance = new Engine();
	}
	return m_instance;
}

void Engine::Update()
{

}

void Engine::Render()
{
	m_graphics->BeginScene(0.0f, 0.0f, 1.0f, 1.0f);
	// Lets render some things :D

	D3DXMATRIX viewMatrix;
	D3DXMATRIX projMatrix;
	D3DXMATRIX worldMatrix;

	D3DXVECTOR3 pos = D3DXVECTOR3(0, 0, -100.0f);
	D3DXVECTOR3 up = D3DXVECTOR3(0, 1.0f, 0.0f);
	D3DXVECTOR3 look = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	D3DXMatrixLookAtLH(&viewMatrix, &pos, &look, &up);
	D3DXMatrixOrthoLH(&projMatrix, SCREEN_WIDTH, SCREEN_HEIGHT, 0.1f, 1000.0f);
	D3DXMatrixIdentity(&worldMatrix);

	m_graphics->EnableAlphaBlending(true);

	m_textureShader->SetShaderParamaters(m_graphics->GetDeviceContext(), m_texture->GetTexture());
	m_textureShader->SetShaderParamaters(m_graphics->GetDeviceContext(), worldMatrix, viewMatrix, projMatrix);

	m_vertexBuffer->Render(m_graphics->GetDeviceContext());

	m_graphics->EndScene();
}

Graphics* Engine::GetGraphics()
{
	return m_graphics;
}