#define _CRT_SECURE_NO_WARNINGS
#include "标头.h"

#include "resource.h"




//#define AcceleratorsEnable
//全局变量区
HWND g_MainHwnd;
HINSTANCE g_hInstance;
SIZE g_ClientSize;
HWND g_TipHwnd_TaskInstation = NULL;
HWND g_TipHwnd_RegInstation = NULL;
HICON g_hIcon;

RECT g_PageRect;
RECT g_HelpRect;
int g_SelectPage = 0;
WNDPROC g_OldTabProc;
WNDPROC g_OldPage1Proc;
WNDPROC g_OldPage2Proc;
WNDPROC g_OldPage3Proc;
WNDPROC g_OldStatusProc;

HFONT g_Font;
HFONT g_EditFont;
HFONT g_FontBold;
SIZE g_FontSize;
INT g_LineAnchorY[7];
HWND g_TabHwnd;
HWND g_PageHwnd[3];
HWND g_ButtonHwnd1_Path;
HWND g_ButtonHwnd2_Path;
HWND g_ButtonHwnd1_StartPath;
HWND g_EditHwnd1_Path;
HWND g_EditHwnd2_Path;
HWND g_EditHwnd1_StartPath;
HWND g_EditHwnd1_Name;
HWND g_EditHwnd1_OutputLnk;
HWND g_EditHwnd1_Args;
HWND g_ButtonHwnd1_Install;
HWND g_ButtonHwnd1_Uninstall;
HWND g_ButtonHwnd1_OpenSchtasks;
HWND g_ButtonHwnd2_OpenRegedit;
HWND g_ButtonHwnd1_OutputLnk;
HWND g_ButtonHwnd1_Execute;
HWND g_ButtonHwnd2_Execute;
HWND g_ButtonHwnd1_OpenEnvironmentVariable;
HWND g_StaticHwnd1_Status;
HWND g_ButtonHwnd1_ContextMenu;
HWND g_StaticHwnd1_Help;
HWND g_StaticHwnd2_Help;
HWND g_LinkHwnd3_Home;
HWND g_LinkHwnd3_Patron;
HBRUSH BackgroundBrush;
TCHAR g_DesktopPath[MAX_PATH];
bool VariableStatus = false;
bool TaskInstalled = false;
BOOL RegInstalled = 0;



