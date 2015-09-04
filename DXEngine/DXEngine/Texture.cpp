#include "Texture.h"


Texture::Texture()
{
	m_width = 0;
	m_height = 0;
	m_texture = nullptr;
}

Texture::~Texture()
{
	ReleaseCom(m_texture);
}

bool Texture::Initialize(ID3D11Device* device, LPCSTR filename)
{
	HRESULT result;

	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	if (FAILED(result))
	{
		return false;
	}
	
	ID3D11Resource* resource = nullptr;
	m_texture->GetResource(&resource);

	ID3D11Texture2D* texture2d = nullptr;

	result = resource->QueryInterface(&texture2d);

	if (SUCCEEDED(result))
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		texture2d->GetDesc(&textureDesc);

		m_width = textureDesc.Width;
		m_height = textureDesc.Height;
	}

	ReleaseCom(texture2d);
	ReleaseCom(resource);

	return true;
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_texture;
}

int Texture::GetWidth()
{
	return m_width;
}

int Texture::GetHeight()
{
	return m_height;
}