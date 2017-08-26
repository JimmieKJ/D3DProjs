#include "SSVEP_Stimuli.h"


SSVEP_Stimuli::SSVEP_Stimuli(void)
{
}

SSVEP_Stimuli::~SSVEP_Stimuli(void)
{
}
/***********************/
// ��ʼ��SSVEP����

// �������:
//	   d3ddev - D3D�豸ָ�룻
//	   FramePerCycle - ��������ɵ�һ�����ڵ�֡�������䲻�ܱ�2����ʱ����һ֡��
//	   rect - SSVEP��������λ�����ꣻ
//	   Addr1,Addr2 - �ֱ��Ӧ����������ͼƬ������λ��
//	   Psurface_On,Psurface_Off - �ֱ������ڴ洢����������ͼƬ�Ļ����ַ
/**********************/
void SSVEP_Stimuli::StimuliInit(LPDIRECT3DDEVICE9 d3ddev, int FramePerCycle, RECT rect, char* Addr1, char* Addr2, LPDIRECT3DSURFACE9* Psurface_On, LPDIRECT3DSURFACE9* Psurface_Off)
{
	//����FramePerCycle����������ɵ�һ�����ڵ�֡��
	SSVEP_Stimuli::LoadImage(d3ddev, Addr1, Addr2, Psurface_On, Psurface_Off); //�ӵ�ַ������ͼƬ
	OffFrame = floor(float(FramePerCycle / 2)); //����ȡ��
	OnFrame = FramePerCycle - OffFrame; //�����ܱ�����ʱ����֡����һ
	FlashCount = OnFrame; //��˸����ֵ
	StimuliRect = rect; //��˸ͼƬ���Ľ�����
}
/***********************/
// �ӵ�ַ�е���ͼƬ�������뻺��

// �������:
//	   d3ddev - D3D�豸ָ�룻
//	   Addr1,Addr2 - �ֱ��Ӧ����������ͼƬ������λ��
//	   Psurface_On,Psurface_Off - �ֱ������ڴ洢����������ͼƬ�Ļ����ַ
/**********************/
void SSVEP_Stimuli::LoadImage(LPDIRECT3DDEVICE9 d3ddev, char* Addr1, char* Addr2, LPDIRECT3DSURFACE9* Psurface_On, LPDIRECT3DSURFACE9* Psurface_Off)
{
	//create surface  
	HRESULT result = d3ddev->CreateOffscreenPlainSurface(
		300,                //width of the surface -- ̫�ͻ�ʹͼƬ��ʾ�����
		300,                //height of the surface  
		D3DFMT_X8R8G8B8,    //surface format  
		D3DPOOL_DEFAULT,    //memory pool to use  
		Psurface_On,           //pointer to the surface  
		NULL);
	//if (resule)
	//����λͼ
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
				//����λͼ
	result = D3DXLoadSurfaceFromFile(
		*Psurface_Off,            //destination surface  
		NULL,               //destination palette  
		NULL,               //destination rectangle  
		Addr2,     //source filename  
		NULL,               //source rectangle  
		D3DX_DEFAULT,       //controls how image is filtered  
		0,                  //for transparency (0 for none)  
		NULL);              //source image info (usually NULL)
							//ArrowState = false; //��ͷ�Ƿ���ʾ��״̬
	FlashState = true; //��ǰ�Ǹ����������״̬
}
/***********************/
// ����״̬����FlashState���ڽ����ϻ����������ͼƬ

// �������:
//	   d3ddev - D3D�豸ָ�룻
//	   surface_On,surface_Off - �ֱ������ڴ洢����������ͼƬ�Ļ����ַ
//	   surface_Arrow - �洢��ͷͼƬ�Ļ����ַ
//     backbuffer - (�ƺ���)������ʾ�õĻ����ַ
/**********************/
void SSVEP_Stimuli::Flash(LPDIRECT3DDEVICE9 d3ddev, LPDIRECT3DSURFACE9 surface_On, LPDIRECT3DSURFACE9 surface_Off, LPDIRECT3DSURFACE9 surface_Arrow, LPDIRECT3DSURFACE9 surface_Red, LPDIRECT3DSURFACE9 backbuffer)
{
	if (ArrowState == true)
	{
		RECT ArrowRect; //���Ƽ�ͷ������λ����ͼƬ�����Ϸ�
		ArrowRect.left = StimuliRect.left;
		ArrowRect.right = StimuliRect.right;
		ArrowRect.bottom = StimuliRect.top;
		ArrowRect.top = ArrowRect.bottom - 30;
		d3ddev->StretchRect(surface_Arrow, NULL, backbuffer, &ArrowRect, D3DTEXF_NONE);
		ArrowCount--;
		if (ArrowCount <= 0)
		{
			//ArrowCount = 0; //��ͷ����ʾʱ��
			ArrowState = false;
		}
	}

	if (RedState == true)
	{
		RECT RedRect; //���ƺ�ɫ���������λ��
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
// ʹ�ܼ�ͷ��ʾ

// �������:
//	   Frame - �ܹ���ʾ��֡����
/**********************/
void SSVEP_Stimuli::EnableArrow(int Frame)
{
	ArrowCount = Frame;
	ArrowState = true;
}
/***********************/
// ʹ�ܼ�ͷ��ʾ
// 2015.03.15

// �������:
//	   Frame - �ܹ���ʾ��֡����
/**********************/
void SSVEP_Stimuli::EnableRed(int Frame)
{
	RedCount = Frame;
	RedState = true;
}
/***********************/
// ����ֵ��һ���Դ˸���FlashState��״̬�Ӷ�ʵ�ֽ����ϵ�ͼƬ��˸
/**********************/
void SSVEP_Stimuli::DecCount()
{
	FlashCount--;
	if (FlashCount == 0)
	{
		FlashState = ~FlashState;
		if (FlashState == true) //��״̬��������ʼ��״̬�ļ���
			FlashCount = OnFrame;
		else
			FlashCount = OffFrame;
	}
}
