#ifndef  FLASHPICTURE_H
#define  FLASHPICTURE_H


#   	if defined( EXPORT_FLASHPICTURE )
#       	define LiuExport __declspec( dllexport )
#   	else
#       	 define LiuExport __declspec( dllimport )
#       endif



// �����Ǵ� OneDll.dll ������
#ifdef __cplusplus
extern "C"
{
#endif


// 	class  OneDll
// 	{
// 	public:
// 		OneDll(void);
// 		~OneDll(void);
// 
// 		int m_a;
// 	};

	//ʾ����addPictureAndFrameNum(3,"1234.png", 60);
	//����ʱ��������ͼƬ�����޸�֡��
	LiuExport  int   addPictureAndFrameNum(int NumberPicture, char * strImagePictureName, int numFrame);
	//����ͼƬ��λ��
	LiuExport  int   setPicturePosition(int NumberPicture,int x,int y,int width,int height);
	//����ÿ�������͸����
	LiuExport int   setWindowAlpha(int alpha );

	//hWnd��ʾ��������˸ͼƬ�ĳ���Ĵ��ھ�������������ΪNULL���� ��˸ͼƬ��ʾ��� �������̽������ø���������˸ͼƬ�ĳ���Ĵ��ڡ�
	LiuExport  int   runFlashPicture(unsigned   int  hWnd);      //_TCHAR    //������������main��������������int argc, char* argv[]

	//hWnd��ʾ��������˸ͼƬ�ĳ���Ĵ��ھ�������������ΪNULL���� ��˸ͼƬ��ʾ��� �������̽������ø���������˸ͼƬ�ĳ���Ĵ��ڡ�
	LiuExport  int   runFlashPictureERP(unsigned   int  hWnd);      //_TCHAR    //������������main��������������int argc, char* argv[]

#ifdef __cplusplus
}
#endif

#endif