#include "Direct3D.h"
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
struct CUSTOMVERTEX { FLOAT X, Y, Z, RHW; DWORD COLOR; };

/***********************/
// ����ȫ�ֱ��� ע�⣺������ͷ�ļ��ж���ȫ�ֱ���
/**********************/
//Direct3D objects.
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPDIRECT3DSURFACE9 surface_On[STIMULI_NUM], surface_Off[STIMULI_NUM], surface_Arrow, surface_Red;
// Stimuli����
SSVEP_Stimuli SSVEP[STIMULI_NUM];

//��ʼ������
jeff_class::CMySerialPort sCom1;
// �༶�˵���menu״̬
char MenuLabel[6] = "";
//������ʾ�����壺A pointer to an ID3DXFont interface, representing the created font object.
ID3DXFont* g_font;
//���̵İ���״̬��Ϊ�˴�����ĳ����ʱ������Ӧ�����
int KeyState = 0;
//�ո�״̬��Ϊ1ʱ��˸ͼƬ��Ϊ0ʱ����˸
BOOL b_Space = true;


/***********************/
// ����window����Ϣ��Ҳ���ǹرմ���ʱ�����Ķ���
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
// ����DirectX����
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
// ��ʼ��SSVEP����
/**********************/
void Stimuli_Init()
{
	//�ĸ�ͼƬ������λ�ã��������Ͻ�Ϊ����ԭ�㣡����������������������
	RECT rect[STIMULI_NUM];
	for (int i = 0; i<STIMULI_NUM; i++)
	{
		rect[i].left = TARGET_X[i];
		rect[i].right = TARGET_X[i] + TARGET_WIDTH;
		rect[i].top = TARGET_Y[i];
		rect[i].bottom = TARGET_Y[i] + TARGET_HEIGHT;
		SSVEP[i].StimuliInit(d3ddev, STIMULI_FRAME[i], rect[i], IMAGE_ADDRESS[0][i], "../Res/OffState.bmp", &surface_On[i], &surface_Off[i]);
	}

	//���ػ�ɫ��ͷ��λͼ 
	HRESULT result = d3ddev->CreateOffscreenPlainSurface(
		300,                //width of the surface -- ̫�ͻ�ʹͼƬ��ʾ�����
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

							//���غ�ɫ�߿��λͼ 
	result = d3ddev->CreateOffscreenPlainSurface(
		300,                //width of the surface -- ̫�ͻ�ʹͼƬ��ʾ�����
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
// ��ʼ��DirectX���ڶ���
/**********************/
int D3D_init(HWND window)
{
	//��ʼ��Direct3D�ӿڶ���
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

	//���������ʽ
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
// ��Direct3D�����Ͻ�����ʾ
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

		//�����һ��ͼ���Ϸ���ʾmenu��־(����5��ͼʱΪ�����ڶ���)
		RECT g_client_rect;
		g_client_rect.left = TARGET_X[STIMULI_NUM - 2];
		g_client_rect.right = TARGET_X[STIMULI_NUM - 2] + TARGET_WIDTH;
		g_client_rect.bottom = TARGET_Y[STIMULI_NUM - 2] - 30;
		g_client_rect.top = TARGET_Y[STIMULI_NUM - 2] - TARGET_HEIGHT;

		DWORD format = DT_SINGLELINE | DT_NOCLIP | DT_CENTER | DT_BOTTOM;
		g_font->DrawText(NULL, MenuLabel, (INT)strlen(MenuLabel), &g_client_rect, format, 0xFFFFFF00);

		d3ddev->EndScene();

		////*************ѵ������ʹ��******************
		//if(time_count == 3*60) //��ʾ��ͷ1
		//	SSVEP[0].EnableArrow(KEY_ARROW); //��ʾ��ɫ��ͷ
		//else if (time_count == 4*60)//��˸ͼƬ
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
		//else if (time_count == 9*60)//ֹͣ��˸
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], IMAGE_ADDRESS[0][k], &surface_On[k], &surface_Off[k]);

		//else if (time_count == 12*60)
		//	SSVEP[2].EnableArrow(KEY_ARROW); //��ʾ��ɫ��ͷ
		//else if (time_count == 13*60)//��˸ͼƬ
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
		//else if (time_count == 18*60)//ֹͣ��˸
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], IMAGE_ADDRESS[0][k], &surface_On[k], &surface_Off[k]);

		//else if (time_count == 21*60)
		//	SSVEP[1].EnableArrow(KEY_ARROW); //��ʾ��ɫ��ͷ
		//else if (time_count == 22*60)//��˸ͼƬ
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
		//else if (time_count == 27*60)//ֹͣ��˸
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], IMAGE_ADDRESS[0][k], &surface_On[k], &surface_Off[k]);

		//else if (time_count == 30*60)
		//	SSVEP[3].EnableArrow(KEY_ARROW); //��ʾ��ɫ��ͷ
		//else if (time_count == 31*60)//��˸ͼƬ
		//	for (int k=0;k<STIMULI_NUM;k++)
		//		SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
		//else if (time_count == 36*60)//ֹͣ��˸
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
// ���ڴ�����
/**********************/
void D3D_Serial()
{
	int i = sCom1.ReadDataWaiting();
	char TempLabel[6];
	if (i) //���ڽ��յ�����
	{
		sCom1.ReadData(TempLabel, i);//��ȡ�������ݣ�����Ϊi
		TempLabel[i] = '\0';
		switch (TempLabel[i - 1])
		{
		case '0': //���˵�0
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[0][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
			strcpy_s(MenuLabel, "");//��Ϊ��������ֻ�յ����һ�����ֵ����
								  //strcpy(MenuLabel,"menu0");//��Ϊ��������ֻ�յ����һ�����ֵ����
			break;
		case '1': //�Ӳ˵�1
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[1][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
			strcpy_s(MenuLabel, "menu1");//��Ϊ��������ֻ�յ����һ�����ֵ����
			break;
		case '2': //�Ӳ˵�2
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[2][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
			strcpy_s(MenuLabel, "menu2");//��Ϊ��������ֻ�յ����һ�����ֵ����
			break;
		case '3': //�Ӳ˵�3
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[3][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
			strcpy_s(MenuLabel, "menu3");//��Ϊ��������ֻ�յ����һ�����ֵ����
			break;
		case '4': //�Ӳ˵�3
			for (int k = 0; k<STIMULI_NUM; k++)
				SSVEP[k].LoadImage(d3ddev, IMAGE_ADDRESS[4][k], "../Res/OffState.bmp", &surface_On[k], &surface_Off[k]);
			strcpy_s(MenuLabel, "menu4");//��Ϊ��������ֻ�յ����һ�����ֵ����
			break;
		case 'F':
			SSVEP[0].EnableRed(SERIAL_ARROW); //�ڵ�һ��ͼƬ�Ϸ���ʾ��ɫ��ͷ
			SSVEP[3].LoadImage(d3ddev, IMAGE_ADDRESS[0][3], "../Res/OffState.bmp", &surface_On[3], &surface_Off[3]);//ֻ��ʵ��2��ʹ��**************************
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
			//SSVEP[3].LoadImage(d3ddev, "../Res/25.bmp", "../Res/OffState.bmp", &surface_On[3], &surface_Off[3]);//��ͣ��ʱ�л���ExcuseMeͼƬ*****ʵ��2�л�excuse meͼƬ
			SSVEP[3].LoadImage(d3ddev, IMAGE_ADDRESS[0][3], "../Res/OffState.bmp", &surface_On[3], &surface_Off[3]);
			break;
		}
	}
}
/***********************/
// ������Ӧ����
/**********************/
void D3D_KeyResponse(HWND window)
{
	int key_down = 0;
	if (KEY_DOWN(VK_ESCAPE))
	{
		PostMessage(window, WM_DESTROY, 0, 0); //�رմ��ڵ���Ϣ������
	}
	else if (KEY_DOWN(VK_SPACE)) //���¿ո�������ֹͣ��˸��ͨ������On��OffͼƬ��������ʵ��
	{
		if (b_Space == true) //����˸ͼƬ
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
			if (KEY_DOWN(i + 49)) //���ְ���1��49����������ĸ���Ϸ������ְ�������
			{
				if (KeyState == 0) //��֮ǰ�����Ѿ�����ʱ��������
				{
					sCom1.SendData(TARGET_LABEL[i], strlen(TARGET_LABEL[i]));
					KeyState = 1;//���´�ѭ����ָʾǰһ��ѭ���Ƿ��м�����
					SSVEP[i].EnableArrow(KEY_ARROW); //��ʾ��ɫ��ͷ
				}
				key_down = 1;//��ʾ��ѭ���м�����
			}
		}
		if (key_down == 0)
			KeyState = 0; //��ʾ�˴�ѭ����û��������������
	}
}
/***********************/
// �ͷŶ�����Դ
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