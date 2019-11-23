#pragma data_seg (".SHAREDMEMORY")
char data[1000000000] = { 0 };
#pragma data_seg() 

#pragma comment(linker,"/SECTION:.SHAREDMEMORY,RWS")
extern "C" _declspec(dllexport) void * GetData();
extern "C" _declspec(dllexport) void ZeroData();
