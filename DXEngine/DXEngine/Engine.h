#pragma once

#include "Graphics.h"

class Engine
{
public:
	~Engine();
	
	bool InitializeGraphics(HWND hwnd);
	bool Initialize(HINSTANCE hInstance, HWND hwnd);
	
	void Run();
	void Release();

	static Engine* GetEngine();
	
	Graphics* GetGraphics();

private:
	Engine();

	void Update();
	void Render();

	Graphics* m_graphics;
	static Engine* m_instance;
};

