//////////////////////////////
// ʱ�䣺2014.10.30
// ����������
//	   �ֲ�ܹ���SSVEP���������Ҫ�����У�
//			1.ͨ�����ڽ���EEG��������
//			2.����ΪSSVEP_Stimuli���࣬�������ɶ������Դ���󣬲�����ͼƬ���ݺ���˸Ƶ�ʽ��з�������
//			3.�Ӻ���Direct3D.cpp�з�װ������DirectX���м�����˸�ͻ�ͼ�Ļ������ܺ���

/////////////////////////////
#include "SSVEP_Interface.h"
int Window_Closed = 0; //��ʾ�Ƿ���Ҫ�˳�ѭ��


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
		//û�и�����ʱ������޷���Ӧ�����϶� 
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		//��˸����
		D3D_Display(window);

		//��Ӧ���ڿ���ָ��
		D3D_Serial();

		//��ӦESC����
		D3D_KeyResponse(window);
	}
	return message.wParam;
}
