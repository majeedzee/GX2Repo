#include "DxEngine.h"

#include "Trivial_VS.csh"
#include "VertexShader.csh"
#include "PixelShader.csh"
#include "Trivial_PS.csh"
#include "SkyboxPixel.csh"
#include "SkyboxVertex.csh"



DxEngine::DxEngine()
{
	device = nullptr;
	context = nullptr;
	renderTargetView = nullptr;
	s_chain = nullptr;
	m_texture = nullptr;
	pDepthStencil = nullptr;
	vram_Buffer = nullptr;
	constantBuffer = nullptr;
	VertBuffer = nullptr;
	IndexBuffer = nullptr;
	m_VertBuffer = nullptr;
	m_IndexBuffer = nullptr;
	m_InstanceBuffer = nullptr;
	VS_Shader = nullptr;
	VS_InstanceShader = nullptr;
	PS_PixelShader = nullptr;
	PS_Shader = nullptr;
	layout = nullptr;
	m_layout = nullptr;
	pDSV = nullptr;
	m_sampler = nullptr;
	m_shaderResource = nullptr;
	m_secondshaderResource = nullptr;
	m_shipResource = nullptr;
	m_alphaEnabledBlendState = nullptr;
	m_RasterState = nullptr;
	VS_SkyboxShader = nullptr;
	PS_SkyboxShader = nullptr;
	plane_VertBuffer = nullptr;
	plane_IndexBuffer = nullptr;
	m_secondshipResource = nullptr;
	defferedContext = nullptr;
	command = nullptr;
	m_torchVertBuffer = nullptr;
	m_lightBuffer = nullptr;

	fullscreen = false;
	once = true;
	FULL_SCREEN = false;
	change = false;

	valueX = 0;
	valueY = 0;
}

DxEngine::~DxEngine()
{
}

void DxEngine::ShutDown()
{
	ReleaseCOM(device);
	ReleaseCOM(context);
	ReleaseCOM(renderTargetView);
	ReleaseCOM(s_chain);
	ReleaseCOM(m_texture);
	ReleaseCOM(layout);
	ReleaseCOM(vram_Buffer);
	ReleaseCOM(VS_Shader);
	ReleaseCOM(PS_Shader);
	ReleaseCOM(constantBuffer);
	ReleaseCOM(VertBuffer);
	ReleaseCOM(IndexBuffer);
	ReleaseCOM(pDSV);
	ReleaseCOM(pDepthStencil);
	ReleaseCOM(m_shaderResource);
	ReleaseCOM(m_secondshaderResource);
	ReleaseCOM(m_sampler);
	ReleaseCOM(m_RasterState);
	ReleaseCOM(m_alphaEnabledBlendState);
	ReleaseCOM(m_VertBuffer);
	ReleaseCOM(m_IndexBuffer);
	ReleaseCOM(m_InstanceBuffer);
	ReleaseCOM(VS_InstanceShader);
	ReleaseCOM(m_shipResource);
	ReleaseCOM(PS_PixelShader);
	ReleaseCOM(VS_SkyboxShader);
	ReleaseCOM(PS_SkyboxShader);
	ReleaseCOM(plane_IndexBuffer);
	ReleaseCOM(plane_VertBuffer);
	ReleaseCOM(m_secondshipResource);
	ReleaseCOM(command);
	ReleaseCOM(defferedContext);
	ReleaseCOM(m_torchVertBuffer);
	ReleaseCOM(m_lightBuffer);
}

bool DxEngine::LoadObject(char* path, vector<VertexBuffer> *out_vertices, vector< unsigned int > *indicies)
{
	vector<unsigned int> vertexIndices, uvIndices, normalIndices;

	vector< XMFLOAT3 > temp_vertices;
	vector< XMFLOAT2 > temp_uvs;
	vector< XMFLOAT3 > temp_normals;

	FILE * file;

	fopen_s(&file, path, "r");

	if (file == NULL)
	{
		printf("Can't open the file !\n");
		return false;
	}

	while (true)
	{
		char lineHeader[128];
		int result = fscanf_s(file, "%s", lineHeader, 128);

		if (result == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			XMFLOAT3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			XMFLOAT3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				printf("File can't be read\n");
				return false;
			}

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		bool equal = false;

		XMFLOAT3 vertex = temp_vertices[vertexIndex - 1];

		XMFLOAT2 uv = temp_uvs[uvIndex - 1];
		XMFLOAT3 normal = temp_normals[normalIndex - 1];

		VertexBuffer out;
		out.UV[0] = uv.x;
		out.UV[1] = uv.y;
		out.norm[0] = normal.x;
		out.norm[1] = normal.y;
		out.norm[2] = normal.z;
		out.COORD[0] = vertex.x;
		out.COORD[1] = vertex.y;
		out.COORD[2] = vertex.z;

		out_vertices->push_back(out);
		indicies->push_back(vertexIndex);
	}

	//*indicies = vertexIndices;
	return true;
}

