#include "VertexBuffer.h"


VertexBuffer::VertexBuffer()
{
	m_vertices = 0;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_shader = nullptr;
}


VertexBuffer::~VertexBuffer()
{
	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
	}
	ReleaseCom(m_vertexBuffer);
	ReleaseCom(m_indexBuffer);
}

bool VertexBuffer::Initialize(ID3D11Device* device, Shader* shader, float size, bool writeable/* = false*/)
{
	ULONG *indicies;
	D3D11_BUFFER_DESC vertBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertData, indexData;
	HRESULT result;

	m_vertexCount = 4;
	m_indexCount = 6;

	m_vertices = new Vertex[m_vertexCount];
	indicies = new ULONG[m_indexCount];

	float halfSize = size / 2.0f;

	// vertices
	m_vertices[0].pos = D3DXVECTOR3(-halfSize, -halfSize, 0.0f);
	m_vertices[0].uv = D3DXVECTOR2(0.0f, 1.0f);

	m_vertices[1].pos = D3DXVECTOR3(-halfSize, halfSize, 0.0f);
	m_vertices[1].uv = D3DXVECTOR2(0.0f, 0.0f);

	m_vertices[2].pos = D3DXVECTOR3(halfSize, halfSize, 0.0f);
	m_vertices[2].uv = D3DXVECTOR2(1.0f, 0.0f);

	m_vertices[3].pos = D3DXVECTOR3(halfSize, -halfSize, 0.0f);
	m_vertices[3].uv = D3DXVECTOR2(1.0f, 1.0f);

	// indicies
	indicies[0] = 0;
	indicies[1] = 1;
	indicies[2] = 2;
	indicies[3] = 0;
	indicies[4] = 2;
	indicies[5] = 3;

	ZeroMemory(&vertBufferDesc, sizeof(vertBufferDesc));
	vertBufferDesc.Usage = (writeable) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	vertBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBufferDesc.CPUAccessFlags = (writeable) ? D3D11_CPU_ACCESS_WRITE : 0;
	vertBufferDesc.MiscFlags = 0;
	vertBufferDesc.StructureByteStride = 0;

	ZeroMemory(&vertData, sizeof(vertData));
	vertData.pSysMem = m_vertices;
	vertData.SysMemPitch = 0;
	vertData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertBufferDesc, &vertData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(ULONG) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = indicies;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	delete[] indicies;
	indicies = 0;

	m_shader = shader;
	return true;
}

void VertexBuffer::Render(ID3D11DeviceContext* deviceContext)
{
	UINT stride;
	UINT offset;

	stride = sizeof(Vertex);
	offset = 0;

	m_shader->Begin(deviceContext, m_indexCount);

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_shader->End(deviceContext);
}

VertexBuffer::Vertex* VertexBuffer::GetVertices()
{
	return m_vertices;
}

ID3D11Buffer* VertexBuffer::GetVetexBuffer()
{
	return m_vertexBuffer;
}

int VertexBuffer::GetIndexCount()
{
	return m_indexCount;
}