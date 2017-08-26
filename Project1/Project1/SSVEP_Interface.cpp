//////////////////////////////
// 时间：2014.10.30
// 功能描述：
//	   分层架构的SSVEP界面程序，主要功能有：
//			1.通过串口接收EEG分类结果；
//			2.有名为SSVEP_Stimuli的类，可以生成多个激励源对象，并对其图片内容和闪烁频率进行方便设置
//			3.子函数Direct3D.cpp中封装了利用DirectX进行激励闪烁和绘图的基本功能函数

/////////////////////////////
#include "SSVEP_Interface.h"
int Window_Closed = 0; //标示是否需要退出循环


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Create the window
	HWND window = D3D_CreateWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	if (!window) return 0;

	//initialize the interface (including the SSVEP stimuli and the serial port)
	if (!D3D_init(window)) return 0;

	MSG message;
	//main loop
	while (!Window_Closed)
	{
		//没有该语句的时候界面无法响应鼠标的拖动 
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		//闪烁程序
		D3D_Display(window);

		//响应串口控制指令
		D3D_Serial();

		//响应ESC按键
		D3D_KeyResponse(window);
	}
	return message.wParam;
}
