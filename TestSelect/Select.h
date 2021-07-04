#include <stdio.h>
#include <stdlib.h>

extern "C" {
	void InitSelect();
	char* ReadLine();
	char* ReadLinew(DWORD* len);
	void SignaleToExit();
	void TermSelect();
}