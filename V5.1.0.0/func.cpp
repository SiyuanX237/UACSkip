#define _CRT_SECURE_NO_WARNINGS
#include "��ͷ.h"

//��ʾ����̨
void ShowConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	cin.clear();
	cout.clear();
}


//�򿪼ƻ����������
void OpenTask()
{
	ShellExecute(NULL, L"open", L"taskschd.msc", NULL, NULL, SW_NORMAL);
	return;
}


//����ģʽ
void Execute()
{
	
	if (argc == 2 && argv[1][0] == L'1')
	{
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
		fp = _wfopen(path, L"r");
		if (fp == nullptr)
		{
			return;
		}

		wstring command;
		wstring Program;
		wchar_t line[4096];
		fgetws(line, MAX_PATH, fp);
		path_length = wcslen(line) - 1;
		while (path_length > 0 && (line[path_length] == L'\n' || line[path_length] == L'\r'))line[path_length--] = L'\0';
		Program = wstr_format(L"\"%ws\"", line);
		while (fgetws(line, 4096, fp))
		{
			command += L' ';
			path_length = wcslen(line) - 1;//���ñ���
			while (path_length > 0 && (line[path_length] == L'\n' || line[path_length] == L'\r'))line[path_length--] = L'\0';		
			command += line;

		}
		fclose(fp);

		ForceSetForeground(g_MainHwnd);
		SHELLEXECUTEINFO sei = { 0 };
		sei.cbSize = sizeof(sei);
		sei.lpVerb = L"runas"; // ��Ȩ
		sei.lpFile = Program.c_str();
		sei.lpParameters = command.c_str();
		sei.nShow = SW_SHOWNORMAL;
		ShellExecuteEx(&sei);
	}
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
	if (lstrcmp(result.c_str(), L".lnk") == 0)
	{
		isLnk = true;
		if (!ResolveShortcut(filePath, filePath, StartPath, Args))
		{
			MessageBox(g_MainHwnd, TEXT("�Ƿ���lnk"), TEXT("��ʾ"), MB_OK | MB_ICONERROR);
		}
		result = CheckPathType(filePath);
	}

	if (result.empty() || result[0] == L'D' ||
		(lstrcmp(result.c_str(), L".exe") && lstrcmp(result.c_str(), L".bat"))
		)
	{
		MessageBox(g_MainHwnd, TEXT("����Ϊexe��bat����"), TEXT("��ʾ"), MB_OK | MB_ICONINFORMATION);
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
		if (!ResolveShortcut(filePath, filePath, NULL, NULL))
		{
			MessageBox(g_MainHwnd, TEXT("�Ƿ���lnk"), TEXT("��ʾ"), MB_OK | MB_ICONERROR);
		}
		result = CheckPathType(filePath);
	}

	if (result.empty() || result[0] == L'D' ||lstrcmp(result.c_str(), L".exe"))
	{
		MessageBox(g_MainHwnd, TEXT("����Ϊexe����"), TEXT("��ʾ"), MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		SetWindowText(g_EditHwnd2_Path, filePath);
	}
	return;
}

BOOL CheckReg()//1Ϊ����,0Ϊ������,-1Ϊ��Ҫ����
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
		(Path.size() + 1)*sizeof(wchar_t)  // ���� null ��ֹ���Ĵ�С
	);
	if (result != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return false;
	}

	Path = L"�Թ���Ա�������(&U)";
	result = RegSetValueEx(
		hKey,
		TEXT("MUIVerb"),
		0,
		REG_SZ,
		(BYTE*)Path.c_str(),
		(Path.size() + 1) * sizeof(wchar_t)  // ���� null ��ֹ���Ĵ�С
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
		(Path.size() + 1) * sizeof(wchar_t)  // ���� null ��ֹ���Ĵ�С
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
	// ��ȡ�����ڿͻ�����С
	RECT rcParent;
	GetClientRect(hWndParent, &rcParent);

	// ��ȡ Tab �ؼ�����Ĵ�С
	RECT rcTab;
	GetWindowRect(hWndTab, &rcTab);
	// ת��Ϊ�����ڵĿͻ�������
	MapWindowPoints(HWND_DESKTOP, hWndParent, (LPPOINT)&rcTab, 2);

	int tabWidth = rcTab.right - rcTab.left;
	int tabHeight = rcTab.bottom - rcTab.top;

	int newX = (rcParent.right - tabWidth) / 2;
	int newY = (rcParent.bottom - tabHeight) / 2;

	MoveWindow(hWndTab, newX, newY, tabWidth, tabHeight, TRUE);
}


bool CheckAndUpdateTaskAction(
	const std::wstring& taskPath,
	const std::wstring& expectedProgram,
	const std::wstring& expectedArgs,
	const std::wstring& expectedFolder
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

	// ��ȡ����
	CComPtr<ITaskDefinition> pDef;
	hr = pTask->get_Definition(&pDef);
	if (FAILED(hr)) return false;

	// ��ȡ��������
	CComPtr<IActionCollection> pActions;
	hr = pDef->get_Actions(&pActions);
	if (FAILED(hr)) return false;

	LONG count = 0;
	pActions->get_Count(&count);
	if (count == 0) return false;

	// ����һ������
	CComPtr<IAction> pAction;
	hr = pActions->get_Item(1, &pAction);  // 1-based
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
	return ((expectedProgram == std::wstring(path)) && (expectedArgs == std::wstring(args))) ? true : false;
}