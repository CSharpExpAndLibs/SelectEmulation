#pragma once

#ifdef SELECT_EXPORTS
#define SELECT_API __declspec(dllexport)
#else
#define SELECT_API __declspec(dllimport)
#endif

extern "C" {
	SELECT_API void InitSelect();
	SELECT_API char* ReadLine();
	SELECT_API char* ReadLinew();
	SELECT_API void SignaleToExit();
	SELECT_API void TermSelect();
}