bool DxEngine::InitializeLight()
{
	m_point.atten[0] = 0;
	m_point.atten[1] = 0.2f;
	m_point.atten[2] = 0;

	m_point.range = 100;
	m_point.pos[0] = 0;
	m_point.pos[1] = 2;
	m_point.pos[2] = 5;

	return true;
}

bool DxEngine::InitializeSwapChain(HWND window)
{
	HRESULT hr;
	ZeroMemory(&swapChain, sizeof(swapChain));
	swapChain.BufferCount = 1;
	swapChain.BufferDesc.Width = BACKBUFFER_WIDTH;
	swapChain.BufferDesc.Height = BACKBUFFER_HEIGHT;
	swapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChain.BufferDesc.RefreshRate.Denominator = 1;
	swapChain.BufferDesc.RefreshRate.Numerator = 60;
	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.OutputWindow = window;
	swapChain.SampleDesc.Count = 1;
	swapChain.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
	swapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChain.Windowed = TRUE;
	swapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	UINT m_DeviceFlags = 0;

#if _DEBUG
	m_DeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif
	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, m_DeviceFlags, nullptr,
		NULL, D3D11_SDK_VERSION, &swapChain, &s_chain, &device, nullptr, &context);

	s_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_texture);
	hr = device->CreateRenderTargetView(m_texture, nullptr, &renderTargetView);

	device->CreateDeferredContext(0, &defferedContext);

	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool DxEngine::InitializeViewports()
{
	ZeroMemory(&viewPort, sizeof(viewPort));
	viewPort.Width = BACKBUFFER_WIDTH;
	viewPort.Height = BACKBUFFER_HEIGHT;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	ZeroMemory(&m_viewPort, sizeof(m_viewPort));
	m_viewPort.Width = BACKBUFFER_WIDTH / 2;
	m_viewPort.Height = BACKBUFFER_HEIGHT / 2;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	return true;
}

