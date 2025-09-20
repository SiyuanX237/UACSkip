#define _CRT_SECURE_NO_WARNINGS
#include "标头.h"

//打开计划任务管理器
void OpenTask()
{
	ShellExecute(NULL, L"open", L"taskschd.msc", NULL, NULL, SW_NORMAL);
	return;
}

BOOL IsAdmin()
{
	BOOL isAdmin = FALSE;
	PSID adminGroup = NULL;

	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if (AllocateAndInitializeSid(&NtAuthority, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup))
	{

		CheckTokenMembership(NULL, adminGroup, &isAdmin);
		FreeSid(adminGroup);
	}

	return isAdmin;
}

void ProcessFile(wchar_t *filePath)
{
	wstring result = CheckPathType(filePath);
	wchar_t StartPath[MAX_PATH];
	wchar_t Args[4096];
	bool isLnk = false;
	int ShowCmd = SW_NORMAL;
	if (lstrcmp(result.c_str(), L".lnk") == 0)
	{
		isLnk = true;
		
		if(!ResolveShortcut(filePath, filePath, StartPath, Args, &ShowCmd))
		{
			MessageBox(g_MainHwnd, TEXT("非法的lnk"), TEXT("提示"), MB_OK | MB_ICONERROR);
		}
		result = CheckPathType(filePath);
	}

	if (result.empty() || result[0] == L'D')
	{
		MessageBox(g_MainHwnd, TEXT("必须为文件"), TEXT("提示"), MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		SetWindowText(g_EditHwnd1_Path, filePath);
		wchar_t LnkName[MAX_PATH];
		getfname(filePath, LnkName, NULL, NULL, NULL);
		int len = lstrlen(LnkName) - 1;
		for (; LnkName[len] != L'.'; --len)
		{

		}
		LnkName[len] = L'\0';

		SetWindowText(g_EditHwnd1_Name, LnkName);
		SetWindowText(g_EditHwnd1_StartPath, isLnk ? StartPath : NULL);
		SetWindowText(g_EditHwnd1_Args, isLnk ? Args : NULL);
		//转换ShowCmd为在组合框中的位置
		switch(ShowCmd)
		{
			case SW_NORMAL:
			default:
				ShowCmd = 0;
				break;
			case SW_SHOWMINNOACTIVE:
				ShowCmd = 1;
				break;
			case SW_SHOWMAXIMIZED:
				ShowCmd = 2;
				break;
		}
		SendMessageW(g_ComboHwnd1_CmdShow, CB_SETCURSEL, ShowCmd, 0);
	}
	return;

}

void ProcessFile2(wchar_t* filePath)
{
	wstring result = CheckPathType(filePath);
	bool isLnk = false;
	if (lstrcmp(result.c_str(), L".lnk") == 0)
	{
		isLnk = true;
		if (!ResolveShortcut(filePath, filePath, NULL, NULL, NULL))
		{
			MessageBox(g_MainHwnd, TEXT("非法的lnk"), TEXT("提示"), MB_OK | MB_ICONERROR);
		}
		result = CheckPathType(filePath);
	}

	if (result.empty() || result[0] == L'D' ||lstrcmp(result.c_str(), L".exe"))
	{
		MessageBox(g_MainHwnd, TEXT("必须为exe类型"), TEXT("提示"), MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		SetWindowText(g_EditHwnd2_Path, filePath);
	}
	return;
}

BOOL CheckReg()//1为正常,0为不存在,-1为需要更新
{
	HKEY hKey;
	DWORD dwType;
	TCHAR value[MAX_PATH];
	DWORD dwSize = MAX_PATH;
	LONG result;
	wstring Value;
	result = RegOpenKeyEx(
		HKEY_CLASSES_ROOT,
		TEXT("exefile\\shell\\UACSkip"),
		0,
		KEY_READ,
		&hKey
	);
	if (result != ERROR_SUCCESS)
	{
		return 0;
	}
	result = RegQueryValueEx(
		hKey,
		TEXT("MUIVerb"),
		NULL,
		&dwType,
		(LPBYTE)value,
		&dwSize
	);
	if (result != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return 0;
	}
	dwSize = MAX_PATH;
	result = RegQueryValueEx(
		hKey,
		TEXT("Icon"),
		NULL,
		&dwType,
		(LPBYTE)value,
		&dwSize
	);
	if (result != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return 0;
	}
	Value = wstr_format(L"%s,0", argv[0]);
	if (lstrcmp(Value.c_str(), value) != 0 || dwType != REG_SZ)
	{
		RegCloseKey(hKey);
		return -1;
	}
	RegCloseKey(hKey);

	result = RegOpenKeyEx(
		HKEY_CLASSES_ROOT,
		TEXT("exefile\\shell\\UACSkip\\command"),
		0,
		KEY_READ,
		&hKey
	);
	if (result != ERROR_SUCCESS)
	{
		return 0;
	}
	dwSize = MAX_PATH;
	result = RegQueryValueEx(
		hKey,
		TEXT(""),
		NULL,
		&dwType,
		(LPBYTE)value,
		&dwSize
	);
	if (result != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return 0;
	}

	Value = wstr_format(L"%s \"%%1\"", argv[0]);
	if (lstrcmp(Value.c_str(), value) != 0 || dwType != REG_SZ)
	{
		RegCloseKey(hKey);
		return -1;
	}
	RegCloseKey(hKey);
	return 1;
}

bool RegInstall()
{
	HKEY hKey;
	LONG result;
	wstring Path;
	result = RegCreateKeyExW(
		HKEY_CLASSES_ROOT,
		L"exefile\\shell\\UACSkip",
		0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
	if (result != ERROR_SUCCESS)
	{
		return false;
	}
	
	Path = wstr_format(L"%s,0", argv[0]);
	result = RegSetValueEx(
		hKey,
		TEXT("Icon"),
		0,
		REG_SZ,
		(BYTE*)Path.c_str(),
		(Path.size() + 1)*sizeof(wchar_t)  // 包括 null 终止符的大小
	);
	if (result != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return false;
	}

	Path = L"以管理员身份运行(&U)";
	result = RegSetValueEx(
		hKey,
		TEXT("MUIVerb"),
		0,
		REG_SZ,
		(BYTE*)Path.c_str(),
		(Path.size() + 1) * sizeof(wchar_t)  // 包括 null 终止符的大小
	);
	if (result != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return false;
	}
	RegCloseKey(hKey);

	result = RegCreateKeyExW(
		HKEY_CLASSES_ROOT,
		L"exefile\\shell\\UACSkip\\command",
		0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
	if (result != ERROR_SUCCESS)
	{
		return false;
	}

	Path = wstr_format(L"%s \"%%1\"", argv[0]);
	result = RegSetValueEx(
		hKey,
		TEXT(""),
		0,
		REG_SZ,
		(BYTE*)Path.c_str(),
		(Path.size() + 1) * sizeof(wchar_t)  // 包括 null 终止符的大小
	);
	if (result != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return false;
	}
	RegCloseKey(hKey);
	return true;
}

bool RegUninstall()
{
	LONG result = RegDeleteTreeW(HKEY_CLASSES_ROOT, L"exefile\\shell\\UACSkip");
	return (result == ERROR_SUCCESS) ? true : false;
}

void CenterTabControl(HWND hWndParent, HWND hWndTab)
{
	// 获取父窗口客户区大小
	RECT rcParent;
	GetClientRect(hWndParent, &rcParent);

	// 获取 Tab 控件本身的大小
	RECT rcTab;
	GetWindowRect(hWndTab, &rcTab);
	// 转换为父窗口的客户区坐标
	MapWindowPoints(HWND_DESKTOP, hWndParent, (LPPOINT)&rcTab, 2);

	int tabWidth = rcTab.right - rcTab.left;
	int tabHeight = rcTab.bottom - rcTab.top;

	int newX = (rcParent.right - tabWidth) / 2;
	int newY = (rcParent.bottom - tabHeight) / 2;

	MoveWindow(hWndTab, newX, newY, tabWidth, tabHeight, TRUE);
}

bool CheckAndUpdateTaskAction(
	const wstring& taskPath,
	const wstring& expectedProgram,
	const wstring& expectedArgs,
	const wstring& expectedFolder
)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) return false;

	CComPtr<ITaskService> pService;
	hr = pService.CoCreateInstance(__uuidof(TaskScheduler));
	if (FAILED(hr)) return false;

	hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr)) return false;

	CComPtr<ITaskFolder> pFolder;
	hr = pService->GetFolder(_bstr_t(expectedFolder.c_str()), &pFolder);
	if (FAILED(hr)) return false;

	CComPtr<IRegisteredTask> pTask;
	hr = pFolder->GetTask(_bstr_t(taskPath.c_str()), &pTask);
	if (FAILED(hr)) return false;

	// 获取定义
	CComPtr<ITaskDefinition> pDef;
	hr = pTask->get_Definition(&pDef);
	if (FAILED(hr)) return false;

	// 获取动作集合
	CComPtr<IActionCollection> pActions;
	hr = pDef->get_Actions(&pActions);
	if (FAILED(hr)) return false;

	LONG count = 0;
	pActions->get_Count(&count);
	if (count == 0) return false;

	// 检查第一个动作
	CComPtr<IAction> pAction;
	hr = pActions->get_Item(1, &pAction);
	if (FAILED(hr)) return false;

	TASK_ACTION_TYPE type;
	pAction->get_Type(&type);
	if (type != TASK_ACTION_EXEC) return false;

	CComPtr<IExecAction> pExec;
	hr = pAction->QueryInterface(IID_PPV_ARGS(&pExec));
	if (FAILED(hr)) return false;

	CComBSTR path, args;
	pExec->get_Path(&path);
	pExec->get_Arguments(&args);

	CoUninitialize();
	return ((expectedProgram == wstring(path)) && (expectedArgs == wstring(args))) ? true : false;
}