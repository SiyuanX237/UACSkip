#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:6031)
#pragma comment(linker,	"\"/manifestdependency:type='win32' \
							name='Microsoft.Windows.Common-Controls' \
							version='6.0.0.0' \
							processorArchitecture='*' \
							publicKeyToken='6595b64144ccf1df' \
							language='*'\"")
#include "��ͷ.h"


#ifdef _DEBUG//�ڴ�й¶���
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
	argv = CommandLineToArgvW(GetCommandLineW(), &argc);//��ȡ�������
	if (argc == 1)//����ģʽ
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
	else//����ģʽ
	{
		if (!IsAdmin())
		{
			//д��������ݵ���ʱ�ļ�
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
			MessageBox(NULL, TEXT("���������Ҫ��Windows NT����ִ�У�"), TEXT("UACSkip����������"), MB_ICONERROR | MB_OK);
			return 0;
		}
		
		g_MainHwnd = CreateWindowEx(0,TEXT("UACSkip"),NULL,NULL,0,0,0,0,NULL,NULL,hInstance,NULL);
		Execute();
	}
#ifdef _DEBUG//����ڴ�й¶����
	_CrtMemCheckpoint(&memStateEnd);
	if (_CrtMemDifference(&memStateDiff, &memStateStart, &memStateEnd))
	{
		wchar_t buffer[512];
		wsprintf(
			buffer,
			L"�ڴ�й©!\n\n"
			L"й¶�ڴ����: %ld\n"
			L"��й¶�ֽ�: %ld\n",
			memStateDiff.lCounts[_NORMAL_BLOCK],
			memStateDiff.lSizes[_NORMAL_BLOCK]
		);
		MessageBox(NULL, buffer, L"������", MB_OK | MB_ICONERROR);
	}
	else
	{
		//MessageBox(NULL, L"û�м�⵽�ڴ�й©��~", L"��~", MB_OK | MB_ICONINFORMATION);
	}
#endif

	return 0;
}