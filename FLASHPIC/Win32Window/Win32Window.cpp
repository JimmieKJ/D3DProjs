// Win32Window.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32Window.h"
#include "FlashPicture.h"
#include "Win32WindowDll.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


HBITMAP bmp;

LRESULT CALLBACK  BitMapWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE://按下Esc键
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		}
		SetCursor(LoadCursor(NULL, IDC_HELP));//设置新的鼠标光标
		ShowCursor(TRUE);//显示鼠标光标
		break;

	case WM_LBUTTONDOWN://MA_NOACTIVATE 
		SetCursor(LoadCursor(NULL, IDC_UPARROW));//设置新的鼠标光标
		ShowCursor(TRUE);//显示鼠标光标
		return DefWindowProc(hWnd, message, MA_NOACTIVATE, lParam);//已经在钩子里拦截了，这里不会来的
		break;
	case WM_RBUTTONDOWN://MA_NOACTIVATE 
		SetCursor(LoadCursor(NULL, IDC_UPARROW));//设置新的鼠标光标
		ShowCursor(TRUE);//显示鼠标光标
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		bmp = (HBITMAP)::LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP1),	IMAGE_BITMAP, 0, 0, 0);
		if (bmp != NULL)
		{
			HDC mem = ::CreateCompatibleDC(hdc);
			::SelectObject(mem, bmp);
			::BitBlt(hdc, 0, 0, 48, 48, mem, 0, 0, SRCCOPY);
			::DeleteDC(mem);
			::DeleteObject(bmp);
		}
		else
		{
			wchar_t err[20];
			wsprintfW(err, L"%d\r\n", GetLastError());
			OutputDebugString(err);
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32WINDOW, szWindowClass, MAX_LOADSTRING);
	//szWindowClass = L"ssssssssb";
	//szTitle = L"Title_SSSSSSSSSSSBB";
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32WINDOW));


	// Register class///////////////////////////////////////////////////////////////////////////////////////////////////////test  add//////////////////////////////////////////
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	WNDPROC  winProc = (WNDPROC)(BitMapWndProc);
	wcex.lpfnWndProc = winProc;			//WNDPROC 
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDM_ABOUT);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);//BackColor
	wcex.lpszMenuName = NULL;

	TCHAR szbuffClasssName[251];
	wsprintf(szbuffClasssName, L"LiuWindowClass%d", 33);
	wcex.lpszClassName = szbuffClasssName;

	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDM_ABOUT);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	HINSTANCE 	g_hInst = hInstance;
	RECT rc = { 0, 0, 160, 320 };//480
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);//L"Tutorial07"
// 	HWND	g_hWnd = CreateWindow(szbuffClasssName, szbuffClasssName, WS_OVERLAPPEDWINDOW,				//创建一个带bmp图片的窗口
// 		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,	NULL);
// 	if (!g_hWnd)
// 		return E_FAIL;
// 
// 	// WM_MOUSEACTIVATE
// 	SetWindowLong(g_hWnd, GWL_STYLE, WS_EX_TRANSPARENT );//WS_EX_NOACTIVATE 会导致不能点击关闭来关闭这个窗口// WS_BORDER是无边框| WS_EX_NOACTIVATE //WS_BORDER
// 	SetWindowLong(g_hWnd, GWL_EXSTYLE, GetWindowLongPtr(g_hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED);//这个WS_EX_LAYERED导致透明成功
// 	//GCL_HBRBACKGROUND
// 	ShowWindow(g_hWnd, nCmdShow);
// 	SetWindowPos(g_hWnd, HWND_TOPMOST, 222, 333, 48, 48, SWP_NOACTIVATE);//置顶  不激活窗口
// 	SetLayeredWindowAttributes(g_hWnd, 0, 193, LWA_ALPHA);//0.33 的ALPHA
// 	SetWindowTextA(g_hWnd, "48 he 48 de");
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    MSG msg;


    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
			Sleep(10);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32WINDOW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
   // wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName   = NULL;
	wcex.lpszClassName = L"ssssssssssbb";// szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(L"ssssssssssbb", L"ssssssssssbb", WS_OVERLAPPEDWINDOW,
      0, 700, 400, 400, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   //SetWindowPos()
   UpdateWindow(hWnd);

   return TRUE;
}

//绘制 折现的方法
void  drawLine(HDC hdc, POINT apt[],int benginx,int beginy,int endx,int endy)
{
	MoveToEx(hdc, benginx, beginy, NULL);
	LineTo(hdc, endx, endy);
}

