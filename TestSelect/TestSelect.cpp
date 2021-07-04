// TestSelect.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
// このプロジェクトでSelect.cppのデバッグを行った。コンソール入力系は日本語含めてほぼOK
// 後は、Semaphoreで中止するデモをTcpClientから接続する。

#include <Windows.h>

#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include "Select.h"

extern char gConsoleBuffer[256];

DWORD WINAPI ConsoleThread(LPVOID args)
{
    char buffer[256];
    while (true) {
        printf("Input> ");
        fgets((char*)args, sizeof(buffer), stdin);
    }
}

void Read()
{
    while (true) {
        printf("Input> ");
        char* buff = ReadLine();
        printf("「%s」が入力されました\n", buff);
        if (strcmp(buff, "exit") == 0)
            break;
    }
}

void Readw()
{
    while (true) {
        wprintf(L"Input> ");
        WCHAR* buff = ReadLinew();
        //wprintf(L"[%ls] was input\n", buff);
        wprintf(L"「%ls」が入力されました\n", buff);
        if (wcscmp(buff, L"exit") == 0)
            break;
    }
}

int main()
{

    InitSelect();

    char* locale = setlocale(LC_ALL, "ja_JP.utf8");
    if (locale == NULL) {
        printf("Locale NOT supported!\n");
    }
    printf("sizeof(wchar)=%d\n", sizeof(wchar_t));

    //Read();
    Readw();

    TermSelect();
    printf("Input Something\n");
    char buffer[256];
    fgets(buffer, sizeof(buffer), stdin);
}




// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
