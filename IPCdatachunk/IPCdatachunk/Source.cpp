#include "Header.hpp"
#include <string.h>
extern "C" _declspec(dllexport) void * GetData(){
return data;
}
extern "C" _declspec(dllexport) void ZeroData() {
	memset(data, 0, sizeof(data));
}