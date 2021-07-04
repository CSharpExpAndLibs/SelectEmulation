#include <stdio.h>
#include <stdlib.h>

extern "C" {
	void InitSelect();
	char* ReadLine();
	WCHAR* ReadLinew();
	void SignaleToExit();
	void TermSelect();
}