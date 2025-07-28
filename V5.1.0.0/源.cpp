#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:6031)
#pragma comment(linker,	"\"/manifestdependency:type='win32' \
							name='Microsoft.Windows.Common-Controls' \
							version='6.0.0.0' \
							processorArchitecture='*' \
							publicKeyToken='6595b64144ccf1df' \
							language='*'\"")
#include "标头.h"


#ifdef _DEBUG//内存泄露检测
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


int argc = 0;
LPTSTR* argv;
int g_iCmdShow;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevIntance, LPSTR szCmdline, int iCmdShow)
{
#ifdef _DEBUG
	ShowConsole();

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
	_CrtMemState memStateStart, memStateEnd, memStateDiff;
	_CrtMemCheckpoint(&memStateStart);
#endif

	g_hInstance = hInstance;
	g_iCmdShow = iCmdShow;
	argv = CommandLineToArgvW(GetCommandLineW(), &argc);//获取程序参数
	if (argc == 1)//配置模式
	{
		if (!IsAdmin())
		{
			SHELLEXECUTEINFOW shellexecinfo = {};
			shellexecinfo.cbSize = sizeof(shellexecinfo);
			shellexecinfo.lpVerb = L"runas";
			shellexecinfo.lpFile = argv[0];
			shellexecinfo.nShow = SW_SHOWNORMAL;
			ShellExecuteEx(&shellexecinfo);
		}
		else
		{
			Config();
		}
	}
	else//工作模式
	{
		if (!IsAdmin())
		{
			//写入参数数据到临时文件
			FILE* fp;
			wchar_t path[MAX_PATH];
			wchar_t name[5] = L"TEMP";
			int path_length;
			getfname(argv[0], NULL, path, NULL, &path_length);

			for (int i = 0; i < 4 && path_length < MAX_PATH - 1;)
			{
				path[path_length++] = name[i++];
			}
			path[path_length] = L'\0';
			fp = _wfopen(path, L"w");
			if (fp == nullptr)
			{
				return 0;
			}
			for (int i = 1; i < argc; ++i)
			{
				fwprintf(fp, L"%ws\n", argv[i]);
			}
			fclose(fp);
			TaskSchedulerHelper t;
			t.RunScheduledTask(L"\\UACSkip\\UACSkip");
			return 0;
		}
		WNDCLASSEX wndclassex = { 0 };
		wndclassex.cbSize = sizeof(WNDCLASSEX);
		wndclassex.lpfnWndProc = DefWindowProc;
		wndclassex.lpszClassName = TEXT("UACSkip");
		if (!RegisterClassEx(&wndclassex))
		{
			MessageBox(NULL, TEXT("这个程序需要在Windows NT才能执行！"), TEXT("UACSkip白名单工具"), MB_ICONERROR | MB_OK);
			return 0;
		}
		
		g_MainHwnd = CreateWindowEx(0,TEXT("UACSkip"),NULL,NULL,0,0,0,0,NULL,NULL,hInstance,NULL);
		Execute();
	}
#ifdef _DEBUG//检测内存泄露问题
	_CrtMemCheckpoint(&memStateEnd);
	if (_CrtMemDifference(&memStateDiff, &memStateStart, &memStateEnd))
	{
		wchar_t buffer[512];
		wsprintf(
			buffer,
			L"内存泄漏!\n\n"
			L"泄露内存块数: %ld\n"
			L"总泄露字节: %ld\n",
			memStateDiff.lCounts[_NORMAL_BLOCK],
			memStateDiff.lSizes[_NORMAL_BLOCK]
		);
		MessageBox(NULL, buffer, L"喵喵！", MB_OK | MB_ICONERROR);
	}
	else
	{
		//MessageBox(NULL, L"没有检测到内存泄漏喵~", L"喵~", MB_OK | MB_ICONINFORMATION);
	}
#endif

	return 0;
}