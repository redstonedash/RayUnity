#include "GetPointerToSharedMemory.h"
#include <windows.h> 
#pragma warning(disable:4996)
HINSTANCE dll;
typedef void*(__stdcall *pfunc)();

void loadSharedMemory() {
	dll = LoadLibrary(TEXT("C:\\Users\\s189064\\Documents\\GitHub\\RayTest1\\Assets\\IPCdatachunk.dll"));
}
void * getSharedMemory() {
	pfunc function;
	function = (pfunc)GetProcAddress(dll, "GetData");
	return function();
}