#pragma once

#include "Shader.h"

class TextureShader : public Shader
{
public:
	TextureShader(ID3D11Device* device, HWND hwnd, LPCSTR shaderfileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName);
	virtual ~TextureShader();

	void Begin(ID3D11DeviceContext* deviceContext, int indexCount);
	void End(ID3D11DeviceContext* deviceContext);

protected:
	bool Initialize(ID3D11Device* device, HWND hwnd, LPCSTR shaderfileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName);
};

