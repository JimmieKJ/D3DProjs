#pragma once
#ifdef _WINDOWS_
#undef _WINDOWS_
#endif
#include <d3d9.h>  
#include <d3dx9.h> 
#include <string> 
using namespace std;
#include "SSVEP_Stimuli.h"
#include "MySerialPort.h" //����ͨ�ź���
//#include <windows.h>  

/***********************/
// Direct3D���ڵ���ز���
/**********************/
// ��ʾ����Ŀ�͸ߣ���������
const unsigned int INTERFACE_WIDTH = 1350;
const unsigned int INTERFACE_HEIGHT = 1000;
const string APPTITLE = "Multilayer SSVEP Interface";//���ڱ��⣬string����ʹ�õ�ʱ�����Ҫ��#include <string> �� using namespace std;����
													 /***********************/
													 // ������Ӧ
													 /**********************/
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0) 
													 /***********************/
													 // ��������������
													 /**********************/
													 // ͼƬ�����سߴ�
static float  TARGET_WIDTH = 150.0;
static float TARGET_HEIGHT = 150.0;
/***********************/
// ����Ŀ��Ĳ��������ı伤������ʱ��Ҫ��Ӧ����
/**********************/
const unsigned int STIMULI_NUM = 5;
const unsigned int STIMULI_FRAME[STIMULI_NUM] = { 13,4, 3 }; //�ò�������Ƶ�ʣ���������������ɵ�ÿ��������ռ��֡�������䲻�ܱ�2����ʱ����֡����֡��1.
															 //const unsigned int STIMULI_FRAME[STIMULI_NUM] = {2,2,2};//{13, 6, 4}; //�ò�������Ƶ�ʣ���������������ɵ�ÿ��������ռ��֡�������䲻�ܱ�2����ʱ����֡����֡��1.
															 //const unsigned int STIMULI_FRAME[STIMULI_NUM] = {1000, 1000, 1000, 1000, 1000}; //�ò�������Ƶ�ʣ���������������ɵ�ÿ��������ռ��֡�������䲻�ܱ�2����ʱ����֡����֡��1.
															 //֡����Ƶ�ʵĶ�Ӧ(60Hzˢ����)��14-4.28Hz; 13-4.61Hz; 12 - 5Hz;11 - 5.45Hz;10 - 6Hz;9 - 6.67Hz;8 - 7.5Hz;7 - 8.57Hz;6 - 10Hz;5 - 12Hz;4 - 15Hz;3 - 20Hz;2 - 30Hz
															 //static char* IMAGE_ADDRESS [STIMULI_NUM][STIMULI_NUM] = { {"../Res/menu01.bmp","../Res/menu02.bmp","../Res/menu03.bmp"},
															 //														{"../Res/menu11.bmp","../Res/menu12.bmp","../Res/menu13.bmp"},
															 //														{"../Res/menu21.bmp","../Res/menu22.bmp","../Res/menu23.bmp"}};
static char* IMAGE_ADDRESS[STIMULI_NUM + 1][STIMULI_NUM] = { { "../Res/menu21.bmp","../Res/menu32.bmp","../Res/menu33.bmp","../Res/menu44.bmp","../Res/menu44.bmp" }, //**********��ͬʵ��ǰ����ͼƬ����14��34
{ "../Res/menu11.bmp","../Res/menu12.bmp","../Res/menu13.bmp","../Res/menu14.bmp","../Res/menu5.bmp" },
{ "../Res/menu31.bmp","../Res/menu22.bmp","../Res/menu23.bmp","../Res/menu24.bmp","../Res/menu5.bmp" },
{ "../Res/menu31.bmp","../Res/menu22.bmp","../Res/menu24.bmp","../Res/menu34.bmp","../Res/menu5.bmp" },
{ "../Res/menu41.bmp","../Res/menu42.bmp","../Res/menu43.bmp","../Res/menu44.bmp","../Res/menu5.bmp" } }; //��constʱ�ᱨ����ʾ������ͷ�ļ����������壬����cpp�о�û��
static char* TARGET_LABEL[STIMULI_NUM] = { "1","2","3","4","5" }; //������ѡĿ�귢����Ӧ�ļ�����ʶ
																  // ������˸Ŀ���λ������(���Ͻ�����,����0,0)ʱΪ���Ͻ�ͼƬ)
const float L = 330; //ĳ�������
const double TARGET_X[STIMULI_NUM] = {
	INTERFACE_WIDTH / 2.0 - (TARGET_WIDTH / 2.0) - 80,
	INTERFACE_WIDTH / 2.0 - (TARGET_WIDTH / 2.0) - L - 80,
	INTERFACE_WIDTH / 2.0 - (TARGET_WIDTH / 2.0) + L - 80,
	INTERFACE_WIDTH / 2.0 - (TARGET_WIDTH / 2.0) - 80,
	INTERFACE_WIDTH - TARGET_WIDTH - 50 };
const double TARGET_Y[STIMULI_NUM] = {
	INTERFACE_HEIGHT / 2.0 - (TARGET_HEIGHT / 2.0) - L,
	INTERFACE_HEIGHT / 2.0 - (TARGET_HEIGHT / 2.0),
	INTERFACE_HEIGHT / 2.0 - (TARGET_HEIGHT / 2.0),
	INTERFACE_HEIGHT / 2.0 - (TARGET_HEIGHT / 2.0) + L,
	30 + 50 };
/***********************/
// �����Ϸ�ָʾ��ͷ����ʾʱ��
/**********************/
const unsigned int KEY_ARROW = 360; //�������ͷ��ʾ��֡��
const unsigned int SERIAL_ARROW = 60; //���յ����ڷ��������ͷ����ʾ֡��
									  /***********************/
									  // ���ڱ��
									  /**********************/
const unsigned int SERIAL_PORT = 9;


/***********************/
// �����������е��õĺ���
/**********************/
HWND D3D_CreateWindow(HINSTANCE, HINSTANCE, LPSTR, int); // ����DirectX����
int D3D_init(HWND);//��ʼ��Direct3D�ӿڶ���
void D3D_Display(HWND); // ��Direct3D�����Ͻ�����ʾ
void D3D_Serial(); //�������ݵĽ��պ���Ӧ����
void D3D_KeyResponse(HWND); //�԰���������Ӧ�ĺ���
void D3D_Release(); //�ͷ������Դ