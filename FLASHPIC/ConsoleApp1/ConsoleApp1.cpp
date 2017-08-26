// ConsoleApp1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include <iostream>
//#include<stdio.h>
#include<stdlib.h> 

//#include<conio.h>  
#include<Windows.h>  

#define DEF_DLL_NAME "HookDll.dll"  
#define DEF_HOOKSTART "HookStart"  
#define DEF_HOOKSTOP "HookStop"  

typedef void(*PFN_HOOKSTART)(int threadId);
typedef void(*PFN_HOOKSTOP)();
#include<Windows.h>  
#include <Shlobj.h>
//#include <boost/tokenizer.hpp> 
#include <process.h>
#include <TlHelp32.h>
#include <tchar.h>

#define DEF_PROCESS_NAME "notepad++.exe"  

HINSTANCE g_hInstance = NULL;

HWND g_hWnd = NULL;

using namespace std;


#define  liuchang 555;


DWORD GetThreadIdByProcessID(DWORD dwProcessId)// 获取程序主线程ID
{
	HANDLE hThreadSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessId);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	THREADENTRY32 te32 = { sizeof(te32) };
	if (::Thread32First(hThreadSnap, &te32))
	{
		do
		{
			if (te32.th32OwnerProcessID == dwProcessId)
			{
				::CloseHandle(hThreadSnap);
				return te32.th32ThreadID;
			}
		} while (::Thread32Next(hThreadSnap, &te32));
	}

	::CloseHandle(hThreadSnap);
	return -1;
}

DWORD GetProcessIdByName(LPCWSTR processName)// 获取进程ID
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapShot, &pe))
	{
		return NULL;
	}
	BOOL clearprocess = FALSE;
	while (Process32Next(hSnapShot, &pe))
	{
	//	char* szStrProcessName = "aaaaa";
	//	char* szStr = "balabala";

	//	WCHAR wszClassName[256];
		//memset(wszClassName, 0, sizeof(wszClassName));
	//	MultiByteToWideChar(CP_ACP, 0, szStr, strlen(szStr) + 1, pe.szExeFile, sizeof(pe.szExeFile) / sizeof(pe.szExeFile[0]));

	//	wchar_t* pwszUnicode = L"Holle, word! 你好，中国！ ";
		int iSize;
		char* pszMultiByteprocessName;
		iSize = WideCharToMultiByte(CP_ACP, 0, processName, -1, NULL, 0, NULL, NULL);
		pszMultiByteprocessName = (char*)malloc(iSize * sizeof(char));
		WideCharToMultiByte(CP_ACP, 0, processName, -1, pszMultiByteprocessName, iSize, NULL, NULL);

		int iSize2;
		char* pszMultiBytePe;
		iSize2 = WideCharToMultiByte(CP_ACP, 0, pe.szExeFile, -1, NULL, 0, NULL, NULL);
		pszMultiBytePe = (char*)malloc(iSize2 * sizeof(char));
		WideCharToMultiByte(CP_ACP, 0, pe.szExeFile, -1, pszMultiBytePe, iSize2, NULL, NULL);


	//	MultiByteToWideChar(CP_ACP, 0, szStrProcessName, strlen(szStrProcessName) + 1, (LPWSTR)processName, sizeof(processName) / sizeof(pe.szExeFile[0]));
		if (!strcmp(pszMultiByteprocessName, pszMultiBytePe))
		{
			::CloseHandle(hSnapShot);
			return pe.th32ProcessID;
		}
	}
	::CloseHandle(hSnapShot);
	return -1;
}

int main()
{
	double isboo = liuchang;
	DWORD dwProcessId = GetProcessIdByName(TEXT("Tutorial07.exe") );
	if (dwProcessId == -1)
	{
	//	g_bThreadRunning = false;
		system("pause");
		return 0;
	}
	//DWORD dwThreadId = GetThreadIdByProcessID(dwProcessId);

	HMODULE hDLL = NULL;
	PFN_HOOKSTART HookStart = NULL;
	PFN_HOOKSTOP HookStop = NULL;

	char ch = 0;
	hDLL = LoadLibraryA(DEF_DLL_NAME);
	//获取导出函数地址  
	HookStart = (PFN_HOOKSTART)GetProcAddress(hDLL, DEF_HOOKSTART);
	HookStop = (PFN_HOOKSTOP)GetProcAddress(hDLL, DEF_HOOKSTOP);

	FILE *fp;
	fp = fopen("OutputThreadID.txt", "rt+");
	int arg[6];
	for (int i=0; i<6;i++)
	{
		fscanf(fp, "%d\n", &arg[i]);
		HookStart(arg[i]);//dwThreadId
	}

	printf("press 'q' to quit!\n");

	if (getchar() == 'q')
	{
		HookStop();
	}
	FreeLibrary(hDLL);
}