void DrawBezier(HDC hdc, POINT apt[])
{
//	PolyBezier(hdc, apt, 4);

	MoveToEx(hdc, apt[0].x, apt[0].y, NULL);
	LineTo(hdc, apt[1].x, apt[1].y);

	MoveToEx(hdc, apt[2].x, apt[2].y, NULL);
	LineTo(hdc, apt[3].x, apt[3].y);

	MoveToEx(hdc, 200, 200, NULL);
	LineTo(hdc, 400,400);
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LPCTSTR g_lpchText = TEXT("Hello, Windows 98");
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int nSize = 1;
	static POINT    apt[4];
	int             cxClient, cyClient;
	HDC             hdc;
	PAINTSTRUCT     ps;

    switch (message)
    {
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		apt[0].x = cxClient / 4;
		apt[0].y = cyClient / 2;

		apt[1].x = cxClient / 2;
		apt[1].y = cyClient / 4;

		apt[2].x = cxClient / 2;
		apt[2].y = 3 * cyClient / 4;

		apt[3].x = 3 * cxClient / 4;
		apt[3].y = cyClient / 2;

		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
			case VK_ESCAPE:
			{
				HWND    hDesk;
				RECT    rc;
				hDesk = GetDesktopWindow();
				GetWindowRect(hDesk, &rc);
				SetWindowLong(hWnd, GWL_STYLE, WS_BORDER);
				SetWindowPos(hWnd, HWND_TOP, 0, 0, rc.right, rc.bottom,	SWP_SHOWWINDOW);//HWND_TOP//HWND_TOPMOST
			}
			break;
			case VK_SPACE://空格键按下时
			{
				HWND    hDesk;
				RECT    rc;
				hDesk = GetDesktopWindow();// 				GetWindowRect(hDesk, &rc);
// 		HWND hCreateWND=		CreateWindowA("IDD_DIALOG1", "CHIld 子窗口ChildProc ", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_OVERLAPPEDWINDOW, 222, 333, 100, 114, hWnd, (HMENU)(124), hInst, NULL);
// 		ShowWindow(hCreateWND, 123);
// 		HWND hClild = CreateWindowA("BUTTON", "BUTTON创建子窗口", WS_CHILD | WS_VISIBLE, 730, 450, 150, 28, hWnd, (HMENU)123, hInst, NULL);
			runFlashPicture((size_t)hWnd);//调用 闪烁图片dll的
				//runFlashPictureERP( (size_t)hWnd );//调用 闪烁图片dll的
			}
			break;
			case VK_UP:
			{
				//addPictureAndFrameNum(5,"1234.png", 60);//测试第二个接口ExportMainStartRun
				//rangsound();//测试代码
				for (int n = 1; n < 20; n++)
				{
					ExportSetBreaklineData(0, nSize * (n % 10));
				}
				nSize++;
			}
			break;
			case VK_DOWN:
			{
				ExportMainStartRun();
				for (int  n=1; n<501; n++)
				{
					ExportSetBreaklineData(0, 1*(n%10));
				}
// 				ExportSetBreaklineData(100, 100);
// 				ExportSetBreaklineData(200, 200);

			}
			break;

		}
	case WM_LBUTTONDOWN:

// 		DestroyWindow(hWnd);
// 		PostQuitMessage(0);
		//SetCursor(LoadCursor(NULL, IDC_UPARROW));//设置新的鼠标光标
		//ShowCursor(TRUE);//显示鼠标光标
		RECT  Rect;
		GetWindowRect(hWnd, &Rect);
		//SetWindowPos(hWnd, HWND_TOP, Rect.left, Rect.top, Rect.right- Rect.left+100, Rect.bottom - Rect.top + 100, SWP_NOACTIVATE);//置顶  不激活窗口
		g_lpchText = TEXT("WM_LBUTTONDOWN");
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;         // TODO: 在此处添加使用 hdc 的任何绘图代码...
            HDC hdc = BeginPaint(hWnd, &ps);
   
			RECT  Rect;

			GetClientRect(hWnd, &Rect);
			HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
			SelectObject(hdc, hRedBrush);
			DrawText(hdc, TEXT("Hello---Windows 98"), -1, &Rect,	DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			InvalidateRect(hWnd, NULL, TRUE);
			static int  number = 0;
			number++;
			//if (number%1000 ==0)
			{
				DrawBezier(hdc, apt);
			}
			



            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}



// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