bool DxEngine::InitializeVertexandIndexBuffers()
{
	//HRESULT hr;

	UINT m_cubeverts[36];
	vector<VertexBuffer> m_modelVector;
	vector<unsigned int> m_modelIndex;
	VertexBuffer m_cube;
	vector<VertexBuffer> m_cubeVector;
	vector<VertexBuffer> m_planeVector;
	vector<unsigned int> m_planeIndex;

	vector<VertexBuffer> m_torchVector;
	vector<unsigned int> m_torchIndex;

	LoadObject("Alientree.obj", &m_modelVector, &m_modelIndex);
	//LoadObject("talon.obj", &m_modelVector, &m_modelIndex);

	D3D11_BUFFER_DESC m_vertBuffer;
	ZeroMemory(&m_vertBuffer, sizeof(m_vertBuffer));
	m_vertBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_vertBuffer.ByteWidth = (UINT)(sizeof(VertexBuffer) * m_modelVector.size());
	m_vertBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA m_vertData;
	ZeroMemory(&m_vertData, sizeof(m_vertData));
	m_vertData.pSysMem = &m_modelVector[0];
	device->CreateBuffer(&m_vertBuffer, &m_vertData, &m_VertBuffer);

	D3D11_BUFFER_DESC m_indexBuffer;
	ZeroMemory(&m_indexBuffer, sizeof(m_indexBuffer));
	m_indexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_indexBuffer.ByteWidth = UINT(sizeof(unsigned int) * m_modelIndex.size());
	m_indexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA m_indexData;
	ZeroMemory(&m_indexData, sizeof(m_indexData));
	m_indexData.pSysMem = &m_modelIndex[0];
	device->CreateBuffer(&m_indexBuffer, &m_indexData, &m_IndexBuffer);
	numverts = (UINT)m_modelIndex.size();

	LoadObject("MyCube.obj", &m_planeVector, &m_planeIndex);

	D3D11_BUFFER_DESC vertBuffer;
	ZeroMemory(&vertBuffer, sizeof(vertBuffer));
	vertBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBuffer.ByteWidth = UINT(sizeof(VertexBuffer) * m_planeVector.size());
	vertBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA vertData;
	ZeroMemory(&vertData, sizeof(vertData));
	vertData.pSysMem = &m_planeVector[0];
	device->CreateBuffer(&vertBuffer, &vertData, &plane_VertBuffer);

	D3D11_BUFFER_DESC indexBuffer;
	ZeroMemory(&indexBuffer, sizeof(indexBuffer));
	indexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffer.ByteWidth = UINT(sizeof(unsigned int) * m_planeIndex.size());
	indexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = &m_planeIndex[0];
	device->CreateBuffer(&indexBuffer, &indexData, &plane_IndexBuffer);

	LoadObject("torch.obj", &m_torchVector, &m_torchIndex);

	ZeroMemory(&vertBuffer, sizeof(vertBuffer));
	vertBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBuffer.ByteWidth = UINT(sizeof(VertexBuffer) * m_torchVector.size());
	vertBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	ZeroMemory(&vertData, sizeof(vertData));
	vertData.pSysMem = &m_torchVector[0];
	device->CreateBuffer(&vertBuffer, &vertData, &m_torchVertBuffer);
	torchVerts = (UINT)m_torchVector.size();

	/// BLAH
	m_cube.COORD[0] = -0.25;
	m_cube.COORD[1] = 0.25;
	m_cube.COORD[2] = -0.25;
	m_cube.UV[0] = 0;
	m_cube.UV[1] = 0;
	m_cubeVector.push_back(m_cube);


	m_cube.COORD[0] = 0.25;
	m_cube.COORD[1] = 0.25;
	m_cube.COORD[2] = -0.25;
	m_cube.UV[0] = 1;
	m_cube.UV[1] = 0;
	m_cubeVector.push_back(m_cube);

	m_cube.COORD[0] = -0.25;
	m_cube.COORD[1] = -0.25;
	m_cube.COORD[2] = -0.25;
	m_cube.UV[0] = 0;
	m_cube.UV[1] = 1;
	m_cubeVector.push_back(m_cube);

	m_cube.COORD[0] = 0.25;
	m_cube.COORD[1] = -0.25;
	m_cube.COORD[2] = -0.25;
	m_cube.UV[0] = 1;
	m_cube.UV[1] = 1;
	m_cubeVector.push_back(m_cube);

	m_cube.COORD[0] = -0.25;
	m_cube.COORD[1] = 0.25;
	m_cube.COORD[2] = 0.25;
	m_cube.UV[0] = 0;
	m_cube.UV[1] = 0;
	m_cubeVector.push_back(m_cube);

	m_cube.COORD[0] = 0.25;
	m_cube.COORD[1] = 0.25;
	m_cube.COORD[2] = 0.25;
	m_cube.UV[0] = 1;
	m_cube.UV[1] = 0;
	m_cubeVector.push_back(m_cube);

	m_cube.COORD[0] = -0.25;
	m_cube.COORD[1] = -0.25;
	m_cube.COORD[2] = 0.25;
	m_cube.UV[0] = 0;
	m_cube.UV[1] = 1;
	m_cubeVector.push_back(m_cube);

	m_cube.COORD[0] = 0.25;
	m_cube.COORD[1] = -0.25;
	m_cube.COORD[2] = 0.25;
	m_cube.UV[0] = 1;
	m_cube.UV[1] = 1;
	m_cubeVector.push_back(m_cube);

	m_cubeverts[0] = 3;
	m_cubeverts[1] = 1;
	m_cubeverts[2] = 0;

	m_cubeverts[3] = 2;
	m_cubeverts[4] = 3;
	m_cubeverts[5] = 0;

	m_cubeverts[6] = 7;
	m_cubeverts[7] = 5;
	m_cubeverts[8] = 1;

	m_cubeverts[9] = 3;
	m_cubeverts[10] = 7;
	m_cubeverts[11] = 1;

	m_cubeverts[12] = 6;
	m_cubeverts[13] = 4;
	m_cubeverts[14] = 5;

	m_cubeverts[15] = 7;
	m_cubeverts[16] = 6;
	m_cubeverts[17] = 5;

	m_cubeverts[18] = 2;
	m_cubeverts[19] = 0;
	m_cubeverts[20] = 4;

	m_cubeverts[21] = 6;
	m_cubeverts[22] = 2;
	m_cubeverts[23] = 4;

	m_cubeverts[24] = 1;
	m_cubeverts[25] = 5;
	m_cubeverts[26] = 4;

	m_cubeverts[27] = 0;
	m_cubeverts[28] = 1;
	m_cubeverts[29] = 4;

	m_cubeverts[30] = 7;
	m_cubeverts[31] = 3;
	m_cubeverts[32] = 2;

	m_cubeverts[33] = 6;
	m_cubeverts[34] = 7;
	m_cubeverts[35] = 2;


	ZeroMemory(&vertBuffer, sizeof(vertBuffer));
	vertBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBuffer.ByteWidth = sizeof(VertexBuffer) * 8;
	vertBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	ZeroMemory(&vertData, sizeof(vertData));
	vertData.pSysMem = &m_cubeVector[0];
	device->CreateBuffer(&vertBuffer, &vertData, &VertBuffer);

	ZeroMemory(&indexBuffer, sizeof(indexBuffer));
	indexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffer.ByteWidth = sizeof(UINT) * 36;
	indexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = m_cubeverts;
	device->CreateBuffer(&indexBuffer, &indexData, &IndexBuffer);

	Model load;
	load.pos[0] = 0;
	load.pos[1] = 0;
	load.pos[2] = 0;
	load.rotate[0] = 0;
	load.rotate[1] = 0;
	load.rotate[2] = 0;

	Matrix world = MatrixMatrixMultipy(BuildRotationMatrixOnAxisY(0), Translate(load.pos[0], load.pos[1], load.pos[2]));
	world = Scale(world, 64, 64, 64);

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			load.SV_WorldMatrix[i][z] = world.vertex[i][z];
		}
	}
	m_model.push_back(load);

	load.pos[0] = 0.5;
	load.pos[1] = -1;
	load.pos[2] = 0.25;

	/*load.pos[0] = 1;
	load.pos[1] = 0;
	load.pos[2] = 1;*/
	load.rotate[0] = 0;
	load.rotate[1] = 0;
	load.rotate[2] = 0;

	world = MatrixMatrixMultipy(Translate(load.pos[0], load.pos[1], load.pos[2]), BuildRotationMatrixOnAxisX(0));
	world = Scale(world, 0.25f, 0.25f, 0.25f);
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			load.SV_WorldMatrix[i][z] = world.vertex[i][z];
		}
	}
	m_model.push_back(load);


	load.pos[0] = 150;
	load.pos[1] = -1;
	load.pos[2] = 60;
	load.rotate[0] = 0;
	load.rotate[1] = 180;
	load.rotate[2] = 0;

	world = MatrixMatrixMultipy(Translate(load.pos[0], load.pos[1], load.pos[2]), BuildRotationMatrixOnAxisX(0));
	world = Scale(world, 64, 0.1f, 64);

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			load.SV_WorldMatrix[i][z] = world.vertex[i][z];
		}
	}
	m_model.push_back(load);

	return true;
}

