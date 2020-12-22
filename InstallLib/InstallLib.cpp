#include "InstallLib.h"

/*
	To test the library, include "InstallLib.h" from an application project
	and call InstallLibTest().
	
	Do not forget to add the library to Project Dependencies in Visual Studio.
*/

static int s_Test = 0;

int InstallLibTest()
{
	return ++s_Test;
}