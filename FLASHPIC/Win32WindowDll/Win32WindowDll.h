#ifndef  WIN32WINDOSW_H
#define  WIN32WINDOSW_H




#   	if defined( EXPORT_WIN32WINDOSW_DLL )
#       	define WinExport __declspec( dllexport )
#   	else
#       	 define WinExport __declspec( dllimport )
#       endif



	// �����Ǵ� OneDll.dll ������
	#ifdef __cplusplus
	extern "C"
	{
	#endif

		//ʾ����addPictureAndFrameNum(3,"1234.png", 60);
	// 	WinExport  int   addPictureAndFrameNum(int NumberPicture, char * strImagePictureName, int numFrame);
	// 
	// 	WinExport  int   setPicturePosition(int NumberPicture, int x, int y, int width, int height);
	// 
	// 	WinExport int   setWindowAlpha(int alpha);

		//hWnd��ʾ��������˸ͼƬ�ĳ���Ĵ��ھ�������������ΪNULL���� ��˸ͼƬ��ʾ��� �������̽������ø���������˸ͼƬ�ĳ���Ĵ��ڡ�
		WinExport  int   ExportMainStartRun();      //_TCHAR    //������������main��������������int argc, char* argv[]
		WinExport  int  	ExportSetBreaklineData(int x,int y);

		WinExport  int  	rangsound();

	#ifdef __cplusplus
	}
	#endif


#endif