#pragma once

#define BACKBUFFER_WIDTH	1280
#define BACKBUFFER_HEIGHT	720
#define PI					3.14159265359

XTime timer;

struct Vertex
{
	float m_verticies[4];
	unsigned int m_color;
	float U_coord;
	float V_coord;
};

struct Line
{
	int StartY = 100;
	int StartX = 0;
	int EndY = 400;
	int EndX = 499;
};

struct Matrix
{
	// Changed to 4x4
	float vertex[4][4];
};

void Bresenham(Line m_line);
void Midline(Line m_line);
unsigned int ConvertCoord(unsigned int x, unsigned int y, unsigned int width);
float ConvertCoordFloats(float x, float y, unsigned int width);
void Draw(unsigned int m_pos, unsigned int m_color/*, unsigned int m_zval*/);
unsigned int ConvertXtoNDC(float x, unsigned int width);
unsigned int ConvertYtoNDC(float y, unsigned int height);
void MultiplyVertexByMatrix(Vertex& multiplyMe, Matrix S_WorldMatrix);
Matrix BuildRotationMatrixOnAxisZ(double m_angle);
float GetIncreasingRadiansOverTime();
float ImplicitLine(float x1, float y1, float x2, float y2, float x, float y);
Matrix MatrixMatrixMultipy(Matrix m1, Matrix m2);
Matrix BuildIdentityMatrix();
Matrix BuildRotationMatrixOnAxisY(float m_angle);
Matrix Translate(Matrix trans, float x, float y, float z);
Matrix BuildRotationMatrixOnAxisX(double  m_angle);
float ConvertDegreestoRadians(float m_angle);
//void BuildProjectionMatrix(float Xscale, float Yscale, float zFar, float zNear, float &proj[4][4]);
Matrix Inverse(Matrix inv);
unsigned int InterpolateTwoColors(unsigned int color1, unsigned int color2, float ratio);

Vertex UV_Coords;

void Draw(unsigned int m_pos, unsigned int m_color/*, unsigned int m_zval = 1*/)
{
	//Backbuffer[m_pos] = m_color;
	//zbuffer[m_pos] = m_zval;
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


unsigned int ConvertCoord(unsigned int x, unsigned int y, unsigned int width)
{
	return (y*width) + x;
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
	return timer.TotalTime();
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

//void BuildProjectionMatrix(float Xscale, float Yscale, float zFar, float zNear, float &proj[4][4])
//{
//	float project[4][4];
//	project[0][0] = Xscale;
//	project[0][1] = 0;
//	project[0][2] = 0;
//	project[0][3] = 0;
//
//	project[1][0] = 0;
//	project[1][1] = Yscale;
//	project[1][2] = 0;
//	project[1][3] = 0;
//
//	project[2][0] = 0;
//	project[2][1] = 0;
//	project[2][2] = zFar / (zFar - zNear);
//	project[2][3] = 1;
//
//	project[3][0] = 0;
//	project[3][1] = 0;
//	project[3][2] = (-1 * (zFar*zNear)) / (zFar - zNear);
//	project[3][3] = 0;
//
//	//rotate.vertex[0][0] = rotate.vertex[2][2];
//	*proj[1][0] = project[1][0];
//
//}

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