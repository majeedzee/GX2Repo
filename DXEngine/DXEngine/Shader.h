#pragma once

#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <fstream>

using namespace std;

#include "Defines.h"

class Shader
{
public:
	Shader(ID3D11Device* device, HWND hwnd, LPCSTR shaderfileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName);
	
	virtual ~Shader();

	virtual void Begin(ID3D11DeviceContext* deviceContext, int indexCount);
	virtual void End(ID3D11DeviceContext* deviceContext);

	bool SetShaderParamaters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture);
	bool SetShaderParamaters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);

	char* GetName();

	bool IsInitialized();

protected:
	virtual bool Initialize(ID3D11Device* device, HWND hwnd, LPCSTR shaderfileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName);
	
	bool m_initialized;

private:
	struct MatrixBuffer
	{
		D3DXMATRIX worldMatrix;
		D3DXMATRIX viewMatrix;
		D3DXMATRIX projectionMatrix;
	};

	bool InitializeShader(ID3D11Device* device, HWND hwnd, LPCSTR vsfileName, LPCSTR psfileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName);

	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, LPCSTR shaderFileName);

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*	m_pixelShader;
	ID3D11InputLayout*	m_layout;
	ID3D11Buffer*		m_matrixBuffer;

	char* m_name;

};

