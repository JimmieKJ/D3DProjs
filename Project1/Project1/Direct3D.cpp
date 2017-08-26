#include "Direct3D.h"
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
struct CUSTOMVERTEX { FLOAT X, Y, Z, RHW; DWORD COLOR; };

/***********************/
// 定义全局变量 注意：不能在头文件中定义全局变量
/**********************/
//Direct3D objects.
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPDIRECT3DSURFACE9 surface_On[STIMULI_NUM], surface_Off[STIMULI_NUM], surface_Arrow, surface_Red;
// Stimuli对象
SSVEP_Stimuli SSVEP[STIMULI_NUM];

//初始化串口
jeff_class::CMySerialPort sCom1;
// 多级菜单的menu状态
char MenuLabel[6] = "";
//用于显示的字体：A pointer to an ID3DXFont interface, representing the created font object.
ID3DXFont* g_font;
//键盘的按下状态，为了处理按下某按键时连续响应的情况
int KeyState = 0;
//空格状态，为1时闪烁图片，为0时不闪烁
BOOL b_Space = true;


/***********************/
// 处理window的消息，也就是关闭窗口时产生的动作
/**********************/
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	extern int Window_Closed;
	switch (msg)
	{
	case WM_DESTROY:
		//gameover = true;  
		PostQuitMessage(0);
		D3D_Release();
		Window_Closed = 1;
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
/***********************/
// 创建DirectX窗口
/**********************/
HWND D3D_CreateWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//initialize window settings  
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = APPTITLE.c_str();
	wc.hIconSm = NULL;
	RegisterClassEx(&wc);

	//create a new window  
	HWND window = CreateWindow(APPTITLE.c_str(), APPTITLE.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		INTERFACE_WIDTH, INTERFACE_HEIGHT, NULL, NULL, hInstance, NULL);

	//was there an error creating the window?  
	if (window == 0) return 0;

	//display the window  
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);
	return window;
}
/***********************/
// 初始化SSVEP激励
/**********************/
void Stimuli_Init()
{
	//四个图片的坐标位置，界面左上角为坐标原点！！！！！！！！！！！！
	RECT rect[STIMULI_NUM];
	for (int i = 0; i<STIMULI_NUM; i++)
	{
		rect[i].left = TARGET_X[i];
		rect[i].right = TARGET_X[i] + TARGET_WIDTH;
		rect[i].top = TARGET_Y[i];
		rect[i].bottom = TARGET_Y[i] + TARGET_HEIGHT;
		SSVEP[i].StimuliInit(d3ddev, STIMULI_FRAME[i], rect[i], IMAGE_ADDRESS[0][i], "../Res/OffState.bmp", &surface_On[i], &surface_Off[i]);
	}

	//加载黄色箭头的位图 
	HRESULT result = d3ddev->CreateOffscreenPlainSurface(
		300,                //width of the surface -- 太低会使图片显示不清楚
		300,                //height of the surface  
		D3DFMT_X8R8G8B8,    //surface format  
		D3DPOOL_DEFAULT,    //memory pool to use  
		&surface_Arrow,           //pointer to the surface  
		NULL);
	result = D3DXLoadSurfaceFromFile(
		surface_Arrow,            //destination surface  
		NULL,               //destination palette  
		NULL,               //destination rectangle  
		"../Res/Arrow.bmp",     //source filename  
		NULL,               //source rectangle  
		D3DX_DEFAULT,       //controls how image is filtered  
		0,                  //for transparency (0 for none)  
		NULL);              //source image info (usually NULL)

							//加载红色边框的位图 
	result = d3ddev->CreateOffscreenPlainSurface(
		300,                //width of the surface -- 太低会使图片显示不清楚
		300,                //height of the surface  
		D3DFMT_X8R8G8B8,    //surface format  
		D3DPOOL_DEFAULT,    //memory pool to use  
		&surface_Red,           //pointer to the surface  
		NULL);
	result = D3DXLoadSurfaceFromFile(
		surface_Red,            //destination surface  
		NULL,               //destination palette  
		NULL,               //destination rectangle  
		"../Res/Red.bmp",     //source filename  
		NULL,               //source rectangle  
		D3DX_DEFAULT,       //controls how image is filtered  
		0,                  //for transparency (0 for none)  
		NULL);              //source image info (usually NULL)
}