bool DxEngine::InitializeShaderResources()
{
	Threading draw;
	draw.d = &device;
	draw.s = &m_shipResource;
	std::thread m_thread(LoadTextures, &draw);
	CreateDDSTextureFromFile(device, L"Castle.dds", NULL, &m_shaderResource);
	CreateDDSTextureFromFile(device, L"DeadGrass.dds", NULL, &m_secondshaderResource);
	CreateDDSTextureFromFile(device, L"torch.dds", NULL, &m_secondshipResource);
	m_thread.join();

	return true;
}

bool DxEngine::InitializeMatrixes()
{
	m_camera.pos[0] = 0;
	m_camera.pos[1] = 0;
	m_camera.pos[2] = 1;

	m_camera.rotate[0] = 0;
	m_camera.rotate[1] = 0;
	m_camera.rotate[2] = 1;

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			m_camera.SV_WorldMatrix[i][z] = BuildIdentityMatrix().vertex[i][z];
		}
	}
	Matrix project = BuildProjectionMatrix(BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT);
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			toShaderWorld.SV_ProjectionMatrix[i][z] = project.vertex[i][z];
		}
	}

	WorldSpaceCamera = BuildIdentityMatrix();
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			toShaderWorld.SV_ViewMatrix[i][z] = WorldSpaceCamera.vertex[i][z];
		}
	}

	//rotate = BuildIdentityMatrix();
	//rotate.vertex[3][2] = 2;

	return true;
}

bool DxEngine::InitializeVertexShaders()
{
	device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &VS_Shader);
	device->CreateVertexShader(VertexShader, sizeof(VertexShader), NULL, &VS_InstanceShader);
	device->CreateVertexShader(SkyboxVertex, sizeof(SkyboxVertex), NULL, &VS_SkyboxShader);

	return true;
}

bool DxEngine::InitializePixelShaders()
{
	device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &PS_Shader);
	device->CreatePixelShader(PixelShader, sizeof(PixelShader), NULL, &PS_PixelShader);
	device->CreatePixelShader(SkyboxPixel, sizeof(SkyboxPixel), NULL, &PS_SkyboxShader);

	return true;
}

