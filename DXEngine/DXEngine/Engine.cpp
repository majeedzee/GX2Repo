#include "Engine.h"

Engine* Engine::m_instance = NULL;

Engine::Engine()
{
	m_graphics = NULL;
}


Engine::~Engine()
{
	if (m_graphics)
	{
		delete m_graphics;
		m_graphics = NULL;
	}
}

bool Engine::InitializeGraphics(HWND hwnd)
{
	m_graphics = new Graphics();

	return m_graphics->InitializeDX(hwnd);
}

bool Engine::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	m_graphics->Initialize();

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

	m_graphics->EndScene();
}

Graphics* Engine::GetGraphics()
{
	return m_graphics;
}