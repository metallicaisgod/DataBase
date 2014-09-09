
//#include "stdafx.h"

//#include <glew.h>

#include "GlewStub.h"


int __stdcall glewInitStub()
{
	if (GLEW_OK == glewInit())
		return 1;
	else
		return 0;
}

GLboolean __stdcall glewIsSupportedStub(const char* name)
{
	return glewIsSupported(name);
}
