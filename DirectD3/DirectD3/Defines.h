#include <Windows.h>
#include <d3d11.h>
#include <WinUser.h>

#pragma comment(lib, "d3d11.lib")

#include <math.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <iostream>
#include <thread>
#include <ctime>
#include <vector>

#include "DDSTextureLoader.h"
#include "XTime.h"



using namespace std;

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
#define PI					3.14159265359

//struct Threading
//{
//	ID3D11Device** d;
//	ID3D11ShaderResourceView** s;
//	ID3D11DeviceContext** context;
//};

//void ThreadDraw(Threading *thread)
//{
//	(*thread->context)->DrawIndexed(36, 0, 0);
//}

//void LoadTextures(Threading *that)
//{
//	CreateDDSTextureFromFile(*that->d, L"lava_seamless.dds", NULL, *&that->s);
//}