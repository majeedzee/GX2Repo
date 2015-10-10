#include "Algorithm.h"

void Draw(unsigned int m_pos, unsigned int m_color/*, unsigned int m_zval = 1*/)
{
	//Backbuffer[m_pos] = m_color;
	//zbuffer[m_pos] = m_zval;
}

unsigned int ConvertCoord(unsigned int x, unsigned int y, unsigned int width)
{
	return (y*width) + x;
}

void Bresenham(Line m_line)
{
	int CurrX = m_line.StartX;
	int CurrY = m_line.StartY;
	int m_incrementer = 0;
	int deltax = abs((int)m_line.EndX - (int)m_line.StartX);
	int deltay = abs((int)m_line.EndY - (int)m_line.StartY);
	int distance = 0;
	if (deltax > deltay)
	{
		distance = 0;
	}
	else if (deltax < deltay)
	{
		distance = 1;
	}
	if (m_line.StartY > m_line.EndY)
	{
		m_incrementer = -1;
	}
	else if ((m_line.StartY < m_line.EndY))
	{
		m_incrementer = 1;
	}
	float Slope = ((float)m_line.EndY - (float)m_line.StartY) / ((float)m_line.EndX - (float)m_line.StartX);
	float Error = (float)deltax * 0.5;

	do
	{
		//Draw(ConvertCoord(CurrX, CurrY, RASTER_WIDTH), 0x00FF0000);
		if (distance == 0)
		{
			Draw(ConvertCoord(CurrX, CurrY, BACKBUFFER_WIDTH), 0x00FF0000);
		}
		else
		{
			Draw(ConvertCoord(CurrY, CurrX, BACKBUFFER_WIDTH), 0x00FF0000);
		}
		Error -= deltay;
		if (Error < 0.0)
		{
			CurrY += m_incrementer;
			Error -= deltax;
		}
		CurrX += 1;
	} while (CurrX < m_line.EndX);

	Draw(ConvertCoord(m_line.StartX, m_line.StartY, BACKBUFFER_WIDTH), 0xFFFFFF00);
	Draw(ConvertCoord(m_line.EndX, m_line.EndY, BACKBUFFER_WIDTH), 0xFFFFFF00);
}

void Midline(Line m_line)
{
	int CurrY = m_line.StartY;
	int CurrX = m_line.StartX;
	float Midpoint[2] = { 0, 0 };
	do
	{
		Draw(ConvertCoord(CurrX, CurrY, BACKBUFFER_WIDTH), 0x0000FF00);
		Midpoint[0] = (float)(CurrX + 1);
		Midpoint[1] = (float)(CurrY + 0.5);
		if (((((float)m_line.StartY - (float)m_line.EndY)*Midpoint[0]) + (((float)m_line.EndX - (float)m_line.StartX)*Midpoint[1])
			+ ((float)m_line.StartX * (float)m_line.EndY) - ((float)m_line.StartY * (float)m_line.EndX)) < 0)
		{
			CurrY += 1;
		}
		CurrX += 1;
	} while (CurrX < m_line.EndX);
	Draw(ConvertCoord(m_line.StartX, m_line.StartY, BACKBUFFER_WIDTH), 0xFFFFFF00);
	Draw(ConvertCoord(m_line.EndX, m_line.EndY, BACKBUFFER_WIDTH), 0xFFFFFF00);
}


float ConvertCoordFloats(float x, float y, unsigned int width)
{
	return (y*width) + x;
}

unsigned int ConvertXtoNDC(float x, unsigned int width)
{
	return ((x + 1) * width) *0.5;
}

unsigned int ConvertYtoNDC(float y, unsigned int height)
{
	return (height * 0.5) - y * (height * 0.5);
	//return ((y - 1) * height) * -0.5;
}

