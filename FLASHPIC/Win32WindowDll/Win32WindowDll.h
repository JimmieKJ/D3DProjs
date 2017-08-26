#ifndef  WIN32WINDOSW_H
#define  WIN32WINDOSW_H




#   	if defined( EXPORT_WIN32WINDOSW_DLL )
#       	define WinExport __declspec( dllexport )
#   	else
#       	 define WinExport __declspec( dllimport )
#       endif



	// 此类是从 OneDll.dll 导出的
	#ifdef __cplusplus
	extern "C"
	{
	#endif

		//示例：addPictureAndFrameNum(3,"1234.png", 60);
	// 	WinExport  int   addPictureAndFrameNum(int NumberPicture, char * strImagePictureName, int numFrame);
	// 
	// 	WinExport  int   setPicturePosition(int NumberPicture, int x, int y, int width, int height);
	// 
	// 	WinExport int   setWindowAlpha(int alpha);

		//hWnd表示【调用闪烁图片的程序的窗口句柄】，如果设置为NULL则不在 闪烁图片显示完后 将鼠标键盘焦点设置给【调用闪烁图片的程序的窗口】
		WinExport  int   ExportMainStartRun();      //_TCHAR    //这里是真正的main函数，程序的入口int argc, char* argv[]
		WinExport  int  	ExportSetBreaklineData(int x,int y);

		WinExport  int  	rangsound();

	#ifdef __cplusplus
	}
	#endif


#endif