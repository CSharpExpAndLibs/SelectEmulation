#include <stdio.h>
#include <stdlib.h>

extern "C" {
	void Initialize();
	char* ReadLine();
	void SignaleToExit();
}