bool DxEngine::InitializeLayout()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC CircleVertLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = device->CreateInputLayout(CircleVertLayout, 4, Trivial_VS, sizeof(Trivial_VS), &layout);
	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool DxEngine::InitializeConstantBuffers()
{
	HRESULT hr;
	D3D11_BUFFER_DESC constbuffDesc;
	ZeroMemory(&constbuffDesc, sizeof(constbuffDesc));
	constbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbuffDesc.ByteWidth = sizeof(Obj);
	constbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	constbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = device->CreateBuffer(&constbuffDesc, nullptr, &constantBuffer);

	Matrix identity[60];
	float move = 0;
	float x = -10.0f;
	for (size_t i = 0; i < 60; i++)
	{
		if (i == 20)
		{
			x = 1.0f;
			move = 0;
		}
		if (i == 40)
		{
			x = 11.0f;
			move = 0;
		}
		identity[i] = Translate(x, -1, 2 + move);
		identity[i] = Scale(identity[i], 0.5f, 0.5f, 0.5f);
		move += 5;
	}
	//identity[0] = Translate(1.0, 0, 2);
	//identity[1] = Translate(1.0, 1.0, 2);

	memcpy_s(ship.instance, sizeof(Matrix) * 60, identity, sizeof(Matrix) * 60);

	D3D11_BUFFER_DESC instBuffer;
	ZeroMemory(&instBuffer, sizeof(instBuffer));
	instBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	instBuffer.ByteWidth = sizeof(Instance);
	instBuffer.Usage = D3D11_USAGE_DYNAMIC;
	instBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA instData;
	ZeroMemory(&instData, sizeof(instData));
	instData.pSysMem = &ship;

	device->CreateBuffer(&instBuffer, &instData, &m_InstanceBuffer);

	D3D11_BUFFER_DESC lightbuffer;
	ZeroMemory(&lightbuffer, sizeof(lightbuffer));
	lightbuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbuffer.ByteWidth = sizeof(LightFrame);
	lightbuffer.Usage = D3D11_USAGE_DYNAMIC;
	lightbuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightbuffer.StructureByteStride = sizeof(LightFrame);

	device->CreateBuffer(&lightbuffer, nullptr, &m_lightBuffer);

	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool DxEngine::InitializeBlendState()
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	device->CreateBlendState(&blendDesc, &m_alphaEnabledBlendState);

	//blendDesc.RenderTarget[0].BlendEnable = false;
	//device->CreateBlendState(&blendDesc, &m_alphaDisabledBlendState);

	return true;
}

bool DxEngine::InitializeDepthStencilView()
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = BACKBUFFER_WIDTH;
	descDepth.Height = BACKBUFFER_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	device->CreateTexture2D(&descDepth, NULL, &pDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(pDepthStencil,
		&descDSV,
		&pDSV);

	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool DxEngine::InitializeRasterizerState()
{
	HRESULT hr;
	D3D11_SAMPLER_DESC SamplerDesc;
	ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	SamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&SamplerDesc, &m_sampler);

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = true;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	hr = device->CreateRasterizerState(&rasterDesc, &m_RasterState);
	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void DxEngine::Resize()
{
	if (once)
	{
		if (fullscreen)
		{
			BOOL full = false;
			s_chain->GetFullscreenState(&full, NULL);
			s_chain->SetFullscreenState(!full, NULL);
			fullscreen = false;
		}
		if (s_chain)
		{
			context->OMSetRenderTargets(0, 0, 0);

			ReleaseCOM(renderTargetView);
			ReleaseCOM(pDSV);
			ReleaseCOM(pDepthStencil);
			ReleaseCOM(m_texture);

			HRESULT hr;
			hr = s_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			DXGI_SWAP_CHAIN_DESC swapC;
			s_chain->GetDesc(&swapC);

			Matrix project = BuildProjectionMatrix((float)swapC.BufferDesc.Width, (float)swapC.BufferDesc.Height);
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t z = 0; z < 4; z++)
				{
					toShaderWorld.SV_ProjectionMatrix[i][z] = project.vertex[i][z];
				}
			}

			D3D11_TEXTURE2D_DESC descDepth;
			ZeroMemory(&descDepth, sizeof(descDepth));
			descDepth.Width = swapC.BufferDesc.Width;
			descDepth.Height = swapC.BufferDesc.Height;
			descDepth.MipLevels = 1;
			descDepth.ArraySize = 1;
			descDepth.Format = DXGI_FORMAT_D32_FLOAT;
			descDepth.SampleDesc.Count = 1;
			descDepth.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
			descDepth.Usage = D3D11_USAGE_DEFAULT;
			descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			descDepth.CPUAccessFlags = 0;
			descDepth.MiscFlags = 0;
			device->CreateTexture2D(&descDepth, NULL, &pDepthStencil);

			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(descDSV));
			descDSV.Format = DXGI_FORMAT_D32_FLOAT;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			descDSV.Texture2D.MipSlice = 0;

			device->CreateDepthStencilView(pDepthStencil,
				&descDSV,
				&pDSV);

			ID3D11Texture2D* pBuffer;
			hr = s_chain->GetBuffer(0, __uuidof(ID3D11Texture2D),
				(LPVOID*)&pBuffer);

			hr = device->CreateRenderTargetView(pBuffer, NULL,
				&renderTargetView);
			pBuffer->Release();

			context->OMSetRenderTargets(1, &renderTargetView, NULL);

			viewPort.Width = (float)swapC.BufferDesc.Width;
			viewPort.Height = (float)swapC.BufferDesc.Height;
			viewPort.MinDepth = 0.0f;
			viewPort.MaxDepth = 1.0f;
			viewPort.TopLeftX = 0;
			viewPort.TopLeftY = 0;
			context->RSSetViewports(0, &viewPort);

			m_viewPort.Width = (float)(swapC.BufferDesc.Width / 2);
			m_viewPort.Height = (float)(swapC.BufferDesc.Height / 2);
			m_viewPort.MinDepth = 0.0f;
			m_viewPort.MaxDepth = 1.0f;
			m_viewPort.TopLeftX = 0;
			m_viewPort.TopLeftY = 0;
			context->RSSetViewports(1, &m_viewPort);
		}
		once = false;
	}
	return;
}

