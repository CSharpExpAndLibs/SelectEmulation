#include "pch.h"
#include "framework.h"
#include "Select.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static HANDLE stdIn = NULL;
static HANDLE exitSem = NULL;
static HANDLE waitHandls[2] = { 0 };

void Initialize()
{
	// 標準入力のハンドルを取得
	stdIn = GetStdHandle(STD_INPUT_HANDLE);
	// 退出セマフォ取得
	exitSem = CreateSemaphore(NULL, 0, 1, NULL);

	// 待ちリストに加える
	waitHandls[0] = stdIn;
	waitHandls[1] = exitSem;

}

char* ReadLine()
{
	// どちらかのイベントが立つまで待つ
	DWORD idx = WaitForMultipleObjectsEx(
		2,
		waitHandls,
		FALSE,
		INFINITE,
		FALSE
	);

	if (idx == WAIT_FAILED) {
		printf("WaitForMultipleObjectsEx() error:%d\n", GetLastError());
		return NULL;
	}

	char buffer[256];
	char* ret = NULL;
	if (idx == 0) {
		fgets(buffer, sizeof(buffer), stdin);
	}
	else {
		strcpy_s(buffer, sizeof(buffer), "exit");
	}
	ret = (char*)malloc(strlen(buffer) + 1);
	if (ret != NULL)
		strcpy_s(ret, sizeof(ret), buffer);

	return ret;
}

void SignaleToExit()
{
	ReleaseSemaphore(exitSem, 1, NULL);
}