/***********************/
// 初始化DirectX窗口对象
/**********************/
int D3D_init(HWND window)
{
	//初始化Direct3D接口对象
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d)
	{
		MessageBox(window, "Error initializing Direct3D", "Error", MB_OK);
		return 0;
	}

	//set Direct3D presentation parameters  
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = INTERFACE_WIDTH;
	d3dpp.BackBufferHeight = INTERFACE_HEIGHT;
	d3dpp.hDeviceWindow = window;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	//create Direct3D device  
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if (!d3ddev)
	{
		MessageBox(window, "Error creating Direct3D device", "Error", MB_OK);
		return 0;
	}

	//建立字体格式
	D3DXCreateFont(d3ddev, 20, 10, 10, 0, FALSE, DEFAULT_CHARSET, 0, 0, 0, "Arial", &g_font);

	//clear the backbuffer to black  
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	//Initialize the SSVEP stimulus
	Stimuli_Init();
	if (d3ddev->BeginScene())
	{
		for (int i = 0; i<STIMULI_NUM; i++)
		{
			SSVEP[i].Flash(d3ddev, surface_On[i], surface_Off[i], surface_Arrow, surface_Red, backbuffer);
		}
		d3ddev->EndScene();
	}
	d3ddev->Present(NULL, NULL, NULL, NULL);

	//Initialize the Serial Port
	sCom1.Open(SERIAL_PORT, 115200);

	return 1;
}
int time_count;
/***********************/
// 在Direct3D窗口上进行显示
/**********************/
void D3D_Display(HWND hwnd)
{
	//make sure the Direct3D device is valid  
	if (!d3ddev) return;

	//create pointer to the back buffer  
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	//start rendering  
	if (d3ddev->BeginScene())
	{
		for (int i = 0; i<STIMULI_NUM; i++)
		{
			SSVEP[i].Flash(d3ddev, surface_On[i], surface_Off[i], surface_Arrow, surface_Red, backbuffer);
			SSVEP[i].DecCount();
		}

		//在最后一幅图的上方显示menu标志(当有5幅图时为倒数第二幅)
		RECT g_client_rect;
		g_client_rect.left = TARGET_X[STIMULI_NUM - 2];
		g_client_rect.right = TARGET_X[STIMULI_NUM - 2] + TARGET_WIDTH;
		g_client_rect.bottom = TARGET_Y[STIMULI_NUM - 2] - 30;
		g_client_rect.top = TARGET_Y[STIMULI_NUM - 2] - TARGET_HEIGHT;

		DWORD format = DT_SINGLELINE | DT_NOCLIP | DT_CENTER | DT_BOTTOM;
		g_font->DrawText(NULL, MenuLabel, (INT)strlen(MenuLabel), &g_client_rect, format, 0xFFFFFF00);

		d3ddev->EndScene();

		////*************训练过程使用******************
		//if(time_count == 3*60) //显示箭头1
		//	SSVEP[0].EnableArrow(KEY_ARROW); //显示黄色箭头
		//else if (time_count == 4*60)//闪烁图片
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
		//else if (time_count == 9*60)//停止闪烁
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], IMAGE_ADDRESS[0][k], &surface_On[k], &surface_Off[k]);

		//else if (time_count == 12*60)
		//	SSVEP[2].EnableArrow(KEY_ARROW); //显示黄色箭头
		//else if (time_count == 13*60)//闪烁图片
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
		//else if (time_count == 18*60)//停止闪烁
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], IMAGE_ADDRESS[0][k], &surface_On[k], &surface_Off[k]);

		//else if (time_count == 21*60)
		//	SSVEP[1].EnableArrow(KEY_ARROW); //显示黄色箭头
		//else if (time_count == 22*60)//闪烁图片
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
		//else if (time_count == 27*60)//停止闪烁
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], IMAGE_ADDRESS[0][k], &surface_On[k], &surface_Off[k]);

		//else if (time_count == 30*60)
		//	SSVEP[3].EnableArrow(KEY_ARROW); //显示黄色箭头
		//else if (time_count == 31*60)//闪烁图片
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
		//else if (time_count == 36*60)//停止闪烁
		//{
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], IMAGE_ADDRESS[0][k], &surface_On[k], &surface_Off[k]);
		//	time_count = 0;
		//}

		//time_count++;
	}
	d3ddev->Present(NULL, NULL, NULL, NULL);
}
/***********************/
// 串口处理函数
/**********************/
void D3D_Serial()
{
	int i = sCom1.ReadDataWaiting();
	char TempLabel[6];
	if (i) //串口接收到数据
	{
		sCom1.ReadData(TempLabel, i);//读取串口数据，长度为i
		TempLabel[i] = '\0';
		switch (TempLabel[i - 1])
		{
		case '0': //主菜单0
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
			strcpy_s(MenuLabel, "");//因为常常出现只收到最后一个数字的情况
								  //strcpy(MenuLabel,"menu0");//因为常常出现只收到最后一个数字的情况
			break;
		case '1': //子菜单1
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[1][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
			strcpy_s(MenuLabel, "menu1");//因为常常出现只收到最后一个数字的情况
			break;
		case '2': //子菜单2
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[2][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
			strcpy_s(MenuLabel, "menu2");//因为常常出现只收到最后一个数字的情况
			break;
		case '3': //子菜单3
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[3][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
			strcpy_s(MenuLabel, "menu3");//因为常常出现只收到最后一个数字的情况
			break;
		case '4': //子菜单3
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[4][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
			strcpy_s(MenuLabel, "menu4");//因为常常出现只收到最后一个数字的情况
			break;
		case 'F':
			SSVEP[0].EnableRed(SERIAL_ARROW); //在第一幅图片上方显示黄色箭头
			SSVEP[3].LoadImage(d3ddev, IMAGE_ADDRESS[0][3], "../Res/OffState.bmp", &surface_On[3], &surface_Off[3]);//只在实验2中使用**************************
			break;
		case 'L':
			SSVEP[1].EnableRed(SERIAL_ARROW);
			SSVEP[3].LoadImage(d3ddev, IMAGE_ADDRESS[0][3], "../Res/OffState.bmp", &surface_On[3], &surface_Off[3]);
			break;
		case 'R':
			SSVEP[2].EnableRed(SERIAL_ARROW);
			SSVEP[3].LoadImage(d3ddev, IMAGE_ADDRESS[0][3], "../Res/OffState.bmp", &surface_On[3], &surface_Off[3]);
			break;
		case 'B':
			SSVEP[3].EnableRed(SERIAL_ARROW);
			//SSVEP[3].LoadImage(d3ddev, "../Res/25.bmp", "../Res/OffState.bmp", &surface_On[3], &surface_Off[3]);//在停下时切换到ExcuseMe图片*****实验2切换excuse me图片
			SSVEP[3].LoadImage(d3ddev, IMAGE_ADDRESS[0][3], "../Res/OffState.bmp", &surface_On[3], &surface_Off[3]);
			break;
		}
	}
}
/***********************/
// 按键响应函数
/**********************/
void D3D_KeyResponse(HWND window)
{
	int key_down = 0;
	if (KEY_DOWN(VK_ESCAPE))
	{
		PostMessage(window, WM_DESTROY, 0, 0); //关闭窗口的消息处理函数
	}
	else if (KEY_DOWN(VK_SPACE)) //按下空格，启动或停止闪烁，通过更换On和Off图片的内容来实现
	{
		if (b_Space == true) //不闪烁图片
		{
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], IMAGE_ADDRESS[0][k], &surface_On[k], &surface_Off[k]);
		}
		else
		{
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
		}
		b_Space = !b_Space;
	}
	else
	{
		for (int i = 0; i<STIMULI_NUM; i++)
		{
			if (KEY_DOWN(i + 49)) //数字按键1是49，必须是字母键上方的数字按键！！
			{
				if (KeyState == 0) //当之前按键已经按下时不做处理
				{
					sCom1.SendData(TARGET_LABEL[i], strlen(TARGET_LABEL[i]));
					KeyState = 1;//在下次循环中指示前一次循环是否有键按下
					SSVEP[i].EnableArrow(KEY_ARROW); //显示黄色箭头
				}
				key_down = 1;//表示该循环有键按下
			}
		}
		if (key_down == 0)
			KeyState = 0; //表示此次循环中没有上述按键按下
	}
}
/***********************/
// 释放对象资源
/**********************/
void D3D_Release()
{
	for (int i = 0; i<STIMULI_NUM; i++)
	{
		if (surface_On[i]) surface_On[i]->Release();
		if (surface_Off[i]) surface_Off[i]->Release();
	}
	if (surface_Arrow) surface_Arrow->Release();
	if (d3ddev) d3ddev->Release();
	if (d3d) d3d->Release();
	sCom1.Close();
}