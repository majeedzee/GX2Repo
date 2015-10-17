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
#include <string.h>

#include "DDSTextureLoader.h"
#include "XTime.h"



using namespace std;

#define BACKBUFFER_WIDTH	1280
#define BACKBUFFER_HEIGHT	720
#define PI					3.14159265359

//void ThreadDraw(Threading *thread)
//{
//	(*thread->context)->DrawIndexed(36, 0, 0);
//}

