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


struct SharedQueue
{
	wchar_t Path[MAX_PATH];
	wchar_t Param[16383];
};
HANDLE hSleep;
HANDLE hWakeup;
HANDLE hExecute;
HANDLE hMutex;
HANDLE hMap;
SharedQueue *queue;

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
		
		//ע����Ϣ
		hWakeup = CreateEvent(NULL, FALSE, FALSE, L"Local\\UACSKIPWakeup");
		hSleep = CreateEvent(NULL, FALSE, FALSE, L"Local\\UACSKIPSleep");
		hExecute = CreateEvent(NULL, FALSE, FALSE, L"Local\\UACSKIPExecute");
		if (!IsAdmin())//�ǹ���Ա
		{
			//���Դ�
			hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"Local\\UACSKIPShare");
			//�򲻿�˵���ǵ�һ��
			if(!hMap)
			{
				//�������������ڴ�
				hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedQueue), L"Local\\UACSKIPShare");
				//����������
				hMutex = CreateMutex(NULL, FALSE, L"Local\\UACSKIPShareMutex");
				queue = (SharedQueue *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedQueue));
				//д�������Ϣ
				lstrcpynW(queue->Path, argv[1], MAX_PATH);
				wstring param;
				for(int i = 2; i < argc; ++i)
				{
					param += argv[i];
				}
				lstrcpynW(queue->Param, param.c_str(), 16383);
				//�Թ���Ա����
				TaskSchedulerHelper t;
				t.RunScheduledTask(L"\\UACSkip\\UACSkip");
				if(WaitForSingleObject(hSleep, 2500) == WAIT_TIMEOUT)
				{
					return 0;
				}
				SetEvent(hExecute);
			}
			//���ֳɵ���
			else
			{
				hMutex = CreateMutex(NULL, FALSE, L"Local\\UACSKIPShareMutex");
				queue = (SharedQueue *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedQueue));
				auto trytoinput = [](LPVOID Param)->DWORD
				{
					if(WaitForSingleObject(hMutex, 5000) == WAIT_TIMEOUT)
					{
						exit(0);
					}
					lstrcpynW(queue->Path, argv[1], MAX_PATH);
					wstring param;
					for(int i = 2; i < argc; ++i)
					{
						param += argv[i];
					}
					lstrcpynW(queue->Param, param.c_str(), 16383);
					SetEvent(hSleep);
					SetEvent(hExecute);
					exit(0);
				};
				HANDLE inputID = CreateThread(0, 0, trytoinput, NULL, 0, NULL);
				bool sleeping = false;//�ò���˯
				while(1)
				{
					if(sleeping)//˯��
					{
						WaitForSingleObject(hWakeup, INFINITE);//�ȴ������ź�
						ResumeThread(inputID);//������������
						sleeping = false;
					}
					else
					{
						WaitForSingleObject(hSleep, INFINITE);//�ȴ�˯���ź�
						SuspendThread(inputID);//���ھ�˯
						sleeping = true;
					}
				}
			}

			//loop1:
			//if (WaitForSingleObject(hWakeup, 2500) != WAIT_OBJECT_0)
			//{
			//	return 0;
			//}
			//
			//
			//if (queue->ready == true)
			//{
			//	ReleaseMutex(hMutex);
			//	goto loop1;
			//}
			//
			//queue->ready = true;
			//ReleaseMutex(hMutex);
			//hEvent = CreateEvent(NULL, TRUE, FALSE, L"Local\\UACSKIPShareEvent");
			//
			//
			//if (WaitForSingleObject(hEvent, 3000) == WAIT_OBJECT_0)
			//{
			//	return 0;
			//}
			//else
			//{
			//	CloseHandle(hMutex);
			//	UnmapViewOfFile(queue);//�ͷ������ڴ�ӳ��
			//	CloseHandle(hMap);//�ͷ��ں˶���
			//	return 0;
			//}
			
		}
		else
		{
			WNDCLASSEX wndclassex = { 0 };
			wndclassex.cbSize = sizeof(WNDCLASSEX);
			wndclassex.lpfnWndProc = DefWindowProc;
			wndclassex.lpszClassName = TEXT("UACSkip");
			if (!RegisterClassEx(&wndclassex))
			{
				MessageBox(NULL, TEXT("���������Ҫ��Windows NT����ִ�У�"), TEXT("UACSkip����������"), MB_ICONERROR | MB_OK);
				return 0;
			}

			g_MainHwnd = CreateWindowEx(0, TEXT("UACSkip"), NULL, NULL, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
			
			hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"Local\\UACSKIPShare");
			queue = (SharedQueue *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedQueue));
			SetEvent(hSleep);

			while(1)
			{
				if(WaitForSingleObject(hExecute, 5000) == WAIT_TIMEOUT)
				{
					exit(0);
				}
				ForceSetForeground(g_MainHwnd);
				SHELLEXECUTEINFO sei = { 0 };
				sei.cbSize = sizeof(sei);
				sei.lpVerb = L"runas"; // ��Ȩ
				sei.nShow = SW_SHOWNORMAL;
				sei.lpFile = (const wchar_t *)queue->Path;
				sei.lpParameters = (const wchar_t *)queue->Param;
				ShellExecuteEx(&sei);
				SetEvent(hWakeup);
			}


			//hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"Local\\UACSKIPShare");
			//hMutex = CreateMutex(NULL, FALSE, L"Local\\UACSKIPShareMutex");
			//loop2:
			//if (WaitForSingleObject(hMutex, 1000) != WAIT_OBJECT_0)
			//{
			//	return 0;
			//}
			//SharedQueue* queue = (SharedQueue*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedQueue));
			//if (queue->ready == false)
			//{
			//	ReleaseMutex(hMutex);
			//	goto loop2;
			//}
			//hEvent = OpenEventW(EVENT_MODIFY_STATE, FALSE, L"Local\\UACSKIPShareEvent");
			//SetEvent(hEvent);
			//ForceSetForeground(g_MainHwnd);
			//SHELLEXECUTEINFO sei = { 0 };
			//sei.cbSize = sizeof(sei);
			//sei.lpVerb = L"runas"; // ��Ȩ
			//sei.nShow = SW_SHOWNORMAL;
			//sei.lpFile = (const wchar_t*)queue->Path;
			//sei.lpParameters = (const wchar_t*)queue->Param;
			//ShellExecuteEx(&sei);

			//ReleaseMutex(hMutex);
			//CloseHandle(hMutex);
			//CloseHandle(hEvent);
			//UnmapViewOfFile(queue);//�ͷ������ڴ�ӳ��
			//CloseHandle(hMap);//�ͷ��ں˶���
		}
		
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