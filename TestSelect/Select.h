#include <stdio.h>
#include <stdlib.h>

extern "C" {
	void Initialize();
	char* ReadLine();
	WCHAR* ReadLinew();
	void SignaleToExit();
	void Term();
}