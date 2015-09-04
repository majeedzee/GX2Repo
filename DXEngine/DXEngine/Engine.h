#pragma once

#include "Graphics.h"
#include "TextureShader.h"
#include "Texture.h"
#include "VertexBuffer.h"

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

	VertexBuffer* m_vertexBuffer;
	TextureShader* m_textureShader;
	Texture* m_texture;
};