void MultiplyVertexByMatrix(Vertex &multiplyMe, Matrix S_WorldMatrix)
{
	Vertex Product;
	Product.m_verticies[0] = (multiplyMe.m_verticies[0] * S_WorldMatrix.vertex[0][0]) + (multiplyMe.m_verticies[1] * S_WorldMatrix.vertex[1][0]) + (multiplyMe.m_verticies[2] * S_WorldMatrix.vertex[2][0]) + (multiplyMe.m_verticies[3] * S_WorldMatrix.vertex[3][0]);
	Product.m_verticies[1] = (multiplyMe.m_verticies[0] * S_WorldMatrix.vertex[0][1]) + (multiplyMe.m_verticies[1] * S_WorldMatrix.vertex[1][1]) + (multiplyMe.m_verticies[2] * S_WorldMatrix.vertex[2][1]) + (multiplyMe.m_verticies[3] * S_WorldMatrix.vertex[3][1]);
	Product.m_verticies[2] = (multiplyMe.m_verticies[0] * S_WorldMatrix.vertex[0][2]) + (multiplyMe.m_verticies[1] * S_WorldMatrix.vertex[1][2]) + (multiplyMe.m_verticies[2] * S_WorldMatrix.vertex[2][2]) + (multiplyMe.m_verticies[3] * S_WorldMatrix.vertex[3][2]);
	Product.m_verticies[3] = (multiplyMe.m_verticies[0] * S_WorldMatrix.vertex[0][3]) + (multiplyMe.m_verticies[1] * S_WorldMatrix.vertex[1][3]) + (multiplyMe.m_verticies[2] * S_WorldMatrix.vertex[2][3]) + (multiplyMe.m_verticies[3] * S_WorldMatrix.vertex[3][3]);

	Product.m_color = multiplyMe.m_color;
	//Product.U_coord = multiplyMe.U_coord;
	//Product.V_coord = multiplyMe.V_coord;
	multiplyMe = Product;
}

Matrix BuildRotationMatrixOnAxisZ(double m_angle)
{
	Matrix rotate;
	rotate.vertex[0][0] = cos(m_angle);
	rotate.vertex[0][1] = -sin(m_angle);
	rotate.vertex[0][2] = 0;
	rotate.vertex[1][0] = sin(m_angle);
	rotate.vertex[1][1] = cos(m_angle);
	rotate.vertex[1][2] = 0;
	rotate.vertex[2][0] = 0;
	rotate.vertex[2][1] = 0;
	rotate.vertex[2][2] = 1;
	return rotate;
}

Matrix BuildRotationMatrixOnAxisY(float  m_angle)
{
	Matrix rotate = {};
	rotate.vertex[0][0] = cos(m_angle);
	rotate.vertex[0][1] = 0;
	rotate.vertex[0][2] = sin(m_angle);
	rotate.vertex[0][3] = 0;

	rotate.vertex[1][0] = 0;
	rotate.vertex[1][1] = 1;
	rotate.vertex[1][2] = 0;
	rotate.vertex[1][3] = 0;

	rotate.vertex[2][0] = -sin(m_angle);
	rotate.vertex[2][1] = 0;
	rotate.vertex[2][2] = cos(m_angle);
	rotate.vertex[2][3] = 0;

	rotate.vertex[3][0] = 0;
	rotate.vertex[3][1] = 0;
	rotate.vertex[3][2] = 0;
	rotate.vertex[3][3] = 1;

	rotate.vertex[0][0] = rotate.vertex[2][2];
	return rotate;
}

float GetIncreasingRadiansOverTime()
{
	//return timer.TotalTime();
	return 0.0f;
}

float ImplicitLine(float X1, float Y1, float X2, float Y2, float x, float y)
{
	return ((Y1 - Y2) *x) + ((X2 - X1) * y) + (X1 * Y2) - (Y1 * X2);
}

Matrix BuildIdentityMatrix()
{
	Matrix Identity;
	Identity.vertex[0][0] = 1;
	Identity.vertex[0][1] = 0;
	Identity.vertex[0][2] = 0;
	Identity.vertex[0][3] = 0;
	Identity.vertex[1][0] = 0;
	Identity.vertex[1][1] = 1;
	Identity.vertex[1][2] = 0;
	Identity.vertex[1][3] = 0;
	Identity.vertex[2][0] = 0;
	Identity.vertex[2][1] = 0;
	Identity.vertex[2][2] = 1;
	Identity.vertex[2][3] = 0;
	Identity.vertex[3][0] = 0;
	Identity.vertex[3][1] = 0;
	Identity.vertex[3][2] = 0;
	Identity.vertex[3][3] = 1;
	return Identity;
}

