#pragma once

#include "Shader.h"

#include <D3D11.h>
#include <D3DX10math.h>

class VertexBuffer
{
public:
	struct Vertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 uv;
	};
	VertexBuffer();
	~VertexBuffer();

	bool Initialize(ID3D11Device* device, Shader* shader, float size, bool writeable = false);
	void Render(ID3D11DeviceContext* deviceContext);

	Vertex* GetVertices();
	ID3D11Buffer* GetVetexBuffer();
	int GetIndexCount();

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
	Vertex* m_vertices;
	Shader* m_shader;
};

