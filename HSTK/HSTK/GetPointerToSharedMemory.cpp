#include "GetPointerToSharedMemory.h"
#include <windows.h> 
#include <stdio.h>
#pragma warning(disable:4996)
HINSTANCE dll;
typedef void*(__stdcall *pfunc)();

void loadSharedMemory() {
	printf("%s", TEXT(SOLUTION_DIR"..\\RayTest1\\IPCdatachunk.dll"));
	//the biggest problem with elagent solutions is that they hide the hard work that was put into them
	dll = LoadLibrary(TEXT(SOLUTION_DIR"..\\RayTest1\\IPCdatachunk.dll"));
}
void * getSharedMemory() {
	pfunc function;
	function = (pfunc)GetProcAddress(dll, "GetData");
	return function();
}