bool DxEngine::Initialize()
{
	HRESULT hr;
	hr = InitializeViewports();
	if (FAILED(hr))
	{
		return false;
	}

	hr = InitializeVertexandIndexBuffers();
	if (FAILED(hr))
	{
		return false;
	}

	hr = InitializeLayout();
	if (FAILED(hr))
	{
		return false;
	}

	hr = InitializeVertexShaders();
	if (FAILED(hr))
	{
		return false;
	}

	hr = InitializePixelShaders();
	if (FAILED(hr))
	{
		return false;
	}

	hr = InitializeConstantBuffers();
	if (FAILED(hr))
	{
		return false;
	}

	hr = InitializeBlendState();
	if (FAILED(hr))
	{
		return false;
	}

	hr = InitializeRasterizerState();
	if (FAILED(hr))
	{
		return false;
	}

	hr = InitializeDepthStencilView();
	if (FAILED(hr))
	{
		return false;
	}

	hr = InitializeShaderResources();
	if (FAILED(hr))
	{
		return false;
	}

	hr = InitializeMatrixes();
	if (FAILED(hr))
	{
		return false;
	}

	InitializeLight();

	return true;
}


void DxEngine::Draw()
{
	timer.Signal();
	//Camera Movement
#if 1
	//	Matrix cam = MatrixMatrixMultipy(Translate(m_camera.pos[0], m_camera.pos[1], m_camera.pos[2]), Translate(m_camera.rotate[0], m_camera.rotate[1], m_camera.rotate[2]));
	Matrix cam = MatrixMatrixMultipy(Translate(m_camera.pos[0], m_camera.pos[1], m_camera.pos[2]), BuildRotationMatrixOnAxisY(0));
	Matrix multcam = MatrixMatrixMultipy(Translate(m_model[1].pos[0], m_model[1].pos[1], m_model[1].pos[2]), BuildRotationMatrixOnAxisY(0));
	multcam = Scale(multcam, 0.25f, 0.25f, 0.25f);
	
	Matrix temp;
	float temppos[3];
	temppos[0] = m_camera.SV_WorldMatrix[3][0];
	m_camera.SV_WorldMatrix[3][0] = 0;
	temppos[1] = m_camera.SV_WorldMatrix[3][1];
	m_camera.SV_WorldMatrix[3][1] = 0;
	temppos[2] = m_camera.SV_WorldMatrix[3][2];
	m_camera.SV_WorldMatrix[3][2] = 0;

	Matrix temp1;
	float temppos1[3];
	temppos1[0] = m_model[1].SV_WorldMatrix[3][0];
	m_model[1].SV_WorldMatrix[3][0] = 0;
	temppos1[1] = m_model[1].SV_WorldMatrix[3][1];
	m_model[1].SV_WorldMatrix[3][1] = 0;
	temppos1[2] = m_model[1].SV_WorldMatrix[3][2];
	m_model[1].SV_WorldMatrix[3][2] = 0;

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			temp.vertex[i][z] = m_camera.SV_WorldMatrix[i][z];
			temp1.vertex[i][z] = m_model[1].SV_WorldMatrix[i][z];
		}
	}

	Matrix x = BuildRotationMatrixOnAxisX(m_camera.rotate[0]);
	Matrix y = BuildRotationMatrixOnAxisY(m_camera.rotate[1]);
	Matrix x1 = BuildRotationMatrixOnAxisX(-m_camera.rotate[0]);
	Matrix y2 = BuildRotationMatrixOnAxisY(-m_camera.rotate[1]);
	Matrix r = MatrixMatrixMultipy(x, temp);
	Matrix rotation = MatrixMatrixMultipy(x1, temp);
	r = MatrixMatrixMultipy(r, y);
	rotation = MatrixMatrixMultipy(rotation, y2);

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			m_camera.SV_WorldMatrix[i][z] = r.vertex[i][z];
			m_model[1].SV_WorldMatrix[i][z] = rotation.vertex[i][z];
		}
	}
	m_camera.SV_WorldMatrix[3][0] = temppos[0];
	m_camera.SV_WorldMatrix[3][1] = temppos[1];
	m_camera.SV_WorldMatrix[3][2] = temppos[2];

	m_model[1].SV_WorldMatrix[3][0] = temppos1[0];
	m_model[1].SV_WorldMatrix[3][1] = temppos1[1];
	m_model[1].SV_WorldMatrix[3][2] = temppos1[2];

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			m_camera.SV_WorldMatrix[i][z] = MatrixMatrixMultipy(cam, r).vertex[i][z];
			m_model[1].SV_WorldMatrix[i][z] = MatrixMatrixMultipy(multcam, rotation).vertex[i][z];
		}
	}

	Matrix inv;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			inv.vertex[i][z] = m_camera.SV_WorldMatrix[i][z];
		}
	}
	//inv = Inverse(inv);

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			toShaderWorld.SV_ViewMatrix[i][z] = inv.vertex[i][z];
		}
	} // Camera Movement
	m_point.pos[0] = m_model[1].SV_WorldMatrix[3][0];
	m_point.pos[1] = m_model[1].SV_WorldMatrix[3][1] + 2;
	m_point.pos[2] = m_model[1].SV_WorldMatrix[3][2] -0.75;

