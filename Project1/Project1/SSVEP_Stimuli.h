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
	void DecCount(); //������ǰ��֡�������жϸ���ʾOn����Off
	void StimuliInit(LPDIRECT3DDEVICE9 d3ddev, int FramePerCycle, RECT rect, char* Addr1, char* Addr2, LPDIRECT3DSURFACE9* surface_On, LPDIRECT3DSURFACE9* surface_Off);
	void SSVEP_Stimuli::LoadImage(LPDIRECT3DDEVICE9 d3ddev, char* Addr1, char* Addr2, LPDIRECT3DSURFACE9* Psurface_On, LPDIRECT3DSURFACE9* Psurface_Off);
	void EnableArrow(int Frame); //��һ��������ʾ��ʾ����֡�ļ�ͷ
	void EnableRed(int Frame); //��һ��������ʾ��ʾ����֡�ĺ�ɫ���
private:
	RECT StimuliRect; //��˸ͼƬ���Ľ�����
	int FlashCount; //��˸����ֵ��С�ڵ�����ʱ��ת
	int OnFrame; //�ܹ�����֡��
	int OffFrame; //�ܹ����֡��
	BOOL FlashState;//�����״̬����
	BOOL ArrowState; //�Ƿ���ʾ��ɫ��ͷ��״̬����
	int ArrowCount; //��ͷ�����������ڼ�����ͷ�����˶���֡
	int RedCount; //��ɫ���������
	BOOL RedState; //�Ƿ���ʾ��ɫ���
};

