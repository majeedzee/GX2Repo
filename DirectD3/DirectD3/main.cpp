//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************
#include "DxEngine.h"

bool resize = false;

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;

	DxEngine* m_Engine = new DxEngine();

public:

	XTime timer;
	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************
DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	application = hinst;
	appWndProc = proc;


	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"Lab 1a Line Land", WS_OVERLAPPEDWINDOW /*& ~(WS_THICKFRAME | WS_MAXIMIZEBOX)*/,
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);
	ShowWindow(window, SW_SHOW);

	m_Engine->InitializeSwapChain(window);

	m_Engine->Initialize();
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************



bool DEMO_APP::Run()
{
	timer.Signal();
	if (GetAsyncKeyState(VK_UP) & 0x1)
	{
		//WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, Translate(0, 0, 0.05));
		//m_camera.pos[2] += 0.05;
		m_Engine->TranslateCamera(2, (float)(-3 * timer.Delta()));
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x1)
	{
		//WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, Translate(0, 0, -0.05));
		//m_camera.pos[2] -= 0.05;
		m_Engine->TranslateCamera(2, (float)(3 * timer.Delta()));
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x1)
	{
		//WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, Translate(0.05, 0, 0));
		//m_camera.pos[0] += 0.05;
		m_Engine->TranslateCamera(0, (float)(-3 * timer.Delta()));
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x1)
	{
		//WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, Translate(-0.05, 0, 0));
		//m_camera.pos[0] -= 0.05;
		m_Engine->TranslateCamera(0, (float)(3 * timer.Delta()));
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x1)
	{
		//WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, Translate(0, 0.05, 0));
		//m_camera.pos[1] += 0.05;
		m_Engine->TranslateCamera(1, (float)(3 * timer.Delta()));
	}
	if (GetAsyncKeyState(VK_SHIFT) & 0x1)
	{
		//WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, Translate(0, 0.05, 0));
		//m_camera.pos[1] -= 0.05;
		m_Engine->TranslateCamera(1, (float)(-3 * timer.Delta()));
	}

	if (GetAsyncKeyState(VK_NUMPAD4) & 0x1)
	{
		//WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, BuildRotationMatrixOnAxisX(0.05));
		m_Engine->RotateCamera(1, (float)(-3 * timer.Delta()));
	}
	if (GetAsyncKeyState(VK_NUMPAD6) & 0x1)
	{
		//WorldSpaceCamera = MatrixMatrixMultipy(WorldSpaceCamera, BuildRotationMatrixOnAxisX(-0.05));
		//m_Engine->RotateCameraX(-0.05);
		m_Engine->RotateCamera(1, (float)(3 * timer.Delta()));
	}
	if (GetAsyncKeyState(VK_NUMPAD8) & 0x1)
	{
		//WorldSpaceCamera = MatrixMatrixMultipy(Translate(m_camera.pos[0], m_camera.pos[1], m_camera.pos[2]), BuildRotationMatrixOnAxisY(-0.05));
		//m_Engine->RotateCameraY(0.05);
		//m_Engine->SetValueY(0.005);
		m_Engine->RotateCamera(0, (float)(-3 * timer.Delta()));
	}
	if (GetAsyncKeyState(VK_NUMPAD2) & 0x1)
	{
		//WorldSpaceCamera = MatrixMatrixMultipy(Translate(m_camera.pos[0], m_camera.pos[1], m_camera.pos[2]), BuildRotationMatrixOnAxisY(0.05));
		//m_Engine->RotateCameraY(-0.05);
		m_Engine->RotateCamera(0, (float)(3 * timer.Delta()));
	}
	if (resize)
	{
		m_Engine->ChangeOnce();
		resize = false;
	}
	if (GetAsyncKeyState(VK_TAB) & 0x1)
	{
		m_Engine->ChangeFull();
		m_Engine->ChangeOnce();
	}

	m_Engine->Draw();
	
	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6
	//s_chain->SetFullscreenState(FALSE, NULL);
	m_Engine->ShutDown();
	//delete[] m_Engine;

	UnregisterClass(L"DirectXApplication", application);
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	myApp.ShutDown();
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_TAB)
		{
			//fullscreen = true;
			//once = true;
		}
	}break;
	case WM_SIZE:
	{
		resize = true;
	}break;
	case (WM_DESTROY) : { PostQuitMessage(0); }
						break;
	}
						return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//

