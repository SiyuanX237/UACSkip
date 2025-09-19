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
		
		//注册消息
		hWakeup = CreateEvent(NULL, FALSE, FALSE, L"Local\\UACSKIPWakeup");
		hSleep = CreateEvent(NULL, FALSE, FALSE, L"Local\\UACSKIPSleep");
		hExecute = CreateEvent(NULL, FALSE, FALSE, L"Local\\UACSKIPExecute");
		if (!IsAdmin())//非管理员
		{
			//尝试打开
			hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"Local\\UACSKIPShare");
			//打不开说明是第一个
			if(!hMap)
			{
				//由它创建共享内存
				hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedQueue), L"Local\\UACSKIPShare");
				//开个互斥锁
				hMutex = CreateMutex(NULL, FALSE, L"Local\\UACSKIPShareMutex");
				queue = (SharedQueue *)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedQueue));
				//写入参数信息
				lstrcpynW(queue->Path, argv[1], MAX_PATH);
				wstring param;
				for(int i = 2; i < argc; ++i)
				{
					param += argv[i];
				}
				lstrcpynW(queue->Param, param.c_str(), 16383);
				//以管理员运行
				TaskSchedulerHelper t;
				t.RunScheduledTask(L"\\UACSkip\\UACSkip");
				if(WaitForSingleObject(hSleep, 2500) == WAIT_TIMEOUT)
				{
					return 0;
				}
				SetEvent(hExecute);
			}
			//有现成的了
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
				bool sleeping = false;//该不该睡
				while(1)
				{
					if(sleeping)//睡了
					{
						WaitForSingleObject(hWakeup, INFINITE);//等待苏醒信号
						ResumeThread(inputID);//起来继续竞争
						sleeping = false;
					}
					else
					{
						WaitForSingleObject(hSleep, INFINITE);//等待睡眠信号
						SuspendThread(inputID);//现在就睡
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
			//	UnmapViewOfFile(queue);//释放虚拟内存映射
			//	CloseHandle(hMap);//释放内核对象
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
				MessageBox(NULL, TEXT("这个程序需要在Windows NT才能执行！"), TEXT("UACSkip白名单工具"), MB_ICONERROR | MB_OK);
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
				sei.lpVerb = L"runas"; // 提权
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
			//sei.lpVerb = L"runas"; // 提权
			//sei.nShow = SW_SHOWNORMAL;
			//sei.lpFile = (const wchar_t*)queue->Path;
			//sei.lpParameters = (const wchar_t*)queue->Param;
			//ShellExecuteEx(&sei);

			//ReleaseMutex(hMutex);
			//CloseHandle(hMutex);
			//CloseHandle(hEvent);
			//UnmapViewOfFile(queue);//释放虚拟内存映射
			//CloseHandle(hMap);//释放内核对象
		}
		
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