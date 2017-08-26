#pragma once
#ifdef _WINDOWS_
#undef _WINDOWS_
#endif
#include <d3d9.h>  
#include <d3dx9.h> 
#include <string> 
using namespace std;
#include "SSVEP_Stimuli.h"
#include "MySerialPort.h" //串口通信函数
//#include <windows.h>  

/***********************/
// Direct3D窗口的相关参数
/**********************/
// 显示界面的宽和高（像素数）
const unsigned int INTERFACE_WIDTH = 1350;
const unsigned int INTERFACE_HEIGHT = 1000;
const string APPTITLE = "Multilayer SSVEP Interface";//窗口标题，string类型使用的时候必须要有#include <string> 和 using namespace std;两句
													 /***********************/
													 // 按键响应
													 /**********************/
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0) 
													 /***********************/
													 // 定义界面参数常量
													 /**********************/
													 // 图片的像素尺寸
static float  TARGET_WIDTH = 150.0;
static float TARGET_HEIGHT = 150.0;
/***********************/
// 激励目标的参数，当改变激励数量时需要相应调整
/**********************/
const unsigned int STIMULI_NUM = 5;
const unsigned int STIMULI_FRAME[STIMULI_NUM] = { 13,4, 3 }; //该参数并非频率，而是由亮灭所组成的每个周期所占的帧数，当其不能被2整除时，亮帧比灭帧多1.
															 //const unsigned int STIMULI_FRAME[STIMULI_NUM] = {2,2,2};//{13, 6, 4}; //该参数并非频率，而是由亮灭所组成的每个周期所占的帧数，当其不能被2整除时，亮帧比灭帧多1.
															 //const unsigned int STIMULI_FRAME[STIMULI_NUM] = {1000, 1000, 1000, 1000, 1000}; //该参数并非频率，而是由亮灭所组成的每个周期所占的帧数，当其不能被2整除时，亮帧比灭帧多1.
															 //帧数与频率的对应(60Hz刷新率)：14-4.28Hz; 13-4.61Hz; 12 - 5Hz;11 - 5.45Hz;10 - 6Hz;9 - 6.67Hz;8 - 7.5Hz;7 - 8.57Hz;6 - 10Hz;5 - 12Hz;4 - 15Hz;3 - 20Hz;2 - 30Hz
															 //static char* IMAGE_ADDRESS [STIMULI_NUM][STIMULI_NUM] = { {"../Res/menu01.bmp","../Res/menu02.bmp","../Res/menu03.bmp"},
															 //														{"../Res/menu11.bmp","../Res/menu12.bmp","../Res/menu13.bmp"},
															 //														{"../Res/menu21.bmp","../Res/menu22.bmp","../Res/menu23.bmp"}};
static char* IMAGE_ADDRESS[STIMULI_NUM + 1][STIMULI_NUM] = { { "../Res/menu21.bmp","../Res/menu32.bmp","../Res/menu33.bmp","../Res/menu44.bmp","../Res/menu44.bmp" }, //**********不同实验前更换图片：从14到34
{ "../Res/menu11.bmp","../Res/menu12.bmp","../Res/menu13.bmp","../Res/menu14.bmp","../Res/menu5.bmp" },
{ "../Res/menu31.bmp","../Res/menu22.bmp","../Res/menu23.bmp","../Res/menu24.bmp","../Res/menu5.bmp" },
{ "../Res/menu31.bmp","../Res/menu22.bmp","../Res/menu24.bmp","../Res/menu34.bmp","../Res/menu5.bmp" },
{ "../Res/menu41.bmp","../Res/menu42.bmp","../Res/menu43.bmp","../Res/menu44.bmp","../Res/menu5.bmp" } }; //用const时会报错，显示不能在头文件中这样定义，放入cpp中就没了
static char* TARGET_LABEL[STIMULI_NUM] = { "1","2","3","4","5" }; //根据所选目标发送相应的激励标识
																  // 六个闪烁目标的位置坐标(左上角坐标,当（0,0)时为左上角图片)
const float L = 330; //某距离参数
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
// 激励上方指示箭头的显示时间
/**********************/
const unsigned int KEY_ARROW = 360; //按键后箭头显示的帧数
const unsigned int SERIAL_ARROW = 60; //接收到串口分类结果后箭头的显示帧数
									  /***********************/
									  // 串口编号
									  /**********************/
const unsigned int SERIAL_PORT = 9;


/***********************/
// 需在主程序中调用的函数
/**********************/
HWND D3D_CreateWindow(HINSTANCE, HINSTANCE, LPSTR, int); // 创建DirectX窗口
int D3D_init(HWND);//初始化Direct3D接口对象
void D3D_Display(HWND); // 在Direct3D窗口上进行显示
void D3D_Serial(); //串口数据的接收和响应程序
void D3D_KeyResponse(HWND); //对按键进行响应的函数
void D3D_Release(); //释放相关资源