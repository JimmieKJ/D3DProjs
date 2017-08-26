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
int nHook = 0;
HHOOK g_hHook[6] = { NULL ,NULL , NULL , NULL , NULL , NULL };

HHOOK g_hHookMouse = NULL;
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

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)//wParam���ĸ�����  ,  lParam��KEYDOWN UP
{
	char* p = NULL;
	char strbuff[33];
	sprintf_s(strbuff, "%d", nCode);
	PCWPSTRUCT pMsg = (PCWPSTRUCT)lParam;       // WH_CALLWNDPROC
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

	if (nCode == HC_ACTION)
	{
		switch (wParam)
		{
	    	case VK_SPACE://����VK_SPACE��
					{

					}
				break;
		}
		HWND HwndFind = FindWindow(NULL, _T("ShooterGame (64-bit, PCD3D_SM5)"));
		if (HwndFind != NULL)
		{
			if (!(lParam & 0x40000000))//0100 0000 0000 0000 0000 0000 0000 0000 //��갴��down��31λΪ0 ,upΪ1
			{
				//	MessageBoxA(NULL, LPCSTR("key down"), LPCSTR("key down"), MB_OK);				//MessageBoxA(NULL, LPCSTR(strggbuff), LPCSTR("message"), MB_OK);
				SendMessage(HwndFind, WM_KEYDOWN, wParam, lParam);//WM_KEYDOWN//00020846
			}
			else//��갴��down��31λΪ0 ,upΪ1
			{
				SendMessage(HwndFind, WM_KEYUP, wParam, lParam);//WM_KEYUP
			}
			char strggbuff[33];
			sprintf_s(strggbuff, "%d", pMsg->message);
		}
		else
		{
			//MessageBoxA΢��� Unicode�Ͷ��ֽںܷ���//�������Ĭ��define Unicode����				//SendMessage(glhDisplayWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szCaption);
		}
	}
	  return 1;
	 // return CallNextHookEx(g_hHook, nCode, wParam, lParam);//����������ԭ�������ص� ������,ִ�� �����ش��ڵ�winProc
}

LRESULT CALLBACK  MouseProc(int nCode, WPARAM wParam, LPARAM lParam)//wParam���ĸ�����  ,  lParam��KEYDOWN UP
{
	char strbuff[93];	//sprintf_s(strbuff, "%d", nCode);
	PCWPSTRUCT pMsg = (PCWPSTRUCT)lParam;       // WH_CALLWNDPROC
	//std::string strShowMessage(strbuff);//΢��� Unicode�Ͷ��ֽںܷ���.......................................................................................................................
	KBDLLHOOKSTRUCT *Key_Info = (KBDLLHOOKSTRUCT*)lParam;

	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;// s�����ߵ�
	if (pMouseStruct != NULL)
	{
		HWND HwndFind = FindWindow(NULL, _T("ShooterGame (64-bit, PCD3D_SM5)"));//BP_FirstPerson (64-bit, PCD3D_SM5)//Win32Window
		if (wParam == WM_LBUTTONDOWN)
		{
			if (HwndFind != NULL)//��갴��
			{	
			//	SendMessage(HwndFind, WM_NCLBUTTONDOWN, wParam, lParam);
				//SendMessage(HwndFind, WM_INPUT, wParam, lParam);
		//		SendMessage(HwndFind, WM_LBUTTONDOWN, wParam, lParam);//  //�ܽ�////��ȫ����Ҫ����ֱ�����ý����ǰ������Ϊ ��Ϸ���ھ�����
		//		SendMessage(HwndFind, WM_CAPTURECHANGED, wParam, lParam);//  //�ܽ�//

// 				SendMessage(HwndFind, WM_SETFOCUS, 0, 0);          //����ť����һ�½���
			//	PostMessage(HwndFind,WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pMouseStruct->pt.x, pMouseStruct->pt.y));
				KEYBDINPUT kDown;////////////////////////////////////////////////////////////////////////////////////////
				KEYBDINPUT kUp;
			//	MOUSEINPUT mouseInput;///////
		//		mouseInput.dwFlags = MOUSEEVENTF_LEFTDOWN;////////////
				INPUT inputKeyDown, inputKeyUP;
				inputKeyDown.type = inputKeyUP.type = INPUT_KEYBOARD; // ָ�� input Ϊ keyboard  
				kDown.time = kUp.time = 0; // the system will provide its own time stamp.  
				kDown.dwExtraInfo = kUp.dwExtraInfo = (WORD)::GetMessageExtraInfo();
				inputKeyDown.ki = kDown;// ָ�� ���� ���Խṹ  
				kDown.dwFlags = VK_ESCAPE;
				kDown.wVk = VK_ESCAPE;
				inputKeyUP.ki = kUp;    // ָ�� ���� ���Խṹ  

				SetForegroundWindow(HwndFind);
				SetActiveWindow(HwndFind);
				SetFocus(HwndFind);
				SetCapture(HwndFind);
			}
		}
		else if (wParam == WM_LBUTTONUP)//��� ���� 
		{
			//	SendMessage(HwndFind, WM_INPUT, wParam, lParam);	//	SendMessage((HWND)133190, WM_NCLBUTTONUP, wParam, lParam);//WM_KEYDOWN  //�ܽ�//WM_LBUTTONDOWN
		//	SendMessage(HwndFind, WM_LBUTTONUP, wParam, lParam);//WM_KEYDOWN  //�ٷְ��ܽ�

// 			SetForegroundWindow(HwndFind);
// 			SetActiveWindow(HwndFind);
// 
// 			SetFocus(HwndFind);
// 			SetActiveWindow(HwndFind);
// 			SetCapture(HwndFind);
		}
		else if (wParam == WM_RBUTTONDOWN)
		{
			if (HwndFind != NULL)
			{
				SendMessage(HwndFind, WM_RBUTTONDOWN, wParam, lParam);//WM_KEYUP   //�ܽ�,��ô��send�������У�������
																	  //SendMessage(HwndFind, WM_CLOSE, 0, 0);//�ر����ص�CMD���� }//�ܽ�
			}

		}
	//	sprintf_s(strbuff, "MousePosition X =%d MousePosition Y=%d", pMouseStruct->pt.x, pMouseStruct->pt.y);	//	MessageBoxA(NULL, LPCSTR(strbuff), LPCSTR("strbuff"), MB_OK);
	}
	   return 1;//����
	//	return CallNextHookEx(g_hHookMouse, nCode, wParam, lParam);// s�����ߵ�
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
		g_hHook[nHook] = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, ModuleFromAddress(KeyboardProc), threadId);//g_hInstance
		nHook++;
		g_hHookMouse = SetWindowsHookEx(WH_MOUSE, MouseProc, ModuleFromAddress(MouseProc), threadId);//g_hInstance
	}

	__declspec(dllexport) void HookStop()
	{
		if (g_hHook[1])
		{
			UnhookWindowsHookEx(g_hHook[1]);
			g_hHook[1] = NULL;
		}

		if (g_hHookMouse)
		{
			UnhookWindowsHookEx(g_hHookMouse);
			g_hHookMouse = NULL;
		}

	}
#ifdef __cplusplus  
}
#endif  
