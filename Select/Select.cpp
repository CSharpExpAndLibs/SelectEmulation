#pragma warning(disable : 4996)

#include <Windows.h>
#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Select.h"

static HANDLE currentIn = NULL;
static HANDLE stdIn = NULL;
static HANDLE exitSem = NULL;
static HANDLE waitHandls[2] = { 0 };
static DWORD currentMode = 0;

char buffer[256];
WCHAR wcBuffer[256];

void InitSelect()
{

	// 標準入力のハンドルを取得
	stdIn = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(stdIn, &currentMode);
	SetConsoleMode(stdIn, ENABLE_PROCESSED_INPUT);
	char* locale = setlocale(LC_ALL, "ja_JP.utf8");
	if (locale == NULL) {
		printf("Locale NOT supported!\n");
	}

	// 退出セマフォ取得
	exitSem = CreateSemaphore(NULL, 0, 1, NULL);

	// 待ちリストに加える
	waitHandls[0] = stdIn;
	waitHandls[1] = exitSem;

}

char* ReadLine(DWORD* length)
{
	DWORD rdLen = 0;
	INPUT_RECORD inputRecord[128];
	memset(buffer, 0, sizeof(buffer));

	while (true) {
		// どちらかのイベントが立つまで待つ
		DWORD idx = WaitForMultipleObjects(
			2,
			waitHandls,
			FALSE,
			INFINITE
		);
		if (idx == WAIT_FAILED) {
			printf("WaitForMultipleObjectsEx() error:%d\n", GetLastError());
			return NULL;
		}

		if (idx == 0) {
			DWORD s = 0;
			BOOL r = ReadConsoleInput(waitHandls[0], inputRecord, sizeof(inputRecord)/sizeof(INPUT_RECORD), &s);
			if (!r) {
				printf("ReadError\n");
				return NULL;
			}
			for (int i = 0; i < s; i++) {
				if (inputRecord[i].EventType == KEY_EVENT && inputRecord[i].Event.KeyEvent.bKeyDown) {
					switch (inputRecord[i].Event.KeyEvent.wVirtualKeyCode) {
					case VK_BACK:
						if (rdLen > 0) {
							printf("\b \b");
							fflush(stdout);
							buffer[--rdLen] = 0;
						}
						break;
					case VK_RETURN:
						printf("\n");
						//printf("%s\n", buffer);
						fflush(stdout);
						*length = rdLen;
						return buffer;
					default:
						char c = (char)inputRecord[i].Event.KeyEvent.uChar.AsciiChar;
						WCHAR wc = inputRecord[i].Event.KeyEvent.uChar.UnicodeChar;
						if (c != '\0') {
							buffer[rdLen++] = c;
							buffer[rdLen] = 0;
							putc(c, stdout);
							fflush(stdout);
						}
						break;
					}
				}
			}
		}
		else {
			strcpy_s(buffer, sizeof(buffer), "exit");
			*length = 4;
			return buffer;
		}
	}
}

char* ReadLinew(DWORD* length)
{
	static char multiStr[sizeof(wcBuffer) * 2];
	DWORD rdLen = 0;
	INPUT_RECORD inputRecord[128];
	memset(wcBuffer, 0, sizeof(wcBuffer));
	memset(multiStr, 0, sizeof(multiStr));

	while (true) {
		// どちらかのイベントが立つまで待つ
		DWORD idx = WaitForMultipleObjects(
			2,
			waitHandls,
			FALSE,
			INFINITE
		);
		if (idx == WAIT_FAILED) {
			wprintf(L"WaitForMultipleObjectsEx() error:%d\n", GetLastError());
			return NULL;
		}

		if (idx == 0) {
			DWORD s = 0;
			BOOL r = ReadConsoleInput(waitHandls[0], inputRecord, sizeof(inputRecord) / sizeof(INPUT_RECORD), &s);
			if (!r) {
				wprintf(L"ReadError\n");
				return NULL;
			}
			for (int i = 0; i < s; i++) {
				if (inputRecord[i].EventType == KEY_EVENT && inputRecord[i].Event.KeyEvent.bKeyDown) {
					switch (inputRecord[i].Event.KeyEvent.wVirtualKeyCode) {
					case VK_BACK:
						if (rdLen > 0) {
							// 半角の時（本当にこの条件で良い？）
							if ((UINT)wcBuffer[rdLen - 1] < 256)
								wprintf(L"\b \b");
							// 全角の時
							else
								wprintf(L"\b\b  \b\b");
							fflush(stdout);
							wcBuffer[--rdLen] = L'\0';
						}
						break;
					case VK_RETURN:
						wprintf(L"\n");
						//printf("%s\n", buffer);
						fflush(stdout);
#pragma warning(suppress : 4996)
						*length = wcstombs(multiStr, wcBuffer, sizeof(multiStr));
						return multiStr;
					default:
						WCHAR wc = inputRecord[i].Event.KeyEvent.uChar.UnicodeChar;
						if (wc != L'\0') {
							wcBuffer[rdLen++] = wc;
							wcBuffer[rdLen] = L'\0';
							putwc(wc, stdout);
							fflush(stdout);
						}
						break;
					}
				}
			}
		}
		else {
#pragma warning(suppress : 4996)
			//wcscpy(wcBuffer, L"exit");
#pragma warning(suppress : 4996)
			strcpy(multiStr, "exit");
			*length = 4;
			return multiStr;
		}
	}
}

void TermSelect()
{
	SetConsoleMode(stdIn, currentMode);
}

void SignaleToExit()
{
	ReleaseSemaphore(exitSem, 1, NULL);
}