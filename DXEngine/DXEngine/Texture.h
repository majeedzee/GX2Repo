#pragma once

#include <d3d11.h>
#include <D3DX11tex.h>

#include "Defines.h"

class Texture
{
public:
	Texture();
	~Texture();

	bool Initialize(ID3D11Device* device, LPCSTR filename);

	ID3D11ShaderResourceView* GetTexture();

	int GetWidth();
	int GetHeight();

private:
	ID3D11ShaderResourceView* m_texture;
	int m_width;
	int m_height;

};

