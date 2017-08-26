#include "SSVEP_Stimuli.h"


SSVEP_Stimuli::SSVEP_Stimuli(void)
{
}

SSVEP_Stimuli::~SSVEP_Stimuli(void)
{
}
/***********************/
// 初始化SSVEP激励

// 输入参数:
//	   d3ddev - D3D设备指针；
//	   FramePerCycle - 亮灭所组成的一个周期的帧数，当其不能被2整除时亮多一帧；
//	   rect - SSVEP激励所在位置坐标；
//	   Addr1,Addr2 - 分别对应亮、灭两幅图片的所在位置
//	   Psurface_On,Psurface_Off - 分别是用于存储亮、灭两幅图片的缓存地址
/**********************/
void SSVEP_Stimuli::StimuliInit(LPDIRECT3DDEVICE9 d3ddev, int FramePerCycle, RECT rect, char* Addr1, char* Addr2, LPDIRECT3DSURFACE9* Psurface_On, LPDIRECT3DSURFACE9* Psurface_Off)
{
	//其中FramePerCycle是亮灭所组成的一个周期的帧数
	SSVEP_Stimuli::LoadImage(d3ddev, Addr1, Addr2, Psurface_On, Psurface_Off); //从地址中载入图片
	OffFrame = floor(float(FramePerCycle / 2)); //向下取整
	OnFrame = FramePerCycle - OffFrame; //当不能被整除时亮的帧数大一
	FlashCount = OnFrame; //闪烁计数值
	StimuliRect = rect; //闪烁图片的四角坐标
}
/***********************/
// 从地址中导入图片，并放入缓存

// 输入参数:
//	   d3ddev - D3D设备指针；
//	   Addr1,Addr2 - 分别对应亮、灭两幅图片的所在位置
//	   Psurface_On,Psurface_Off - 分别是用于存储亮、灭两幅图片的缓存地址
/**********************/
void SSVEP_Stimuli::LoadImage(LPDIRECT3DDEVICE9 d3ddev, char* Addr1, char* Addr2, LPDIRECT3DSURFACE9* Psurface_On, LPDIRECT3DSURFACE9* Psurface_Off)
{
	//create surface  
	HRESULT result = d3ddev->CreateOffscreenPlainSurface(
		300,                //width of the surface -- 太低会使图片显示不清楚
		300,                //height of the surface  
		D3DFMT_X8R8G8B8,    //surface format  
		D3DPOOL_DEFAULT,    //memory pool to use  
		Psurface_On,           //pointer to the surface  
		NULL);
	//if (resule)
	//加载位图
	result = D3DXLoadSurfaceFromFile(
		*Psurface_On,            //destination surface  
		NULL,               //destination palette  
		NULL,               //destination rectangle  
		Addr1,     //source filename  
		NULL,               //source rectangle  
		D3DX_DEFAULT,       //controls how image is filtered  
		0,                  //for transparency (0 for none)  
		NULL);              //source image info (usually NULL)

							//create surface  
	result = d3ddev->CreateOffscreenPlainSurface(
		300,                //width of the surface  
		300,                //height of the surface  
		D3DFMT_X8R8G8B8,    //surface format  
		D3DPOOL_DEFAULT,    //memory pool to use  
		Psurface_Off,           //pointer to the surface  
		NULL);  //reserved (always NULL)  
				//加载位图
	result = D3DXLoadSurfaceFromFile(
		*Psurface_Off,            //destination surface  
		NULL,               //destination palette  
		NULL,               //destination rectangle  
		Addr2,     //source filename  
		NULL,               //source rectangle  
		D3DX_DEFAULT,       //controls how image is filtered  
		0,                  //for transparency (0 for none)  
		NULL);              //source image info (usually NULL)
							//ArrowState = false; //箭头是否显示的状态
	FlashState = true; //当前是该亮还是灭的状态
}
/***********************/
// 根据状态变量FlashState，在界面上绘制亮或灭的图片

// 输入参数:
//	   d3ddev - D3D设备指针；
//	   surface_On,surface_Off - 分别是用于存储亮、灭两幅图片的缓存地址
//	   surface_Arrow - 存储箭头图片的缓存地址
//     backbuffer - (似乎是)界面显示用的缓存地址
/**********************/
void SSVEP_Stimuli::Flash(LPDIRECT3DDEVICE9 d3ddev, LPDIRECT3DSURFACE9 surface_On, LPDIRECT3DSURFACE9 surface_Off, LPDIRECT3DSURFACE9 surface_Arrow, LPDIRECT3DSURFACE9 surface_Red, LPDIRECT3DSURFACE9 backbuffer)
{
	if (ArrowState == true)
	{
		RECT ArrowRect; //绘制箭头的坐标位置在图片的正上方
		ArrowRect.left = StimuliRect.left;
		ArrowRect.right = StimuliRect.right;
		ArrowRect.bottom = StimuliRect.top;
		ArrowRect.top = ArrowRect.bottom - 30;
		d3ddev->StretchRect(surface_Arrow, NULL, backbuffer, &ArrowRect, D3DTEXF_NONE);
		ArrowCount--;
		if (ArrowCount <= 0)
		{
			//ArrowCount = 0; //箭头的显示时间
			ArrowState = false;
		}
	}

	if (RedState == true)
	{
		RECT RedRect; //绘制红色方框的坐标位置
		RedRect.left = (StimuliRect.left + StimuliRect.right) / 2 - 90;
		RedRect.right = (StimuliRect.left + StimuliRect.right) / 2 + 90;
		RedRect.bottom = (StimuliRect.top + StimuliRect.bottom) / 2 + 90;
		RedRect.top = (StimuliRect.top + StimuliRect.bottom) / 2 - 90;
		d3ddev->StretchRect(surface_Red, NULL, backbuffer, &RedRect, D3DTEXF_NONE);
		RedCount--;
		if (RedCount <= 0)
		{
			RedState = false;
		}
	}

	if (FlashState == true)
		d3ddev->StretchRect(surface_On, NULL, backbuffer, &StimuliRect, D3DTEXF_NONE);
	else
		d3ddev->StretchRect(surface_Off, NULL, backbuffer, &StimuliRect, D3DTEXF_NONE);
}
/***********************/
// 使能箭头显示

// 输入参数:
//	   Frame - 总共显示的帧数；
/**********************/
void SSVEP_Stimuli::EnableArrow(int Frame)
{
	ArrowCount = Frame;
	ArrowState = true;
}
/***********************/
// 使能箭头显示
// 2015.03.15

// 输入参数:
//	   Frame - 总共显示的帧数；
/**********************/
void SSVEP_Stimuli::EnableRed(int Frame)
{
	RedCount = Frame;
	RedState = true;
}
/***********************/
// 计数值减一，以此更改FlashState的状态从而实现界面上的图片闪烁
/**********************/
void SSVEP_Stimuli::DecCount()
{
	FlashCount--;
	if (FlashCount == 0)
	{
		FlashState = ~FlashState;
		if (FlashState == true) //灭状态结束，开始亮状态的计数
			FlashCount = OnFrame;
		else
			FlashCount = OffFrame;
	}
}
