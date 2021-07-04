#include <stdio.h>
#include <stdlib.h>

extern "C" {
	void InitSelect();
	char* ReadLine();
	char* ReadLinew();
	void SignaleToExit();
	void TermSelect();
}