// dllmain.cpp : 定义 DLL 应用程序的入口点。
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

 const char * GetSymbolicMsg(UINT message, map<UINT, string> &mapSymbolicMsgs) // 消息转换为字符串
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

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)//wParam是哪个按键  ,  lParam是KEYDOWN UP
{
	char* p = NULL;
	char strbuff[33];
	sprintf_s(strbuff, "%d", nCode);
	PCWPSTRUCT pMsg = (PCWPSTRUCT)lParam;       // WH_CALLWNDPROC
	std::string strShowMessage(strbuff);//微软的 Unicode和多字节很烦人.......................................................................................................................
	KBDLLHOOKSTRUCT *Key_Info = (KBDLLHOOKSTRUCT*)lParam;
	
// 	if (HC_ACTION == nCode)
// 	{
// 
// 		if (WM_KEYDOWN == pMsg->message)  //如果按键为按下状态   pMsg->message是傻逼
// 		{
// 			MessageBoxA(NULL, LPCSTR("WM_KEYDOWN"), LPCSTR("find WM_KEYDOWN"), MB_OK);
// 			if (Key_Info->vkCode == VK_SPACE ) // VK_SPACE键
// 			{
// 				MessageBoxA(NULL, LPCSTR("i  VK_SPACE it"), LPCSTR("find VK_SPACE"), MB_OK);
// 				//return TRUE;
// 			}
// 			if (Key_Info->vkCode == 0x4D && ((GetKeyState(VK_LWIN) & 0x8000) ||
// 				(GetKeyState(VK_RWIN) & 0x8000))) //屏敝 WIN+D 组合键(左右)
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
	    	case VK_SPACE://按下VK_SPACE键
					{

					}
				break;
		}
		HWND HwndFind = FindWindow(NULL, _T("ShooterGame (64-bit, PCD3D_SM5)"));
		if (HwndFind != NULL)
		{
			if (!(lParam & 0x40000000))//0100 0000 0000 0000 0000 0000 0000 0000 //鼠标按下down的31位为0 ,up为1
			{
				//	MessageBoxA(NULL, LPCSTR("key down"), LPCSTR("key down"), MB_OK);				//MessageBoxA(NULL, LPCSTR(strggbuff), LPCSTR("message"), MB_OK);
				SendMessage(HwndFind, WM_KEYDOWN, wParam, lParam);//WM_KEYDOWN//00020846
			}
			else//鼠标按下down的31位为0 ,up为1
			{
				SendMessage(HwndFind, WM_KEYUP, wParam, lParam);//WM_KEYUP
			}
			char strggbuff[33];
			sprintf_s(strggbuff, "%d", pMsg->message);
		}
		else
		{
			//MessageBoxA微软的 Unicode和多字节很烦人//这个工程默认define Unicode编码				//SendMessage(glhDisplayWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szCaption);
		}
	}
	  return 1;
	 // return CallNextHookEx(g_hHook, nCode, wParam, lParam);//继续发生到原来被拦截的 窗口上,执行 被拦截窗口的winProc
}

LRESULT CALLBACK  MouseProc(int nCode, WPARAM wParam, LPARAM lParam)//wParam是哪个按键  ,  lParam是KEYDOWN UP
{
	char strbuff[93];	//sprintf_s(strbuff, "%d", nCode);
	PCWPSTRUCT pMsg = (PCWPSTRUCT)lParam;       // WH_CALLWNDPROC
	//std::string strShowMessage(strbuff);//微软的 Unicode和多字节很烦人.......................................................................................................................
	KBDLLHOOKSTRUCT *Key_Info = (KBDLLHOOKSTRUCT*)lParam;

	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;// s鼠标这边的
	if (pMouseStruct != NULL)
	{
		HWND HwndFind = FindWindow(NULL, _T("ShooterGame (64-bit, PCD3D_SM5)"));//BP_FirstPerson (64-bit, PCD3D_SM5)//Win32Window
		if (wParam == WM_LBUTTONDOWN)
		{
			if (HwndFind != NULL)//鼠标按下
			{	
			//	SendMessage(HwndFind, WM_NCLBUTTONDOWN, wParam, lParam);
				//SendMessage(HwndFind, WM_INPUT, wParam, lParam);
		//		SendMessage(HwndFind, WM_LBUTTONDOWN, wParam, lParam);//  //能进////完全不需要啊，直接设置焦点和前景窗口为 游戏窗口就行了
		//		SendMessage(HwndFind, WM_CAPTURECHANGED, wParam, lParam);//  //能进//

// 				SendMessage(HwndFind, WM_SETFOCUS, 0, 0);          //给按钮设置一下焦点
			//	PostMessage(HwndFind,WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pMouseStruct->pt.x, pMouseStruct->pt.y));
				KEYBDINPUT kDown;////////////////////////////////////////////////////////////////////////////////////////
				KEYBDINPUT kUp;
			//	MOUSEINPUT mouseInput;///////
		//		mouseInput.dwFlags = MOUSEEVENTF_LEFTDOWN;////////////
				INPUT inputKeyDown, inputKeyUP;
				inputKeyDown.type = inputKeyUP.type = INPUT_KEYBOARD; // 指定 input 为 keyboard  
				kDown.time = kUp.time = 0; // the system will provide its own time stamp.  
				kDown.dwExtraInfo = kUp.dwExtraInfo = (WORD)::GetMessageExtraInfo();
				inputKeyDown.ki = kDown;// 指定 键盘 属性结构  
				kDown.dwFlags = VK_ESCAPE;
				kDown.wVk = VK_ESCAPE;
				inputKeyUP.ki = kUp;    // 指定 键盘 属性结构  

				SetForegroundWindow(HwndFind);
				SetActiveWindow(HwndFind);
				SetFocus(HwndFind);
				SetCapture(HwndFind);
			}
		}
		else if (wParam == WM_LBUTTONUP)//鼠标 弹起 
		{
			//	SendMessage(HwndFind, WM_INPUT, wParam, lParam);	//	SendMessage((HWND)133190, WM_NCLBUTTONUP, wParam, lParam);//WM_KEYDOWN  //能进//WM_LBUTTONDOWN
		//	SendMessage(HwndFind, WM_LBUTTONUP, wParam, lParam);//WM_KEYDOWN  //百分百能进

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
				SendMessage(HwndFind, WM_RBUTTONDOWN, wParam, lParam);//WM_KEYUP   //能进,那么是send函数不行？？？？
																	  //SendMessage(HwndFind, WM_CLOSE, 0, 0);//关闭隐藏的CMD窗口 }//能进
			}

		}
	//	sprintf_s(strbuff, "MousePosition X =%d MousePosition Y=%d", pMouseStruct->pt.x, pMouseStruct->pt.y);	//	MessageBoxA(NULL, LPCSTR(strbuff), LPCSTR("strbuff"), MB_OK);
	}
	   return 1;//拦截
	//	return CallNextHookEx(g_hHookMouse, nCode, wParam, lParam);// s鼠标这边的
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
