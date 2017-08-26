// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <string>
#include <map>
#include<stdio.h>  
#include<Windows.h>  
#include <Shlobj.h>
//#include <boost/tokenizer.hpp> 
#include <process.h>
#include <TlHelp32.h>
#include <tchar.h>

#define DEF_PROCESS_NAME "notepad++.exe"  

HINSTANCE g_hInstance = NULL;
HHOOK g_hHook = NULL;
HWND g_hWnd = NULL;

using namespace std;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = hModule;
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

 const char * GetSymbolicMsg(UINT message, map<UINT, string> &mapSymbolicMsgs) // ��Ϣת��Ϊ�ַ���
{
	map<UINT, string>::const_iterator cit = mapSymbolicMsgs.begin();

	for (; cit != mapSymbolicMsgs.end(); cit++)
	{
		if (cit->first == message)
		{
			return (cit->second).c_str();
		}
	}

	return ("User_Define");
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)//wParam���ĸ�����  ,  lParam��Ь�����ַ�����
{
	char szPath[MAX_PATH] = { 0, };
	char* p = NULL;
	char strbuff[33];
	sprintf_s(strbuff, "%d", nCode);
	PCWPSTRUCT pMsg = (PCWPSTRUCT)lParam;       // WH_CALLWNDPROC
// 	static map<UINT, string> mapSymbolicMsgs;   // ���ܹ���DLL��ȫ�ֱ���///////////////////////////////////////////////////////////////////////////////////////////////////////
// 	if (nCode == HC_ACTION)
// 	{
// 		PCWPSTRUCT pMsg = (PCWPSTRUCT)lParam;       // WH_CALLWNDPROC
// 		char buffer[200] = { 0 };
// 		if (mapSymbolicMsgs.size() == 0)
// 		{
// 			//InitSymbolicMsg(mapSymbolicMsgs);
// 		}
// 		sprintf_s(buffer, "%08X S Msg: %s(%04X), wParam: %08X, lParam: %08X\n", pMsg->hwnd, GetSymbolicMsg(pMsg->message, mapSymbolicMsgs), pMsg->message, (int)pMsg->wParam, (int)pMsg->lParam);
// 	//	AppendDebug(NULL, buffer, strlen(buffer));
// 	}/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string strShowMessage(strbuff);//΢��� Unicode�Ͷ��ֽںܷ���.......................................................................................................................
	KBDLLHOOKSTRUCT *Key_Info = (KBDLLHOOKSTRUCT*)lParam;
	
// 	if (HC_ACTION == nCode)
// 	{
// 
// 		if (WM_KEYDOWN == pMsg->message)  //�������Ϊ����״̬   pMsg->message��ɵ��
// 		{
// 			MessageBoxA(NULL, LPCSTR("WM_KEYDOWN"), LPCSTR("find WM_KEYDOWN"), MB_OK);
// 			if (Key_Info->vkCode == VK_SPACE ) // VK_SPACE��
// 			{
// 				MessageBoxA(NULL, LPCSTR("i  VK_SPACE it"), LPCSTR("find VK_SPACE"), MB_OK);
// 				//return TRUE;
// 			}
// 			if (Key_Info->vkCode == 0x4D && ((GetKeyState(VK_LWIN) & 0x8000) ||
// 				(GetKeyState(VK_RWIN) & 0x8000))) //���� WIN+D ��ϼ�(����)
// 			{
// 				//return TRUE;
// 			}
// 		}
// 
// 	}
//	return CallNextHookEx(h_HotKey, nCode, wParam, lParam); //�ص���....................................................................................................................................................

	if (nCode == HC_ACTION)
	{
		switch (wParam)
		{
	    	case VK_SPACE://����VK_SPACE��
				//	if (WM_KEYDOWN == wParam)//Ц��һ��
					{
						HWND HwndFind = FindWindow(NULL, _T("BP_FirstPerson (64-bit, PCD3D_SM5)") );
						if (HwndFind != NULL)
						{
							if (!(lParam & 0x40000000))//0100 0000 0000 0000 0000 0000 0000 0000
							{
							//	MessageBoxA(NULL, LPCSTR("key down"), LPCSTR("key down"), MB_OK);
								SendMessage(HwndFind, WM_KEYDOWN, wParam, lParam);//WM_KEYDOWN
							}
							else
							{
								SendMessage(HwndFind, WM_KEYUP, wParam, lParam);//WM_KEYUP
							}
				
						char strggbuff[33];
						sprintf_s(strggbuff, "%d", pMsg->message);
						//MessageBoxA(NULL, LPCSTR(strggbuff), LPCSTR("message"), MB_OK);
						}
						else
						{
									//	MessageBoxA(NULL, LPCSTR(strShowMessage.c_str()), LPCSTR("VK_SPACE"), MB_OK);//MessageBoxA΢��� Unicode�Ͷ��ֽںܷ���//�������Ĭ��define Unicode����
									//SendMessage(glhDisplayWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szCaption);
						}
					}
				break;
		}


		if (!(lParam & 0x80000000))
		{
			GetModuleFileNameA(NULL, szPath, MAX_PATH);
			p = strrchr(szPath, '\\');
		}
		if (!_stricmp(p + 1, DEF_PROCESS_NAME))
			return 1;
	}
	return 1;
	//return CallNextHookEx(g_hHook, nCode, wParam, lParam);//����������ԭ�������ص� ������
}
HMODULE WINAPI ModuleFromAddress(PVOID pv)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
	{
		return (HMODULE)mbi.AllocationBase;
	}
	else
	{
		return NULL;
	}
}


#ifdef __cplusplus  
extern "C"
{
#endif  
	__declspec(dllexport) void HookStart(int threadId)
	{
		g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, ModuleFromAddress(KeyboardProc), threadId);//g_hInstance
		//g_hHook = SetWindowsHookEx(WH_MOUSE, KeyboardProc, ModuleFromAddress(KeyboardProc), threadId);//g_hInstance
	}

	__declspec(dllexport) void HookStop()
	{
		if (g_hHook)
		{
			UnhookWindowsHookEx(g_hHook);
			g_hHook = NULL;
		}
	}
#ifdef __cplusplus  
}
#endif  
