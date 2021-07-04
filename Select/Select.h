#pragma once

#ifdef SELECT_EXPORTS
#define SELECT_API __declspec(dllexport)
#else
#define SELECT_API __declspec(dllimport)
#endif


extern "C" {
	SELECT_API void __cdecl InitSelect();
	SELECT_API char* __cdecl ReadLine(DWORD* length);
	SELECT_API char* __cdecl ReadLinew(DWORD* length);
	SELECT_API void  __cdecl SignaleToExit();
	SELECT_API void  __cdecl TermSelect();
}