#endif

	LightFrame pointLight;
	pointLight.point = m_point;
	D3D11_MAPPED_SUBRESOURCE light;
	ZeroMemory(&light, sizeof(light));
	context->Map(m_lightBuffer, NULL, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &light);
	memcpy(light.pData, &pointLight, sizeof(pointLight));
	context->Unmap(m_lightBuffer, NULL);
	context->PSSetConstantBuffers(0, 1, &m_lightBuffer);

	//Deferred Context Thread
#if 1
	Threading drawPlane;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			toShaderWorld.SV_WorldMatrix[i][z] = m_model[2].SV_WorldMatrix[i][z];
		}
	}

	drawPlane.cmd = &command;
	drawPlane.cont = &defferedContext;
	drawPlane.lay = &layout;
	drawPlane.PS = &PS_Shader;
	drawPlane.srv = &m_secondshaderResource;
	drawPlane.stride = sizeof(VertexBuffer);
	drawPlane.vert = &plane_VertBuffer;
	drawPlane.VS = &VS_Shader;
	drawPlane.RTV = &renderTargetView;
	drawPlane.DSV = &pDSV;
	drawPlane.view = &viewPort;
	drawPlane.sampler = &m_sampler;
	drawPlane.constant = &constantBuffer;
	drawPlane.toShader = &toShaderWorld;

	//std::thread m_draw(DrawOnThread, &drawPlane);
 // 
#endif

	D3D11_MAPPED_SUBRESOURCE camData;
	ZeroMemory(&camData, sizeof(camData));

	context->Map(constantBuffer, NULL, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &camData);
	memcpy(camData.pData, &toShaderWorld, sizeof(toShaderWorld));
	context->Unmap(constantBuffer, NULL);
	context->VSSetConstantBuffers(1, 1, &constantBuffer);


	Resize();

	context->OMSetRenderTargets(1, &renderTargetView, pDSV);
	context->RSSetViewports(1, &viewPort);

	float m_color[4] = { 1.0f, 1.0f, 1.0f, 1 };
	context->ClearRenderTargetView(renderTargetView, m_color);
	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;



	context->PSSetShaderResources(0, 1, &m_shaderResource);

	context->PSSetSamplers(0, 1, &m_sampler);

	//context->OMSetBlendState(m_alphaEnabledBlendState, blendFactor, 0xFFFFFFFF);

	//Draw Skybox
#if 1
	m_model[0].pos[0] = -m_camera.pos[0];
	m_model[0].pos[1] = -m_camera.pos[1];
	m_model[0].pos[2] = -m_camera.pos[2];
	m_model[0].rotate[0] = -m_camera.rotate[0];
	m_model[0].rotate[1] = -m_camera.rotate[1];
	m_model[0].rotate[2] = -m_camera.rotate[2];

	Matrix mult = MatrixMatrixMultipy(Translate(m_model[0].pos[0], m_model[0].pos[1], m_model[0].pos[2]), BuildRotationMatrixOnAxisY(m_model[0].rotate[1]));

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			m_model[0].SV_WorldMatrix[i][z] = mult.vertex[i][z];
		}
	}

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			toShaderWorld.SV_WorldMatrix[i][z] = m_model[0].SV_WorldMatrix[i][z];

		}
	}
	D3D11_MAPPED_SUBRESOURCE objData;
	ZeroMemory(&objData, sizeof(objData));

	context->Map(constantBuffer, NULL, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &objData);
	memcpy(objData.pData, &toShaderWorld, sizeof(toShaderWorld));
	context->Unmap(constantBuffer, NULL);
	context->VSSetConstantBuffers(1, 1, &constantBuffer);

	UINT stride = sizeof(VertexBuffer);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &VertBuffer, &stride, &offset);
	context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->VSSetShader(VS_SkyboxShader, 0, 0);
	context->PSSetShader(PS_SkyboxShader, 0, 0);
	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->DrawIndexed(36, 0, 0);
