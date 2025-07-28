#pragma once
#include <iostream>
#include <windows.h>
#include <windowsx.h>
#include <string>
#include <D:\C files\myfunc.h>
#include <D:\C files\myWin32func.h>
using namespace std;


extern int argc;
extern LPTSTR* argv;
extern HINSTANCE g_hInstance;
extern HWND g_MainHwnd;
extern int g_iCmdShow;
extern HWND g_EditHwnd1_Name;
extern HWND g_EditHwnd1_Path;
extern HWND g_EditHwnd1_StartPath;
extern HWND g_EditHwnd1_Args;
extern HWND g_EditHwnd2_Path;

BOOL IsAdmin();

void Config();

void Execute();

void ShowConsole();

void OpenTask();

void ProcessFile(wchar_t* filePath);

void ProcessFile2(wchar_t* filePath);

BOOL CheckReg();

bool RegInstall();

bool RegUninstall();

void CenterTabControl(HWND hWndParent, HWND hWndTab);

bool CheckAndUpdateTaskAction(
	const std::wstring& taskPath,
	const std::wstring& expectedProgram,
	const std::wstring& expectedArgs,
	const std::wstring& expectedFolder = L"\\"
);