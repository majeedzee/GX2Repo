#include "Foundation.h"

void main()
{
	Foundation* frameWork = new Foundation();

	if (frameWork->Initialize())
	{
		frameWork->Run();
	}

	delete frameWork;

	return;
}