#endif 

	// Draw Torch
#if 1
	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			toShaderWorld.SV_WorldMatrix[i][z] = m_model[1].SV_WorldMatrix[i][z];
		}
	}
	D3D11_MAPPED_SUBRESOURCE objData2;
	ZeroMemory(&objData2, sizeof(objData2));

	context->Map(constantBuffer, NULL, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &objData2);
	memcpy(objData2.pData, &toShaderWorld, sizeof(toShaderWorld));
	context->Unmap(constantBuffer, NULL);

	//if (!change)
	//{
	//	context->PSSetShaderResources(0, 1, &m_shipResource);
	//}
	//else
	//{
	//	context->PSSetShaderResources(0, 1, &m_secondshipResource);
	//}
	//context->VSSetShader(VS_InstanceShader, NULL, 0);
	context->PSSetShaderResources(0, 1, &m_secondshipResource);
	context->IASetVertexBuffers(0, 1, &m_torchVertBuffer, &stride, &offset);
	context->VSSetShader(VS_Shader, 0, 0);
	context->PSSetShader(PS_Shader, 0, 0);
	//context->VSSetShader(VS_InstanceShader, 0, 0);
	//context->PSSetShader(PS_PixelShader, 0, 0);
	//context->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->Draw(torchVerts, 0);

#endif

	// Draw Plane
#if 1
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			toShaderWorld.SV_WorldMatrix[i][z] = m_model[2].SV_WorldMatrix[i][z];
		}
	}
	D3D11_MAPPED_SUBRESOURCE objData3;
	ZeroMemory(&objData3, sizeof(objData3));

	context->Map(constantBuffer, NULL, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &objData3);
	memcpy(objData3.pData, &toShaderWorld, sizeof(toShaderWorld));
	context->Unmap(constantBuffer, NULL);


	//context->VSSetShader(VS_SkyboxShader, 0, 0);
	context->VSSetShader(VS_Shader, 0, 0);
	context->PSSetShader(PS_Shader, 0, 0);
	context->PSSetShaderResources(0, 1, &m_secondshaderResource);
	context->IASetVertexBuffers(0, 1, &plane_VertBuffer, &stride, &offset);
	context->IASetIndexBuffer(plane_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->Draw(36, 0);
	/*m_draw.join();
	context->ExecuteCommandList(command, false);
	command->Release();*/

	//context->PSSetShaderResources(0, 1, &m_shipResource);
	//context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
#endif

	// Draw Instanced Trees
#if 1
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t z = 0; z < 4; z++)
		{
			toShaderWorld.SV_WorldMatrix[i][z] = m_model[1].SV_WorldMatrix[i][z];
		}
	}
	ZeroMemory(&objData2, sizeof(objData2));

	context->Map(constantBuffer, NULL, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &objData2);
	memcpy(objData2.pData, &toShaderWorld, sizeof(toShaderWorld));
	context->Unmap(constantBuffer, NULL);

	context->PSSetShaderResources(0, 1, &m_shipResource);
	context->IASetVertexBuffers(0, 1, &m_VertBuffer, &stride, &offset);
	context->VSSetConstantBuffers(2, 1, &m_InstanceBuffer);
	context->VSSetShader(VS_InstanceShader, 0, 0);

	context->DrawInstanced(numverts, 60, 0, 0);


	context->RSSetViewports(1, &m_viewPort);
#endif

	//Viewport stuff

	//	context->PSSetShaderResources(0, 1, &m_shipResource);
	//	context->IASetVertexBuffers(0, 1, &m_VertBuffer, &stride, &offset);
	//	context->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//	context->VSSetConstantBuffers(2, 1, &m_InstanceBuffer);
	//	context->VSSetShader(VS_InstanceShader, 0, 0);
	//
	//	context->DrawInstanced(numverts, 2, 0, 0);


	context->RSSetState(m_RasterState);

	m_camera.rotate[0] = 0;
	m_camera.rotate[1] = 0;
	m_camera.rotate[2] = 0;

	s_chain->Present(0, 0);
}

void DxEngine::TranslateCamera(UINT sub, float distance)
{
	m_camera.pos[sub] += distance;
	m_model[1].pos[sub] -= distance;
}

void DxEngine::RotateCamera(UINT sub, float distance)
{
	m_camera.rotate[sub] += distance;
}

template<typename DX>
void DxEngine::ReleaseCOM(DX& item)
{
	if (item != NULL)
	{
		item->Release();
		item = NULL;
	}
}