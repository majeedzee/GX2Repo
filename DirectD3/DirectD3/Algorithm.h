#pragma once
#include "Defines.h"



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
Matrix Translate(float x, float y, float z);
Matrix BuildRotationMatrixOnAxisX(double  m_angle);
float ConvertDegreestoRadians(float m_angle);
Matrix Scale(Matrix scale, float value);
Matrix BuildProjectionMatrix(float width, float height);
Matrix Inverse(Matrix inv);
unsigned int InterpolateTwoColors(unsigned int color1, unsigned int color2, float ratio);

//Vertex UV_Coords;