LRESULT CALLBACK Page1Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			HGDIOBJ OldFont;
			OldFont = SelectObject(hdc, g_Font);
			
			RECT rc;
			GetClientRect(hwnd, &rc);
			FillRect(hdc, &rc, BackgroundBrush);

			RECT RectDrawText;
			RectDrawText.left = 0;
			RectDrawText.right = g_FontSize.cx * 8;

			RectDrawText.top = g_LineAnchorY[0] - g_FontSize.cy * 0.2;
			RectDrawText.bottom = RectDrawText.top + g_FontSize.cy * 1.3;
			DrawText(hdc, TEXT("任务计划服务"), -1, &RectDrawText, DT_RIGHT | DT_SINGLELINE);

			RectDrawText.top = g_LineAnchorY[1] - g_FontSize.cy * 0.2;
			RectDrawText.bottom = RectDrawText.top + g_FontSize.cy * 1.3;
			DrawText(hdc, TEXT("目标程序路径"), -1, &RectDrawText, DT_RIGHT | DT_SINGLELINE);

			RectDrawText.top = g_LineAnchorY[2] - g_FontSize.cy * 0.2;
			RectDrawText.bottom = RectDrawText.top + g_FontSize.cy * 1.3;
			DrawText(hdc, TEXT("快捷方式名"), -1, &RectDrawText, DT_RIGHT | DT_SINGLELINE);

			RectDrawText.top = g_LineAnchorY[3] - g_FontSize.cy * 0.2;
			RectDrawText.bottom = RectDrawText.top + g_FontSize.cy * 1.3;
			DrawText(hdc, TEXT("起始位置"), -1, &RectDrawText, DT_RIGHT | DT_SINGLELINE);
			
			RectDrawText.top = g_LineAnchorY[4] - g_FontSize.cy * 0.2;
			RectDrawText.bottom = RectDrawText.top + g_FontSize.cy * 1.3;
			DrawText(hdc, TEXT("参数"), -1, &RectDrawText, DT_RIGHT | DT_SINGLELINE);

			RectDrawText.top = g_LineAnchorY[5] - g_FontSize.cy * 0.2;
			RectDrawText.bottom = RectDrawText.top + g_FontSize.cy * 1.3;
			DrawText(hdc, TEXT("快捷方式位置"), -1, &RectDrawText, DT_RIGHT | DT_SINGLELINE);

			Rectangle(hdc, g_HelpRect.left, g_HelpRect.top, g_HelpRect.right, g_HelpRect.bottom);
			DrawText(hdc, TEXT("帮助"), -1, &g_HelpRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			
			
			EndPaint(hwnd, &ps);
			SelectObject(hdc, OldFont);
			break;
		}
		case WM_COMMAND:
		{
			if (HIWORD(wParam) != BN_CLICKED)//仅点击事件
			{
				break;
			}
			switch (LOWORD(wParam))//看消息是谁发出的
			{
				//自定义菜单、加速键消息区（需要区分就内部使用switch(HIWORD(wParam))判断，0是菜单，1是加速键

				//控件ID区
				case ID_BTN1_Path:
				{
					wchar_t path[260] = {};
					if (
						ShowOpenFileDialog(hwnd,
							{
								{L"可执行程序(*.exe)",L"*.exe"},
								{L"脚本文件(*.bat)",L"*.bat"},
							},
							path, NULL, g_DesktopPath, TEXT("选取一个程序…"))
							)
					{
						ProcessFile(path);
					}
					
					break;
				}
				case ID_BTN1_StartPath:
				{
					wchar_t path[260];
					if (ShowSelectFolderDialog(hwnd, path))
					{
						SetWindowText(g_EditHwnd1_StartPath, path);
					}
					
					break;
				}
				case ID_BTN1_Install:
				{
					if (!VariableStatus)
					{
						HKEY hKey;
						LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"),0, KEY_SET_VALUE, &hKey);
						if (lResult != ERROR_SUCCESS)
						{
							MessageBox(hwnd, TEXT("写入环境变量失败，检查是否有权限"), TEXT("错误"), MB_OK | MB_ICONERROR);
							return false;
						}
						RegDeleteValue(hKey, L"UACSkip");
						lResult = RegSetValueEx(hKey, L"UACSkip", 0, REG_SZ,
							(const BYTE*)argv[0],
							(DWORD)((_tcslen(argv[0]) + 1) * sizeof(TCHAR)));

						RegCloseKey(hKey);

						if (lResult != ERROR_SUCCESS)
						{
							MessageBox(hwnd, TEXT("写入环境变量失败，检查是否有权限"), TEXT("错误"), MB_OK | MB_ICONERROR);
							return false;
						}

						SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
							(LPARAM)TEXT("Environment"),
							SMTO_ABORTIFHUNG, 3000, nullptr);

						VariableStatus = true;
					}
					if (!TaskInstalled)
					{
						TaskSchedulerHelper t;
						TASK_CREATION_INFO ti;
						ti.author = TEXT("UACSkip");
						ti.folderPath = TEXT("\\UACSkip");
						ti.taskName = TEXT("UACSkip");
						ti.description = TEXT("UAC白名单任务计划服务");
						ti.documentation = TEXT("喵喵！你导出了XML，这段文字被你发现了喵ฅ(･ω･*ฅ)~");
						ti.runLevel = TASK_RUNLEVEL_HIGHEST;
						ti.execPath = argv[0];
						ti.arguments = TEXT("1");
						ti.allowOverwrite = true;
						if (t.CreateTask(move(ti)))
						{
							EnableWindow(g_ButtonHwnd1_Install, false);
							EnableWindow(g_ButtonHwnd1_Uninstall, true);
							TaskInstalled = true;
							if (g_TipHwnd_TaskInstation)
							{
								ShowWindow(g_TipHwnd_TaskInstation, SW_HIDE);
							}
							SetWindowText(g_StaticHwnd1_Status, TEXT("已安装"));
						}
						else
						{
							MessageBox(hwnd, TEXT("操作失败"), TEXT("提示"), MB_OK | MB_ICONERROR);
							return false;
						}
					}
					EnableWindow(g_ButtonHwnd1_ContextMenu, true);
					RegInstalled = CheckReg();
					if (RegInstalled == -1)
					{
						SetWindowText(g_ButtonHwnd1_ContextMenu, TEXT("修复右键菜单"));
						POINT pt = { g_FontSize.cx * 6,g_LineAnchorY[6] + g_FontSize.cy };
						ClientToScreen(g_PageHwnd[0], &pt);
						ShowTooltip(g_PageHwnd[0], &g_TipHwnd_RegInstation, pt, TEXT("检测到右键注册表参数与实际不符，点此修复"), TEXT("提示"), true, true, TTI_INFO_LARGE, 12000);
					}
					else if (RegInstalled == 1)
					{
						SetWindowText(g_ButtonHwnd1_ContextMenu, TEXT("移除右键菜单"));
					}
					MessageBox(hwnd, TEXT("安装成功"), TEXT("提示"), MB_OK | MB_ICONINFORMATION);
					break;
				}
				case ID_BTN1_Uninstall:
				{
					if (VariableStatus)
					{
						HKEY hKey;
						LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"), 0, KEY_SET_VALUE, &hKey);
						if (lResult != ERROR_SUCCESS)
						{
							MessageBox(hwnd, TEXT("写入环境变量失败，检查是否有权限"), TEXT("错误"), MB_OK | MB_ICONERROR);
							return false;
						}
						RegDeleteValue(hKey, L"UACSkip");
						VariableStatus = false;
					}
					if (TaskInstalled)
					{
						TaskSchedulerHelper t;
						if (t.DeleteFolder(TEXT("\\UACSkip")))
						{
							EnableWindow(g_ButtonHwnd1_Install, true);
							EnableWindow(g_ButtonHwnd1_Uninstall, false);
							TaskInstalled = false;
							SetWindowText(g_StaticHwnd1_Status, TEXT("未安装"));
						}
						else
						{
							MessageBox(hwnd, TEXT("操作失败"), TEXT("提示"), MB_OK | MB_ICONERROR);
							return false;
						}
					}
					if (RegInstalled == 0)
					{
						EnableWindow(g_ButtonHwnd1_ContextMenu, false);
					}
					RegInstalled = CheckReg();

					MessageBox(hwnd, TEXT("卸载成功"), TEXT("提示"), MB_OK | MB_ICONINFORMATION);
					break;
				}
				case ID_BTN1_OpenSchtasks:
				{
					OpenTask();
					break;
				}
				case ID_BTN1_Output:
				{
					wchar_t path[260];
					GetWindowText(g_EditHwnd1_Name, path, MAX_PATH);
					if (
						ShowSaveFileDialog(hwnd,
							{
								{L"快捷方式(*.lnk)",L"*.lnk"},
							},
							path, NULL,g_DesktopPath,NULL)
							)
					{
						StrNCatW(path, L".lnk", MAX_PATH);
						SetWindowText(g_EditHwnd1_OutputLnk, path);
					}

					break;
				}
				case ID_BTN1_Execute:
				{
					wchar_t Path[MAX_PATH];
					wchar_t Name[MAX_PATH];
					wchar_t InitPath[MAX_PATH];
					wchar_t* Args = (wchar_t*)malloc(sizeof(wchar_t) * 4096);
					wchar_t OutputLnk[MAX_PATH];
					
					GetWindowText(g_EditHwnd1_Path, Path, MAX_PATH);
					if (!PathFileExists(Path))
					{
						MessageBox(hwnd, TEXT("目标路径不合法"), TEXT("提示"), MB_OK | MB_ICONERROR);
						break;
					}
					GetWindowText(g_EditHwnd1_Name, Name, MAX_PATH);
					GetWindowText(g_EditHwnd1_StartPath, InitPath, MAX_PATH);
					GetWindowText(g_EditHwnd1_Args, Args, 4096);
					GetWindowText(g_EditHwnd1_OutputLnk, OutputLnk, MAX_PATH);
					if (Name[0] == L'\0')
					{
						getfname(Path, Name, NULL, NULL, NULL);
						int len = lstrlen(Name) - 1;
						for (; Name[len] != L'.'; --len)
						{

						}
						Name[len] = L'\0';
					}
					if (OutputLnk[0] == L'\0')
					{
						_snwprintf(OutputLnk, MAX_PATH, L"%ws\\%ws.lnk", g_DesktopPath, Name);
					}
					wchar_t* Args_ = (wchar_t*)malloc(sizeof(wchar_t) * 4096);
					_snwprintf(Args_, 4096, L"\"%ws\" %ws", Path, Args);
					wstring result = CheckPathType(Path);
					bool isbat = (lstrcmp(result.c_str(), L".bat") == 0) ? true : false;;
					if (!CreateShortcut(OutputLnk, argv[0], Args_, InitPath, isbat ? L"%SystemRoot%\\System32\\imageres.dll" : Path, isbat ? -68 : 0, TEXT("UACSkip白名单"), true, L"%UACSkip%"))
					{
						MessageBox(hwnd, TEXT("创建失败"), TEXT("提示"), MB_OK | MB_ICONERROR);
					}
					else
					{
						MessageBox(hwnd, TEXT("创建成功"), TEXT("提示"), MB_OK | MB_ICONINFORMATION);
					}
					free(Args_);
					free(Args);
					break;
				}
				case ID_BTN1_OpenVironment:
				{
					if ((INT_PTR)ShellExecute(NULL, L"open", L"rundll32.exe", L"sysdm.cpl,EditEnvironmentVariables", NULL, SW_SHOW) <= 32)
					{
						MessageBox(hwnd, TEXT("界面打开失败，请手动定位"), TEXT("提示"), MB_OK | MB_ICONERROR);
					}
					break;
				}
				case ID_BTN1_ContextMenu:
				{
					if (RegInstalled == 1)
					{
						if (RegUninstall())
						{
							RegInstalled = 0;
							SetWindowText(g_ButtonHwnd1_ContextMenu, TEXT("添加右键菜单"));
							if (!TaskInstalled)
							{
								EnableWindow(g_ButtonHwnd1_ContextMenu, false);
							}
							MessageBox(hwnd, TEXT("移除成功"), TEXT("提示"), MB_OK | MB_ICONINFORMATION);
						}
						else
						{
							MessageBox(hwnd, TEXT("操作失败"), TEXT("提示"), MB_OK | MB_ICONERROR);
						}
					}
					else
					{
						if (RegInstall())
						{
							RegInstalled = 1;
							SetWindowText(g_ButtonHwnd1_ContextMenu, TEXT("移除右键菜单"));
							if (g_TipHwnd_RegInstation)
							{
								ShowWindow(g_TipHwnd_RegInstation, SW_HIDE);
							}
							MessageBox(hwnd, TEXT("添加成功"), TEXT("提示"), MB_OK | MB_ICONINFORMATION);
						}
						else
						{
							MessageBox(hwnd, TEXT("操作失败"), TEXT("提示"), MB_OK | MB_ICONERROR);
						}
					}
					break;
				}
			}
			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			HWND hwndStatic = (HWND)lParam;

			if (hwndStatic == g_StaticHwnd1_Status)
			{
				if (TaskInstalled)
				{
					SetTextColor(hdcStatic, RGB(0, 200, 0));
				}
				else
				{
					SetTextColor(hdcStatic, RGB(200, 0, 0));
				}
				SetBkMode(hdcStatic, TRANSPARENT);       // 设置背景透明
			}
			return (INT_PTR)BackgroundBrush;
		}
		case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP)wParam;
			wchar_t filePath[MAX_PATH];
			DragQueryFile(hDrop, 0, filePath, MAX_PATH);
			ProcessFile(filePath);
			DragFinish(hDrop);//释放资源
			break;
		}
		case WM_DESTROY:
		{
			break;
		}
	}
	
	return CallWindowProc(g_OldPage1Proc, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK TabProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CTLCOLORSTATIC:
		return (INT_PTR)GetStockObject(NULL_BRUSH);

	}
	return CallWindowProc(g_OldTabProc, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK Page2Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			HGDIOBJ OldFont;
			OldFont = SelectObject(hdc, g_Font);

			RECT rc;
			GetClientRect(hwnd, &rc);
			FillRect(hdc, &rc, BackgroundBrush);

			RECT RectDrawText;
			RectDrawText.left = 0;
			RectDrawText.right = g_FontSize.cx * 8;

			RectDrawText.top = g_LineAnchorY[1] - g_FontSize.cy * 0.2;
			RectDrawText.bottom = RectDrawText.top + g_FontSize.cy * 1.3;
			DrawText(hdc, TEXT("目标程序路径"), -1, &RectDrawText, DT_RIGHT | DT_SINGLELINE);

			Rectangle(hdc, g_HelpRect.left, g_HelpRect.top, g_HelpRect.right, g_HelpRect.bottom);
			DrawText(hdc, TEXT("帮助"), -1, &g_HelpRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

			EndPaint(hwnd, &ps);
			SelectObject(hdc, OldFont);
			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			HWND hwndStatic = (HWND)lParam;
			return (INT_PTR)BackgroundBrush;
		}
		case WM_COMMAND:
		{
			if (HIWORD(wParam) != BN_CLICKED)
			{
				break;
			}
			switch (LOWORD(wParam))
			{
				case ID_BTN2_Path:
				{
					wchar_t path[260] = {};
					if (
						ShowOpenFileDialog(hwnd,
							{
								{L"可执行程序(*.exe)",L"*.exe"},
							},
							path, NULL, g_DesktopPath, TEXT("选取一个程序…"))
							)
					{
						ProcessFile2(path);
					}

					break;
				}
				case ID_BTN2_Execute:
				{
					wchar_t Path[MAX_PATH];
					GetWindowText(g_EditHwnd2_Path, Path, MAX_PATH);
					HKEY hKey;
					if (RegCreateKeyExW(HKEY_CURRENT_USER,
						L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers",
						0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
					{
						RegSetValueExW(hKey, Path, 0, REG_SZ,
							reinterpret_cast<const BYTE*>(L"RunAsInvoker"),
							static_cast<DWORD>((wcslen(L"RunAsInvoker") + 1) * sizeof(wchar_t)));
						RegCloseKey(hKey);
						MessageBox(hwnd, TEXT("添加成功"), TEXT("提示"), MB_OK | MB_ICONINFORMATION);
					}
					else
					{
						MessageBox(hwnd, TEXT("添加失败"), TEXT("提示"), MB_OK | MB_ICONERROR);
					}
					break;
				}
				case ID_BTN1_OpenRegedit:
				{
					OpenRegLocation(L"HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers");
					POINT pt = { 0 };
					pt = { g_FontSize.cx * 28,(LONG)(g_LineAnchorY[0] + g_FontSize.cy) };
					ClientToScreen(g_PageHwnd[1], &pt);
					ShowTooltip(g_ButtonHwnd1_OpenSchtasks, NULL, pt, TEXT("目标程序只找键值为RunAsInvoker的"), TEXT("提示"), true, false, TTI_INFO_LARGE, 3000);
					break;
				}
			}
			break;
		}
		case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP)wParam;
			wchar_t filePath[MAX_PATH];
			DragQueryFile(hDrop, 0, filePath, MAX_PATH);
			ProcessFile2(filePath);
			DragFinish(hDrop);//释放资源
			break;
		}
	}
	return CallWindowProc(g_OldPage2Proc, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK Page3Proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			HGDIOBJ OldFont;
			OldFont = SelectObject(hdc, g_FontBold);

			RECT rc;
			GetClientRect(hwnd, &rc);
			FillRect(hdc, &rc, BackgroundBrush);

			RECT RectDrawText;
			RectDrawText.left = g_FontSize.cx * 8;
			RectDrawText.right = g_PageRect.right - g_FontSize.cx * 2;

			RectDrawText.top = g_LineAnchorY[0];
			RectDrawText.bottom = RectDrawText.top + g_FontSize.cy * 1.3;
			DrawText(hdc, TEXT("UACSkip - UAC白名单工具"), -1, &RectDrawText, DT_LEFT | DT_SINGLELINE);
			SelectObject(hdc, g_Font);
			DrawIconEx(hdc, g_FontSize.cx * 2, g_FontSize.cy, g_hIcon, g_FontSize.cx * 4, g_FontSize.cx * 4, 0, NULL, DI_NORMAL);

			RectDrawText.top = g_LineAnchorY[1];
			RectDrawText.bottom = RectDrawText.top + g_FontSize.cy * 1.3;
			DrawText(hdc, TEXT("版本：5.1.0.0 - Beta            日期：2025.7.23"), -1, &RectDrawText, DT_LEFT | DT_SINGLELINE);
			MoveToEx(hdc, g_PageRect.left + g_FontSize.cx * 2, g_FontSize.cy + g_FontSize.cx * 4 + g_FontSize.cy, NULL);
			LineTo(hdc, g_PageRect.right - g_FontSize.cx * 2, g_FontSize.cy + g_FontSize.cx * 4 + g_FontSize.cy);
			
			RectDrawText.left = g_PageRect.left + g_FontSize.cx * 2;
			RectDrawText.right = g_PageRect.right - g_FontSize.cx * 2;
			RectDrawText.top = g_FontSize.cy + g_FontSize.cx * 4 + g_FontSize.cy + g_FontSize.cy;
			RectDrawText.bottom = RectDrawText.top + g_FontSize.cy * 1.3;
			DrawText(hdc, TEXT("UACSkip是个免费软件，您可以自由地分发、复制与运行于任何非商业用途"), -1, &RectDrawText, DT_LEFT | DT_SINGLELINE);

			EndPaint(hwnd, &ps);
			SelectObject(hdc, OldFont);
			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			HWND hwndStatic = (HWND)lParam;
			return (INT_PTR)BackgroundBrush;
		}
		case WM_NOTIFY:
		{
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			if (lpnmh->code == NM_CLICK || lpnmh->code == NM_RETURN)
			{
				if (lpnmh->idFrom == ID_LNK3) // ID 匹配
				{
					NMLINK* pNMLink = (NMLINK*)lParam;
					ShellExecute(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
				}
			}
			break;
		}
	}

	return CallWindowProc(g_OldPage3Proc, hwnd, message, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		
		SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, g_DesktopPath);
		BackgroundBrush = GetStockBrush(WHITE_BRUSH);
		HDC hdc = GetDC(hwnd);
		g_Font = CreateFont(
			-g_ClientSize.cy / 24,
			0,
			0,
			0,
			FW_NORMAL,
			false,
			false,
			false,
			GB2312_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			FF_SWISS,
			TEXT("微软雅黑")
		);
		g_EditFont = CreateFont(
			-g_ClientSize.cy / 27,
			0,
			0,
			0,
			FW_NORMAL,
			false,
			false,
			false,
			GB2312_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			FF_SWISS,
			TEXT("微软雅黑")
		);

		g_FontBold = CreateFont(
			-g_ClientSize.cy / 18,
			0,
			0,
			0,
			FW_BOLD,
			false,
			false,
			false,
			GB2312_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			FF_SWISS,
			TEXT("微软雅黑")
		);


		HGDIOBJ OldFont = SelectObject(hdc, g_Font);
		GetTextExtentPoint32(hdc, TEXT("中"), 1, &g_FontSize);
		
		SelectObject(hdc, OldFont);
		g_hIcon = (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 256, 256, LR_DEFAULTCOLOR);
		
		SIZE size1, size2;
		GetTextExtentPoint32(hdc, TEXT("本软件由 "), 5, &size1);
		GetTextExtentPoint32(hdc, TEXT("Bilibili-个人隐思"), 13, &size2);


		g_LineAnchorY[0] = g_FontSize.cy;
		int LineAnchorYStep = g_FontSize.cy * 1.8;
		for (int i = 1; i < 7; ++i)
		{
			g_LineAnchorY[i] = g_LineAnchorY[i - 1] + LineAnchorYStep;
		}

		g_TabHwnd = CreateWindowEx(
			NULL,
			TEXT("SysTabControl32"),
			NULL,
			WS_VISIBLE | WS_CHILD,
			0,
			0,
			g_ClientSize.cx - 2 * g_FontSize.cx,
			g_ClientSize.cy - 1.8 * g_FontSize.cy,
			hwnd,
			(HMENU)ID_TAB,
			NULL,
			NULL
		);
		SendMessage(g_TabHwnd, WM_SETFONT, (WPARAM)g_Font, 0);
		g_OldTabProc = (WNDPROC)SetWindowLongPtr(g_TabHwnd, GWLP_WNDPROC, (LONG_PTR)TabProc);
		CenterTabControl(hwnd, g_TabHwnd);


		//标签页结构
		TCITEM tie = {};
		TCHAR TabText[10] = TEXT("任务计划模式");
		tie.mask = TCIF_TEXT;//编辑选项卡文本模式
		tie.pszText = TabText;
		TabCtrl_InsertItem(g_TabHwnd, 0, &tie);//插入指定位置
		lstrcpyW(TabText, TEXT("注册表模式"));
		TabCtrl_InsertItem(g_TabHwnd, 1, &tie);//插入指定位置
		lstrcpyW(TabText, TEXT("关于"));
		TabCtrl_InsertItem(g_TabHwnd, 2, &tie);//插入指定位置


		//计算Page的尺寸
		RECT rect = {};
		TabCtrl_AdjustRect(g_TabHwnd, true, &rect);
		ReleaseDC(hwnd, hdc);
		LONG width, height, headheight;
		headheight = rect.bottom - rect.top;
		
		POINT pt;
		GetWindowRect(g_TabHwnd, &g_PageRect);
		width = g_PageRect.right - g_PageRect.left;
		height = g_PageRect.bottom - g_PageRect.top;
		pt.x = g_PageRect.left;
		pt.y = g_PageRect.top + rect.bottom - rect.top;
		ScreenToClient(hwnd, &pt);
		g_PageRect.left = pt.x + 5;
		g_PageRect.top = pt.y;
		g_PageRect.right = pt.x + width - 5;
		g_PageRect.bottom = pt.y + height - headheight - 5;

		g_PageHwnd[0] = CreateWindowEx(
			NULL,
			TEXT("static"),
			NULL,
			WS_VISIBLE | WS_CHILD | SS_NOTIFY,
			g_PageRect.left,
			g_PageRect.top,
			g_PageRect.right - g_PageRect.left,
			g_PageRect.bottom - g_PageRect.top,
			hwnd, NULL, NULL, NULL
		);
		g_OldPage1Proc = (WNDPROC)SetWindowLongPtr(g_PageHwnd[0], GWLP_WNDPROC, (LONG_PTR)Page1Proc);
		SetWindowPos(g_PageHwnd[0], HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOACTIVATE);
		DragAcceptFiles(g_PageHwnd[0], true);
		ChangeWindowMessageFilterEx(g_PageHwnd[0], WM_DROPFILES, MSGFLT_ALLOW, NULL);
		ChangeWindowMessageFilterEx(g_PageHwnd[0], 0x0049, MSGFLT_ALLOW, NULL);//这句不详，但是没它不行

		g_PageHwnd[1] = CreateWindowEx(
			NULL,
			TEXT("static"),
			NULL,
			WS_CHILD | SS_NOTIFY,
			g_PageRect.left,
			g_PageRect.top,
			g_PageRect.right - g_PageRect.left,
			g_PageRect.bottom - g_PageRect.top,
			hwnd, NULL, NULL, NULL
		);
		g_OldPage2Proc = (WNDPROC)SetWindowLongPtr(g_PageHwnd[1], GWLP_WNDPROC, (LONG_PTR)Page2Proc);
		SetWindowPos(g_PageHwnd[1], HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOACTIVATE);
		DragAcceptFiles(g_PageHwnd[1], true);
		ChangeWindowMessageFilterEx(g_PageHwnd[1], WM_DROPFILES, MSGFLT_ALLOW, NULL);
		ChangeWindowMessageFilterEx(g_PageHwnd[1], 0x0049, MSGFLT_ALLOW, NULL);//这句不详，但是没它不行

		g_PageHwnd[2] = CreateWindowEx(
			NULL,
			TEXT("static"),
			NULL,
			WS_CHILD,
			g_PageRect.left,
			g_PageRect.top,
			g_PageRect.right - g_PageRect.left,
			g_PageRect.bottom - g_PageRect.top,
			hwnd, NULL, NULL, NULL
		);
		g_OldPage3Proc = (WNDPROC)SetWindowLongPtr(g_PageHwnd[2], GWLP_WNDPROC, (LONG_PTR)Page3Proc);
		SetWindowPos(g_PageHwnd[2], HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOACTIVATE);

		g_HelpRect.left = g_PageRect.right - g_FontSize.cx * 5;
		g_HelpRect.right = g_HelpRect.left + g_FontSize.cx * 3;
		g_HelpRect.top = g_LineAnchorY[0] - g_FontSize.cy * 0.2;
		g_HelpRect.bottom = g_HelpRect.top + g_FontSize.cy * 1.5;

		g_ButtonHwnd1_Install= CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("安装"),
			WS_VISIBLE | WS_CHILD,
			g_FontSize.cx * 13,
			g_LineAnchorY[0] - g_FontSize.cy * 0.4,
			g_FontSize.cx * 3,
			g_FontSize.cy * 1.5,
			g_PageHwnd[0],
			(HMENU)ID_BTN1_Install,
			NULL,
			NULL
		);
		SendMessage(g_ButtonHwnd1_Install, WM_SETFONT, (WPARAM)g_EditFont, 0);
		CreatePointToolTip(hwnd, g_ButtonHwnd1_Install, TEXT("此操作会添加一个任务计划(路径\\UACSkip\\UACSkip)和一个环境变量UACSkip\n安装后可点击“打开任务计划”按钮和系统环境变量窗口查看"), TEXT("提示"), true, TTI_NONE, NULL, 5000);

		g_ButtonHwnd1_Uninstall = CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("卸载"),
			WS_VISIBLE | WS_CHILD,
			g_FontSize.cx * 17,
			g_LineAnchorY[0] - g_FontSize.cy * 0.4,
			g_FontSize.cx * 3,
			g_FontSize.cy * 1.5,
			g_PageHwnd[0],
			(HMENU)ID_BTN1_Uninstall,
			NULL,
			NULL
		);
		SendMessage(g_ButtonHwnd1_Uninstall, WM_SETFONT, (WPARAM)g_EditFont, 0);
		CreatePointToolTip(hwnd, g_ButtonHwnd1_Uninstall, TEXT("此操作会删除整个\\UACSkip任务计划文件夹和环境变量UACSkip\n卸载后可点击“打开任务计划”按钮和系统环境变量窗口查看"), TEXT("提示"), true, TTI_NONE, NULL, 5000);

		g_ButtonHwnd1_OpenSchtasks = CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("打开任务计划程序"),
			WS_VISIBLE | WS_CHILD,
			g_FontSize.cx * 21,
			g_LineAnchorY[0] - g_FontSize.cy * 0.4,
			g_FontSize.cx * 9,
			g_FontSize.cy * 1.5,
			g_PageHwnd[0],
			(HMENU)ID_BTN1_OpenSchtasks,
			NULL,
			NULL
		);
		SendMessage(g_ButtonHwnd1_OpenSchtasks, WM_SETFONT, (WPARAM)g_EditFont, 0);

		g_ButtonHwnd1_OpenEnvironmentVariable = CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("打开环境变量界面"),
			WS_VISIBLE | WS_CHILD,
			g_FontSize.cx * 31,
			g_LineAnchorY[0] - g_FontSize.cy * 0.4,
			g_FontSize.cx * 9,
			g_FontSize.cy * 1.5,
			g_PageHwnd[0],
			(HMENU)ID_BTN1_OpenVironment,
			NULL,
			NULL
		);
		SendMessage(g_ButtonHwnd1_OpenEnvironmentVariable, WM_SETFONT, (WPARAM)g_EditFont, 0);

		TaskSchedulerHelper t;
		if (t.TaskExists(L"\\UACSkip", L"UACSkip"))
		{
			wchar_t Variable[MAX_PATH];
			if (!CheckAndUpdateTaskAction(L"\\UACSkip\\UACSkip", argv[0], L"1"))
			{
				POINT pt = { g_FontSize.cx * 11,(LONG)(g_LineAnchorY[0] + g_FontSize.cy * 0.5) };
				ClientToScreen(g_PageHwnd[0], &pt);
				ShowTooltip(g_PageHwnd[0], &g_TipHwnd_TaskInstation, pt, TEXT("检测到已存在服务但参数与预期不符\n可能原因:遭到篡改或者安装服务后本程序遭到移动\n需要重新安装"), TEXT("警告"), true, true, TTI_INFO_LARGE, 8000);
				goto error;
			}
			if (!GetEnvironmentVariable(L"UACSkip", Variable, MAX_PATH) || lstrcmpW(Variable,argv[0]) != 0)
			{
				VariableStatus = false;
				POINT pt = { g_FontSize.cx * 11,(LONG)(g_LineAnchorY[0] + g_FontSize.cy * 0.5) };
				ClientToScreen(g_PageHwnd[0], &pt);
				ShowTooltip(g_PageHwnd[0], &g_TipHwnd_TaskInstation, pt, TEXT("检测到已存在服务但环境变量与预期不符\n可能原因:遭到篡改或者安装服务后本程序遭到移动\n需要重新安装"), TEXT("警告"), true, true, TTI_INFO_LARGE, 8000);
				goto error;
			}
			VariableStatus = true;
			g_StaticHwnd1_Status = CreateWindowEx(
				NULL,
				TEXT("static"),
				TEXT("已安装"),
				WS_VISIBLE | WS_CHILD | SS_LEFT,
				g_FontSize.cx * 9,
				g_LineAnchorY[0] - g_FontSize.cy * 0.2,
				g_FontSize.cx * 3,
				g_FontSize.cy * 1.3,
				g_PageHwnd[0],
				NULL,
				NULL,
				NULL
			);
			TaskInstalled = true;
			EnableWindow(g_ButtonHwnd1_Install, false);

		}
		else
		{
			error:
			g_StaticHwnd1_Status = CreateWindowEx(
				NULL,
				TEXT("static"),
				TEXT("未安装"),
				WS_VISIBLE | WS_CHILD | SS_LEFT,
				g_FontSize.cx * 9,
				g_LineAnchorY[0] - g_FontSize.cy * 0.2,
				g_FontSize.cx * 3,
				g_FontSize.cy * 1.3,
				g_PageHwnd[0],
				NULL,
				NULL,
				NULL
			);
			TaskInstalled = false;
			EnableWindow(g_ButtonHwnd1_Uninstall, false);

		}
		SendMessage(g_StaticHwnd1_Status, WM_SETFONT, (WPARAM)g_Font, 0);

		


		//g_O1ldStatusProc = (WNDPROC)SetWindowLongPtr(g_StaticHwnd1_Status, GWLP_WNDPROC, (LONG_PTR)StatusProc);

		GetClientRect(g_PageHwnd[0], &g_PageRect);
		g_EditHwnd1_Path = CreateWindowEx(
			NULL,
			TEXT("Edit"),
			NULL,
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
			g_FontSize.cx * 8.5,
			g_LineAnchorY[1] - g_FontSize.cy * 0.2,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 8.5 - g_FontSize.cx * 5,
			g_FontSize.cy * 1.4,
			g_PageHwnd[0],
			NULL,
			NULL,
			NULL
		);
		SendMessage(g_EditHwnd1_Path, WM_SETFONT, (WPARAM)g_EditFont, 0);
		g_ButtonHwnd1_Path = CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("浏览"),
			WS_VISIBLE | WS_CHILD,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 4.5,
			g_LineAnchorY[1] - g_FontSize.cy * 0.2,
			g_FontSize.cx * 3,
			g_FontSize.cy * 1.5,
			g_PageHwnd[0],
			(HMENU)ID_BTN1_Path,
			NULL,
			NULL
		);
		//SendMessage(g_ButtonHwnd1_Path, BCM_SETSHIELD, 0, TRUE);
		SendMessage(g_ButtonHwnd1_Path, WM_SETFONT, (WPARAM)g_EditFont, 0);
		
		g_EditHwnd1_Name = CreateWindowEx(
			NULL,
			TEXT("Edit"),
			NULL,
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
			g_FontSize.cx * 8.5,
			g_LineAnchorY[2] - g_FontSize.cy * 0.2,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 8.5 - g_FontSize.cx * 5,
			g_FontSize.cy * 1.4,
			g_PageHwnd[0],
			NULL,
			NULL,
			NULL
		);
		SendMessage(g_EditHwnd1_Name, WM_SETFONT, (WPARAM)g_EditFont, 0);


		g_EditHwnd1_StartPath = CreateWindowEx(
			NULL,
			TEXT("Edit"),
			NULL,
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
			g_FontSize.cx * 8.5,
			g_LineAnchorY[3] - g_FontSize.cy * 0.2,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 8.5 - g_FontSize.cx * 5,
			g_FontSize.cy * 1.4,
			g_PageHwnd[0],
			NULL,
			NULL,
			NULL
		);
		SendMessage(g_EditHwnd1_StartPath, EM_SETCUEBANNER, TRUE, (LPARAM)L"可选");
		SendMessage(g_EditHwnd1_StartPath, WM_SETFONT, (WPARAM)g_EditFont, 0);
		g_ButtonHwnd1_StartPath = CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("浏览"),
			WS_VISIBLE | WS_CHILD,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 4.5,
			g_LineAnchorY[3] - g_FontSize.cy * 0.2,
			g_FontSize.cx * 3,
			g_FontSize.cy * 1.5,
			g_PageHwnd[0],
			(HMENU)ID_BTN1_StartPath,
			NULL,
			NULL
		);
		SendMessage(g_ButtonHwnd1_StartPath, WM_SETFONT, (WPARAM)g_EditFont, 0);

		g_EditHwnd1_Args = CreateWindowEx(
			NULL,
			TEXT("Edit"),
			NULL,
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
			g_FontSize.cx * 8.5,
			g_LineAnchorY[4] - g_FontSize.cy * 0.2,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 8.5 - g_FontSize.cx * 5,
			g_FontSize.cy * 1.4,
			g_PageHwnd[0],
			NULL,
			NULL,
			NULL
		);
		SendMessage(g_EditHwnd1_Args, WM_SETFONT, (WPARAM)g_EditFont, 0);
		SendMessage(g_EditHwnd1_Args, EM_SETCUEBANNER, TRUE, (LPARAM)L"可选");

		g_EditHwnd1_OutputLnk = CreateWindowEx(
			NULL,
			TEXT("Edit"),
			NULL,
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
			g_FontSize.cx * 8.5,
			g_LineAnchorY[5] - g_FontSize.cy * 0.2,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 8.5 - g_FontSize.cx * 5,
			g_FontSize.cy * 1.4,
			g_PageHwnd[0],
			NULL,
			NULL,
			NULL
		);
		SendMessage(g_EditHwnd1_OutputLnk, EM_SETCUEBANNER, TRUE, (LPARAM)L"默认为桌面");
		SendMessage(g_EditHwnd1_OutputLnk, WM_SETFONT, (WPARAM)g_EditFont, 0);

		g_ButtonHwnd1_OutputLnk = CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("浏览"),
			WS_VISIBLE | WS_CHILD,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 4.5,
			g_LineAnchorY[5] - g_FontSize.cy * 0.2,
			g_FontSize.cx * 3,
			g_FontSize.cy * 1.5,
			g_PageHwnd[0],
			(HMENU)ID_BTN1_Output,
			NULL,
			NULL
		);
		SendMessage(g_ButtonHwnd1_OutputLnk, WM_SETFONT, (WPARAM)g_EditFont, 0);
		g_ButtonHwnd1_Execute = CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("创建快捷方式"),
			WS_VISIBLE | WS_CHILD,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 9,
			g_LineAnchorY[6],
			g_FontSize.cx * 7,
			g_FontSize.cy * 2,
			g_PageHwnd[0],
			(HMENU)ID_BTN1_Execute,
			NULL,
			NULL
		);
		SendMessage(g_ButtonHwnd1_Execute, WM_SETFONT, (WPARAM)g_EditFont, 0);
		
		g_ButtonHwnd1_ContextMenu = CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("添加右键菜单"),
			WS_VISIBLE | WS_CHILD,
			g_FontSize.cx * 3,
			g_LineAnchorY[6],
			g_FontSize.cx * 7,
			g_FontSize.cy * 2,
			g_PageHwnd[0],
			(HMENU)ID_BTN1_ContextMenu,
			NULL,
			NULL
		);
		SendMessage(g_ButtonHwnd1_ContextMenu, WM_SETFONT, (WPARAM)g_EditFont, 0);
		CreatePointToolTip(g_PageHwnd[0], g_ButtonHwnd1_ContextMenu, TEXT("点此将在注册表HKEY_CLASSES_ROOT\\exefile\\shell处\n新建/移除\"UACSkip\"项"), TEXT("提示"), true, TTI_NONE, NULL, NULL);
		if (TaskInstalled)
		{
			RegInstalled = CheckReg();
			if (RegInstalled == -1)
			{
				SetWindowText(g_ButtonHwnd1_ContextMenu, TEXT("修复右键菜单"));
				POINT pt = { g_FontSize.cx * 6,g_LineAnchorY[6] + g_FontSize.cy };
				ClientToScreen(g_PageHwnd[0], &pt);
				ShowTooltip(g_PageHwnd[0], &g_TipHwnd_RegInstation, pt, TEXT("检测到右键注册表参数与实际不符，点此修复"), TEXT("提示"), true, true, TTI_INFO_LARGE, 5000);
			}
			else if (RegInstalled == 1)
			{
				SetWindowText(g_ButtonHwnd1_ContextMenu, TEXT("移除右键菜单"));
			}
		}
		else
		{
			EnableWindow(g_ButtonHwnd1_ContextMenu, false);
		}


		
		g_EditHwnd2_Path = CreateWindowEx(
			NULL,
			TEXT("Edit"),
			NULL,
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
			g_FontSize.cx * 8.5,
			g_LineAnchorY[1] - g_FontSize.cy * 0.2,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 8.5 - g_FontSize.cx * 5,
			g_FontSize.cy * 1.4,
			g_PageHwnd[1],
			NULL,
			NULL,
			NULL
		);
		SendMessage(g_EditHwnd2_Path, WM_SETFONT, (WPARAM)g_EditFont, 0);

		g_ButtonHwnd2_Path = CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("浏览"),
			WS_VISIBLE | WS_CHILD,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 4.5,
			g_LineAnchorY[1] - g_FontSize.cy * 0.2,
			g_FontSize.cx * 3,
			g_FontSize.cy * 1.5,
			g_PageHwnd[1],
			(HMENU)ID_BTN2_Path,
			NULL,
			NULL
		);
		SendMessage(g_ButtonHwnd2_Path, WM_SETFONT, (WPARAM)g_EditFont, 0);

		g_ButtonHwnd2_Execute = CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("添加白名单项"),
			WS_VISIBLE | WS_CHILD,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 9,
			g_LineAnchorY[6],
			g_FontSize.cx * 7,
			g_FontSize.cy * 2,
			g_PageHwnd[1],
			(HMENU)ID_BTN2_Execute,
			NULL,
			NULL
		);
		SendMessage(g_ButtonHwnd2_Execute, WM_SETFONT, (WPARAM)g_EditFont, 0);

		/*g_StaticHwnd2_Help = CreateWindowEx(
			NULL,
			TEXT("static"),
			TEXT("帮助"),
			WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER | SS_NOTIFY,
			g_PageRect.right - g_PageRect.left - g_FontSize.cx * 4.5,
			g_LineAnchorY[0] - g_FontSize.cy * 0.5,
			g_FontSize.cx * 3,
			g_FontSize.cy * 1.5,
			g_PageHwnd[1],
			NULL,
			NULL,
			NULL
		);
		SendMessage(g_StaticHwnd2_Help, WM_SETFONT, (WPARAM)g_EditFont, 0);*/

		g_ButtonHwnd2_OpenRegedit = CreateWindowEx(
			NULL,
			TEXT("Button"),
			TEXT("打开注册表编辑器"),
			WS_VISIBLE | WS_CHILD,
			g_FontSize.cx * 21,
			g_LineAnchorY[0] - g_FontSize.cy * 0.4,
			g_FontSize.cx * 9,
			g_FontSize.cy * 1.5,
			g_PageHwnd[1],
			(HMENU)ID_BTN1_OpenRegedit,
			NULL,
			NULL
		);
		SendMessage(g_ButtonHwnd2_OpenRegedit, WM_SETFONT, (WPARAM)g_EditFont, 0);


		CreatePointToolTip(g_PageHwnd[0], NULL, TEXT("0.先安装任务计划服务(若移动本程序需重新更新安装)\n1.选取欲加入UAC白名单的程序\n2.重命名待生成的快捷方式名\n3.选填剩下三项后创建快捷方式"), TEXT("提示"), true, TTI_INFO_LARGE, NULL, 8000, &g_HelpRect);
		CreatePointToolTip(g_PageHwnd[1], NULL, TEXT("1.添加:选取一个exe再点击\"添加白名单项\"按钮\n2.删除:点击\"打开注册表\"找到目标程序并删除该项"), TEXT("提示"), true, TTI_INFO_LARGE, NULL, 8000, &g_HelpRect);

		INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_LINK_CLASS };
		InitCommonControlsEx(&icex); // 初始化SysLink


		g_LinkHwnd3_Home = CreateWindowEx(
			NULL, L"SysLink",
			L"本软件由 <a href=\"https://space.bilibili.com/1081364881\">Bilibili-个人隐思</a> 用心打造",
			WS_CHILD | WS_VISIBLE,
			g_PageRect.left + g_FontSize.cx * 2,
			g_FontSize.cy + g_FontSize.cx * 4 + g_FontSize.cy + g_FontSize.cy + g_FontSize.cy * 2,
			g_PageRect.right,
			g_FontSize.cy * 1.5,
			g_PageHwnd[2],
			(HMENU)ID_LNK3,
			NULL,
			NULL
		);
		SendMessage(g_LinkHwnd3_Home, WM_SETFONT, (WPARAM)g_Font, 0);

		g_LinkHwnd3_Patron = CreateWindowEx(
			NULL, L"SysLink",
			L"如果觉得对您有所帮助，不妨在 <a href=\"https://afdian.com/a/X1415\">爱发电</a> 赞助一下我，以支持我做出更多优质作品",
			WS_CHILD | WS_VISIBLE,
			g_PageRect.left + g_FontSize.cx * 2,
			g_FontSize.cy + g_FontSize.cx * 4 + g_FontSize.cy + g_FontSize.cy + g_FontSize.cy * 2 + g_FontSize.cy * 2,
			g_PageRect.right,
			g_FontSize.cy * 1.5,
			g_PageHwnd[2],
			(HMENU)ID_LNK3,
			NULL,
			NULL
		);
		SendMessage(g_LinkHwnd3_Patron, WM_SETFONT, (WPARAM)g_Font, 0);
		break;
	}
	case WM_PAINT:
	{
		ForceSetForeground(g_MainHwnd);
		SetWindowPos(g_MainHwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
		break;
	}
	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->idFrom == ID_TAB && ((LPNMHDR)lParam)->code == TCN_SELCHANGE)
		{
			ShowWindow(g_PageHwnd[g_SelectPage], SW_HIDE);
			g_SelectPage = TabCtrl_GetCurSel(g_TabHwnd);
			if (g_TipHwnd_TaskInstation)
			{
				if (g_SelectPage != 0)
				{
					ShowWindow(g_TipHwnd_TaskInstation, SW_HIDE);
				}
				else
				{
					ShowWindow(g_TipHwnd_TaskInstation, SW_NORMAL);
				}
			}
			if (g_TipHwnd_RegInstation)
			{
				if (g_SelectPage != 0)
				{
					ShowWindow(g_TipHwnd_RegInstation, SW_HIDE);
				}
				else
				{
					ShowWindow(g_TipHwnd_RegInstation, SW_NORMAL);
				}
			}
			ShowWindow(g_PageHwnd[g_SelectPage], SW_NORMAL);
		}

		break;
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize.x = 170; // 设置最小宽度
		mmi->ptMinTrackSize.y = 96; // 设置最小高度
		return 0;
	}

	case WM_CLOSE:
	{
		break;
	}
	case WM_DESTROY:
		DeleteObject(g_FontBold);
		DeleteObject(g_EditFont);
		DeleteObject(g_Font);
		DeleteObject(g_hIcon);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}