Matrix MatrixMatrixMultipy(Matrix m1, Matrix m2)
{
	Matrix mult;
	mult.vertex[0][0] = (m1.vertex[0][0] * m2.vertex[0][0]) + (m1.vertex[0][1] * m2.vertex[1][0]) + (m1.vertex[0][2] * m2.vertex[2][0]) + (m1.vertex[0][3] * m2.vertex[3][0]);
	mult.vertex[0][1] = (m1.vertex[0][0] * m2.vertex[0][1]) + (m1.vertex[0][1] * m2.vertex[1][1]) + (m1.vertex[0][2] * m2.vertex[2][1]) + (m1.vertex[0][3] * m2.vertex[3][1]);
	mult.vertex[0][2] = (m1.vertex[0][0] * m2.vertex[0][2]) + (m1.vertex[0][1] * m2.vertex[1][2]) + (m1.vertex[0][2] * m2.vertex[2][2]) + (m1.vertex[0][3] * m2.vertex[3][2]);
	mult.vertex[0][3] = (m1.vertex[0][0] * m2.vertex[0][3]) + (m1.vertex[0][1] * m2.vertex[1][3]) + (m1.vertex[0][2] * m2.vertex[2][3]) + (m1.vertex[0][3] * m2.vertex[3][3]);

	mult.vertex[1][0] = (m1.vertex[1][0] * m2.vertex[0][0]) + (m1.vertex[1][1] * m2.vertex[1][0]) + (m1.vertex[1][2] * m2.vertex[2][0]) + (m1.vertex[1][3] * m2.vertex[3][0]);
	mult.vertex[1][1] = (m1.vertex[1][0] * m2.vertex[0][1]) + (m1.vertex[1][1] * m2.vertex[1][1]) + (m1.vertex[1][2] * m2.vertex[2][1]) + (m1.vertex[1][3] * m2.vertex[3][1]);
	mult.vertex[1][2] = (m1.vertex[1][0] * m2.vertex[0][2]) + (m1.vertex[1][1] * m2.vertex[1][2]) + (m1.vertex[1][2] * m2.vertex[2][2]) + (m1.vertex[1][3] * m2.vertex[3][2]);
	mult.vertex[1][3] = (m1.vertex[1][0] * m2.vertex[0][3]) + (m1.vertex[1][1] * m2.vertex[1][3]) + (m1.vertex[1][2] * m2.vertex[2][3]) + (m1.vertex[1][3] * m2.vertex[3][3]);

	mult.vertex[2][0] = (m1.vertex[2][0] * m2.vertex[0][0]) + (m1.vertex[2][1] * m2.vertex[1][0]) + (m1.vertex[2][2] * m2.vertex[2][0]) + (m1.vertex[2][3] * m2.vertex[3][0]);
	mult.vertex[2][1] = (m1.vertex[2][0] * m2.vertex[0][1]) + (m1.vertex[2][1] * m2.vertex[1][1]) + (m1.vertex[2][2] * m2.vertex[2][1]) + (m1.vertex[2][3] * m2.vertex[3][1]);
	mult.vertex[2][2] = (m1.vertex[2][0] * m2.vertex[0][2]) + (m1.vertex[2][1] * m2.vertex[1][2]) + (m1.vertex[2][2] * m2.vertex[2][2]) + (m1.vertex[2][3] * m2.vertex[3][2]);
	mult.vertex[2][3] = (m1.vertex[2][0] * m2.vertex[0][3]) + (m1.vertex[2][1] * m2.vertex[1][3]) + (m1.vertex[2][2] * m2.vertex[2][3]) + (m1.vertex[2][3] * m2.vertex[3][3]);

	mult.vertex[3][0] = (m1.vertex[3][0] * m2.vertex[0][0]) + (m1.vertex[3][1] * m2.vertex[1][0]) + (m1.vertex[3][2] * m2.vertex[2][0]) + (m1.vertex[3][3] * m2.vertex[3][0]);
	mult.vertex[3][1] = (m1.vertex[3][0] * m2.vertex[0][1]) + (m1.vertex[3][1] * m2.vertex[1][1]) + (m1.vertex[3][2] * m2.vertex[2][1]) + (m1.vertex[3][3] * m2.vertex[3][1]);
	mult.vertex[3][2] = (m1.vertex[3][0] * m2.vertex[0][2]) + (m1.vertex[3][1] * m2.vertex[1][2]) + (m1.vertex[3][2] * m2.vertex[2][2]) + (m1.vertex[3][3] * m2.vertex[3][2]);
	mult.vertex[3][3] = (m1.vertex[3][0] * m2.vertex[0][3]) + (m1.vertex[3][1] * m2.vertex[1][3]) + (m1.vertex[3][2] * m2.vertex[2][3]) + (m1.vertex[3][3] * m2.vertex[3][3]);

	return mult;

}

