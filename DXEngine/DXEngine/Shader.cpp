#include "Shader.h"


Shader::Shader(ID3D11Device* device, HWND hwnd, LPCSTR shaderfileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName)
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;

	m_initialized = Initialize(device, hwnd, shaderfileName, vertexFuncName, pixelFuncName);
}


Shader::~Shader()
{
	ReleaseCom(m_matrixBuffer);
	ReleaseCom(m_pixelShader);
	ReleaseCom(m_layout);
	ReleaseCom(m_vertexShader);

	if (m_name)
	{
		delete[] m_name;
		m_name = NULL;
	}
}

void Shader::Begin(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//render
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

void Shader::End(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetInputLayout(NULL);
	deviceContext->VSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(NULL, NULL, 0);
}

bool Shader::SetShaderParamaters(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture)
{
	deviceContext->PSSetShaderResources(0, 0, &texture);

	return true;
}

bool Shader::SetShaderParamaters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer *dataPtr;
	unsigned int bufferNumber;

	//transpose all matrix
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBuffer*)mappedResource.pData;
	dataPtr->worldMatrix = worldMatrix;
	dataPtr->viewMatrix = viewMatrix;
	dataPtr->projectionMatrix = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

char* Shader::GetName()
{
	return m_name;
}

bool Shader::IsInitialized()
{
	return m_initialized;
}

bool Shader::Initialize(ID3D11Device* device, HWND hwnd, LPCSTR shaderfileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName)
{
	bool result;

	m_name = new char[strlen(shaderfileName) + 1];
	memcpy(m_name, shaderfileName, strlen(shaderfileName) + 1);

	char vsFilename[100];
	strcpy_s(vsFilename, shaderfileName);
	strcat_s(vsFilename, ".vs");
	vsFilename[strlen(shaderfileName) + 4] = '\0';

	char psFilename[100];
	strcpy_s(psFilename, shaderfileName);
	strcat_s(psFilename, ".ps");
	psFilename[strlen(shaderfileName) + 4] = '\0';

	// initialize
	return InitializeShader(device, hwnd, vsFilename, psFilename, vertexFuncName, pixelFuncName);
}

bool Shader::InitializeShader(ID3D11Device* device, HWND hwnd, LPCSTR vsfileName, LPCSTR psfileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	UINT numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	// compile vertex shader
	result = D3DX11CompileFromFile(vsfileName, NULL, NULL, vertexFuncName, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// we have an error message
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsfileName);
		}
		else
		{
			MessageBox(hwnd, vsfileName, "Error in Shader File", MB_OK);
		}
		return false;
	}

	// Compile pixel shader
	result = D3DX11CompileFromFile(psfileName, NULL, NULL, pixelFuncName, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psfileName);
		}
		else
		{
			MessageBox(hwnd, psfileName, "Error in PS Shader File", MB_OK);
		}
		return false;
	}


	//create vertex shader
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//create pixel shader
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// setup data for the shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//create vertex input layout

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	ReleaseCom(vertexShaderBuffer);
	ReleaseCom(pixelShaderBuffer);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, LPCSTR shaderFileName)
{
	char* compileErrors = (char*)errorMessage->GetBufferPointer();
	unsigned long buffersize = errorMessage->GetBufferSize();

	ofstream fout;
	//open file to write
	fout.open("shader-error.txt");

	for (size_t i = 0; i < buffersize; i++)
	{
		fout << compileErrors[i];
	}
	fout.close();

	errorMessage->Release();
	errorMessage = NULL;

	MessageBox(hwnd, "Error compiling shader, Check shader-error.txt", shaderFileName, MB_OK);
}

