#ifndef  FLASHPICTURE_H
#define  FLASHPICTURE_H


#   	if defined( EXPORT_FLASHPICTURE )
#       	define LiuExport __declspec( dllexport )
#   	else
#       	 define LiuExport __declspec( dllimport )
#       endif



// 此类是从 OneDll.dll 导出的
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

	//示例：addPictureAndFrameNum(3,"1234.png", 60);
	//运行时加载其他图片或者修改帧率
	LiuExport  int   addPictureAndFrameNum(int NumberPicture, char * strImagePictureName, int numFrame);
	//设置图片的位置
	LiuExport  int   setPicturePosition(int NumberPicture,int x,int y,int width,int height);
	//设置每个窗体的透明度
	LiuExport int   setWindowAlpha(int alpha );

	//hWnd表示【调用闪烁图片的程序的窗口句柄】，如果设置为NULL则不在 闪烁图片显示完后 将鼠标键盘焦点设置给【调用闪烁图片的程序的窗口】
	LiuExport  int   runFlashPicture(unsigned   int  hWnd);      //_TCHAR    //这里是真正的main函数，程序的入口int argc, char* argv[]

	//hWnd表示【调用闪烁图片的程序的窗口句柄】，如果设置为NULL则不在 闪烁图片显示完后 将鼠标键盘焦点设置给【调用闪烁图片的程序的窗口】
	LiuExport  int   runFlashPictureERP(unsigned   int  hWnd);      //_TCHAR    //这里是真正的main函数，程序的入口int argc, char* argv[]

#ifdef __cplusplus
}
#endif

#endif