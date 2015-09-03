#pragma once

#define WIN32_LEAN_AND_MEAN

#include "Engine.h"

class Foundation
{
public:
	Foundation();
	~Foundation();

	bool Initialize();
	void Run();

private:
	bool CreateAppWindow(char* windowTitle, int x, int y, int width, int height);
	
	char* m_appName;
	HINSTANCE m_instance;
};