Matrix Translate(float x, float y, float z)
{
	Matrix trans = {};
	trans.vertex[0][0] = 1;
	trans.vertex[1][1] = 1;
	trans.vertex[2][2] = 1;
	trans.vertex[3][3] = 1;

	trans.vertex[3][0] = x;
	trans.vertex[3][1] = y;
	trans.vertex[3][2] = z;
	return trans;
}

Matrix BuildRotationMatrixOnAxisX(double  m_angle)
{
	Matrix rotate;
	rotate.vertex[0][0] = 1;
	rotate.vertex[0][1] = 0;
	rotate.vertex[0][2] = 0;
	rotate.vertex[0][3] = 0;

	rotate.vertex[1][0] = 0;
	rotate.vertex[1][1] = cos(m_angle);
	rotate.vertex[1][2] = -sin(m_angle);
	rotate.vertex[1][3] = 0;

	rotate.vertex[2][0] = 0;
	rotate.vertex[2][1] = sin(m_angle);
	rotate.vertex[2][2] = cos(m_angle);
	rotate.vertex[2][3] = 0;

	rotate.vertex[3][0] = 0;
	rotate.vertex[3][1] = 0;
	rotate.vertex[3][2] = 0;
	rotate.vertex[3][3] = 1;

	//rotate.vertex[0][0] = rotate.vertex[2][2];
	return rotate;
}

float ConvertDegreestoRadians(float m_angle)
{
	return m_angle * PI / 180.0f;
}

void LoadTextures(Threading *draw)
{
	//CreateDDSTextureFromFile(*draw->d, L"defender.dds", NULL, *&draw->s);
	CreateDDSTextureFromFile(*draw->d, L"AlienTree.dds", NULL, *&draw->s);
}

void DrawOnThread(Threading *draw)
{
	UINT offset = 0;
	(*draw->cont)->OMSetRenderTargets(1, draw->RTV, *draw->DSV);
	(*draw->cont)->RSSetViewports(1, draw->view);

	//float m_color[4] = { 1.0f, 1.0f, 1.0f, 1 };
	//(*draw->cont)->ClearRenderTargetView(*draw->RTV, m_color);
	//(*draw->cont)->ClearDepthStencilView(*draw->DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	(*draw->cont)->PSSetSamplers(0, 1, draw->sampler);

	D3D11_MAPPED_SUBRESOURCE objData3;
	ZeroMemory(&objData3, sizeof(objData3));

	(*draw->cont)->Map(*draw->constant, NULL, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &objData3);
	memcpy(objData3.pData, draw->toShader, sizeof(*draw->toShader));
	(*draw->cont)->Unmap(*draw->constant, NULL);

	(*draw->cont)->IASetVertexBuffers(0, 1, draw->vert, &draw->stride, &offset);
	//(*draw->cont)->IASetIndexBuffer(*draw->index, DXGI_FORMAT_R32_UINT, 0);
	(*draw->cont)->VSSetShader(*draw->VS, 0, 0);
	(*draw->cont)->PSSetShader(*draw->PS, 0, 0);
	(*draw->cont)->IASetInputLayout(*draw->lay);
	(*draw->cont)->PSSetShaderResources(0, 1, draw->srv);
	(*draw->cont)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	(*draw->cont)->Draw(36, 0);
	(*draw->cont)->FinishCommandList(true, draw->cmd);
}