void Config()
{
	WNDCLASSEX wndclassex = {};
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = NULL;
	wndclassex.lpfnWndProc = WndProc;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = g_hInstance;
	wndclassex.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclassex.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wndclassex.lpszMenuName = NULL;
	wndclassex.lpszClassName = TEXT("UACSkip");
	wndclassex.hIconSm = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1));
	if (!RegisterClassEx(&wndclassex))
	{
		MessageBox(NULL, TEXT("这个程序需要在Windows NT才能执行！"), TEXT("UACSkip白名单工具"), MB_ICONERROR | MB_OK);
		return;
	}
	SIZE WindowSize = AdjustClientSize(18, 9, 16, true, &g_ClientSize, WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, NULL, NULL);
	g_MainHwnd = CreateWindowEx(
		WS_EX_ACCEPTFILES,
		TEXT("UACSkip"),
		TEXT("UACSkip白名单工具"),
		WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		(GetSystemMetrics(SM_CXSCREEN) - WindowSize.cx) >> 1,
		(GetSystemMetrics(SM_CYSCREEN) - WindowSize.cy) >> 1,
		WindowSize.cx,
		WindowSize.cy,
		NULL,
		NULL,
		g_hInstance,
		NULL
	);
	ShowWindow(g_MainHwnd, g_iCmdShow);
	UpdateWindow(g_MainHwnd);

	MSG msg;
#ifdef AcceleratorsEnable
	HACCEL haccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(g_MainHwnd, haccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
#else
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#endif
}
