// Win32Window.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Win32Window.h"
#include "FlashPicture.h"
#include "Win32WindowDll.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

// �˴���ģ���а����ĺ�����ǰ������: 
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
		case VK_ESCAPE://����Esc��
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		}
		SetCursor(LoadCursor(NULL, IDC_HELP));//�����µ������
		ShowCursor(TRUE);//��ʾ�����
		break;

	case WM_LBUTTONDOWN://MA_NOACTIVATE 
		SetCursor(LoadCursor(NULL, IDC_UPARROW));//�����µ������
		ShowCursor(TRUE);//��ʾ�����
		return DefWindowProc(hWnd, message, MA_NOACTIVATE, lParam);//�Ѿ��ڹ����������ˣ����ﲻ������
		break;
	case WM_RBUTTONDOWN://MA_NOACTIVATE 
		SetCursor(LoadCursor(NULL, IDC_UPARROW));//�����µ������
		ShowCursor(TRUE);//��ʾ�����
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

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32WINDOW, szWindowClass, MAX_LOADSTRING);
	//szWindowClass = L"ssssssssb";
	//szTitle = L"Title_SSSSSSSSSSSBB";
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
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
// 	HWND	g_hWnd = CreateWindow(szbuffClasssName, szbuffClasssName, WS_OVERLAPPEDWINDOW,				//����һ����bmpͼƬ�Ĵ���
// 		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,	NULL);
// 	if (!g_hWnd)
// 		return E_FAIL;
// 
// 	// WM_MOUSEACTIVATE
// 	SetWindowLong(g_hWnd, GWL_STYLE, WS_EX_TRANSPARENT );//WS_EX_NOACTIVATE �ᵼ�²��ܵ���ر����ر��������// WS_BORDER���ޱ߿�| WS_EX_NOACTIVATE //WS_BORDER
// 	SetWindowLong(g_hWnd, GWL_EXSTYLE, GetWindowLongPtr(g_hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED);//���WS_EX_LAYERED����͸���ɹ�
// 	//GCL_HBRBACKGROUND
// 	ShowWindow(g_hWnd, nCmdShow);
// 	SetWindowPos(g_hWnd, HWND_TOPMOST, 222, 333, 48, 48, SWP_NOACTIVATE);//�ö�  �������
// 	SetLayeredWindowAttributes(g_hWnd, 0, 193, LWA_ALPHA);//0.33 ��ALPHA
// 	SetWindowTextA(g_hWnd, "48 he 48 de");
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    MSG msg;


    // ����Ϣѭ��: 
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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


//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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

//���� ���ֵķ���
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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
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
			case VK_SPACE://�ո������ʱ
			{
				HWND    hDesk;
				RECT    rc;
				hDesk = GetDesktopWindow();// 				GetWindowRect(hDesk, &rc);
// 		HWND hCreateWND=		CreateWindowA("IDD_DIALOG1", "CHIld �Ӵ���ChildProc ", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_OVERLAPPEDWINDOW, 222, 333, 100, 114, hWnd, (HMENU)(124), hInst, NULL);
// 		ShowWindow(hCreateWND, 123);
// 		HWND hClild = CreateWindowA("BUTTON", "BUTTON�����Ӵ���", WS_CHILD | WS_VISIBLE, 730, 450, 150, 28, hWnd, (HMENU)123, hInst, NULL);
			runFlashPicture((size_t)hWnd);//���� ��˸ͼƬdll��
				//runFlashPictureERP( (size_t)hWnd );//���� ��˸ͼƬdll��
			}
			break;
			case VK_UP:
			{
				//addPictureAndFrameNum(5,"1234.png", 60);//���Եڶ����ӿ�ExportMainStartRun
				//rangsound();//���Դ���
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
		//SetCursor(LoadCursor(NULL, IDC_UPARROW));//�����µ������
		//ShowCursor(TRUE);//��ʾ�����
		RECT  Rect;
		GetWindowRect(hWnd, &Rect);
		//SetWindowPos(hWnd, HWND_TOP, Rect.left, Rect.top, Rect.right- Rect.left+100, Rect.bottom - Rect.top + 100, SWP_NOACTIVATE);//�ö�  �������
		g_lpchText = TEXT("WM_LBUTTONDOWN");
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
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
            PAINTSTRUCT ps;         // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
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



// �����ڡ������Ϣ�������
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