Matrix Scale(Matrix scale, float x, float y, float z)
{
	Matrix temp = {};
	scale.vertex[0][0] *= x;
	scale.vertex[1][1] *= y;
	scale.vertex[2][2] *= z;

	temp = scale;
	return temp;
}
Matrix BuildProjectionMatrix(float width, float height)
{
	float Yscale = 1 / tan(ConvertDegreestoRadians(35));
	float Xscale = Yscale * (width / height);
	Matrix project;
	project.vertex[0][0] = Xscale;
	project.vertex[0][1] = 0;
	project.vertex[0][2] = 0;
	project.vertex[0][3] = 0;

	project.vertex[1][0] = 0;
	project.vertex[1][1] = Yscale;
	project.vertex[1][2] = 0;
	project.vertex[1][3] = 0;

	project.vertex[2][0] = 0;
	project.vertex[2][1] = 0;
	project.vertex[2][2] = 1000 / (1000 - 0.1);
	project.vertex[2][3] = 1;

	project.vertex[3][0] = 0;
	project.vertex[3][1] = 0;
	project.vertex[3][2] = (-1 * (1000 * 0.1)) / (1000 - 0.1);
	project.vertex[3][3] = 0;

	//rotate.vertex[0][0] = rotate.vertex[2][2];
	return project;
}

Matrix Inverse(Matrix inv)
{
	Matrix result;
	result.vertex[0][0] = inv.vertex[0][0];
	result.vertex[1][0] = inv.vertex[0][1];
	result.vertex[2][0] = inv.vertex[0][2];

	result.vertex[0][1] = inv.vertex[1][0];
	result.vertex[1][1] = inv.vertex[1][1];
	result.vertex[2][1] = inv.vertex[1][2];

	result.vertex[0][2] = inv.vertex[2][0];
	result.vertex[1][2] = inv.vertex[2][1];
	result.vertex[2][2] = inv.vertex[2][2];

	result.vertex[0][3] = 0;
	result.vertex[1][3] = 0;
	result.vertex[2][3] = 0;

	Vertex pos;
	pos.m_verticies[0] = inv.vertex[3][0];
	pos.m_verticies[1] = inv.vertex[3][1];
	pos.m_verticies[2] = inv.vertex[3][2];
	pos.m_verticies[3] = 1.0f;

	result.vertex[3][0] = 0;
	result.vertex[3][1] = 0;
	result.vertex[3][2] = 0;
	result.vertex[3][3] = 1;

	MultiplyVertexByMatrix(pos, result);

	pos.m_verticies[0] = -1 * pos.m_verticies[0];
	pos.m_verticies[1] = -1 * pos.m_verticies[1];
	pos.m_verticies[2] = -1 * pos.m_verticies[2];

	Matrix inveresed;
	inveresed = result;
	inveresed.vertex[3][0] = pos.m_verticies[0];
	inveresed.vertex[3][1] = pos.m_verticies[1];
	inveresed.vertex[3][2] = pos.m_verticies[2];
	inveresed.vertex[3][3] = 1.0f;
	return inveresed;
}

unsigned int InterpolateTwoColors(unsigned int color1, unsigned int color2, float ratio)
{
	unsigned char a = (color1 & 0xFF000000) >> 24;
	unsigned char b = (color1 & 0x000000FF);
	unsigned char r = (color1 & 0x00FF0000) >> 16;
	unsigned char g = (color1 & 0x0000FF00) >> 8;

	unsigned char n_a = (color2 & 0xFF000000) >> 24;
	unsigned char n_b = (color2 & 0x000000FF);
	unsigned char n_r = (color2 & 0x00FF0000) >> 16;
	unsigned char n_g = (color2 & 0x0000FF00) >> 8;

	a = (unsigned char)((a - n_a)*ratio + n_a);
	b = (unsigned char)((b - n_b)*ratio + n_b);
	g = (unsigned char)((g - n_g)*ratio + n_g);
	r = (unsigned char)((r - n_r)*ratio + n_r);

	unsigned int m_color = 0;
	m_color |= (unsigned int)a << 24;
	m_color |= (unsigned int)r << 16;
	m_color |= (unsigned int)g << 8;
	m_color |= (unsigned int)b;
	return m_color;
}