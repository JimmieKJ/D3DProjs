#pragma once
#include <d3d9.h>  
#include <d3dx9.h>
#include <math.h>
//#include <windows.h> 
class SSVEP_Stimuli
{
public:
	SSVEP_Stimuli();
	~SSVEP_Stimuli(void);
	void Flash(LPDIRECT3DDEVICE9 d3ddev, LPDIRECT3DSURFACE9 surface_On, LPDIRECT3DSURFACE9 surface_Off, LPDIRECT3DSURFACE9 surface_Arrow, LPDIRECT3DSURFACE9 surface_Red, LPDIRECT3DSURFACE9 backbuffer);
	void DecCount(); //计数当前的帧数，并判断该显示On还是Off
	void StimuliInit(LPDIRECT3DDEVICE9 d3ddev, int FramePerCycle, RECT rect, char* Addr1, char* Addr2, LPDIRECT3DSURFACE9* surface_On, LPDIRECT3DSURFACE9* surface_Off);
	void SSVEP_Stimuli::LoadImage(LPDIRECT3DDEVICE9 d3ddev, char* Addr1, char* Addr2, LPDIRECT3DSURFACE9* Psurface_On, LPDIRECT3DSURFACE9* Psurface_Off);
	void EnableArrow(int Frame); //第一个参数表示显示多少帧的箭头
	void EnableRed(int Frame); //第一个参数表示显示多少帧的红色外框
private:
	RECT StimuliRect; //闪烁图片的四角坐标
	int FlashCount; //闪烁计数值，小于等于零时翻转
	int OnFrame; //总共亮的帧数
	int OffFrame; //总共灭的帧数
	BOOL FlashState;//亮灭的状态变量
	BOOL ArrowState; //是否显示黄色箭头的状态变量
	int ArrowCount; //箭头计数器，用于计数箭头出现了多少帧
	int RedCount; //红色方框计数器
	BOOL RedState; //是否显示红色外框
};

