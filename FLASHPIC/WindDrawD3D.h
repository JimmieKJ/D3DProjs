#ifndef WIND_DRAW_D3D_H
#define  WIND_DRAW_D3D_H

#include <process.h>  
#include <assert.h>  
#include <iostream>
#include<stdio.h>
#include<stdlib.h> 
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <fcntl.h>
#include <sstream>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <d3dx9.h>
#include <xnamath.h>
#include <limits>
#include "resource.h"
#include <assert.h>  
#include <tchar.h>
#include "ShareStackSpace.cpp"

//#pragma comment(linker,"/subsystem:\"Windows\" /entry:\"mainCRTStartup\"")

//ID3D11RenderTargetView* g_pOffscreenParticleColorRTV = NULL;
//static ID3D11Texture2D* g_pD3DTexture2D_Capture = NULL;

#define IMAGE_WIDTH     798
#define IMAGE_HEIGHT    598
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }

// Generic wrapper around a DIB with a 32-bit color depth.
struct Image
{
	int width;
	int height;
	int pitch;
	HDC hdc;
	HBITMAP hBitmap;
	BITMAPINFO info;
	BYTE *pPixels;
};


extern  HANDLE  ghDataEvent;
extern HANDLE  g_HEventQuit;
//extern ShareStackSpace<std::string> shareStackPic;
extern queue<std::string> g_strQuence;
extern int g_iMaxQueneSize;
extern  HANDLE  g_hMutex;
extern HANDLE g_hEmpty;
extern HANDLE g_hFull;
extern HANDLE  g_HEventQuit;
extern bool g_bIsConnTCP;

class WindDrawD3D
{
public:
	WindDrawD3D();
	~WindDrawD3D()
	{

	}

	void setWindowAlpha(int alpha)
	{
		if (mhWnd)
		{
			if (g_TotalFrameNumber == 0)
			{
				SetLayeredWindowAttributes(mhWnd, 0, 0, LWA_ALPHA);//ALPHA
			}
			else
			{
				mAlpha = alpha;
				SetLayeredWindowAttributes(mhWnd, 0, alpha, LWA_ALPHA);//ALPHA
			}
		}
	}
	//准备导出 declspec_ Dll  export
	//	动态添加图片和其显示时间  	ps:本类唯一对外接口，一般是读取config文件结束后就可以调用了
	void addPictureAndTime(std::string strPictureName, int numberFrame);

	//准备导出 declspec_ Dll  export
	//	暴露的设置位置的接口
	void setWindowPosition(int ArgPositionX = 0, int ArgPositionY= 0, int ArgWidth = 160, int ArgHeight = 160)
	{
		if (mhWnd)
		{
			//windows窗体 SetWindowPos 宽高 132 40是最小值
			SetWindowPos(mhWnd, HWND_TOPMOST, ArgPositionX, ArgPositionY, ArgWidth, ArgWidth, SWP_NOACTIVATE);//置顶  不激活窗口
		}
	}
	//准备导出 declspec_ Dll  export
	//	从配置文件config获取 纹理信息和窗体信息
	void  getDataFromConfigFile(int nThreadID);



	//-------------------------------------------------------------------------------------------------------------------------
	static	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	/*闪缩图片程序的主体，相当于简易版D3D程序的winmain函数
	*/
	unsigned int WINAPI  StartThreadMainFunc(void *ptr, bool isERP);	//int  wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
	//-------------------------------------------------------------------------------------------------------------------------
private:
	UINT WINAPI SubThread(void* ptr);
	HRESULT InitDevice();
	void CleanupDevice();
	//--------------------------------------------------------------------------------------
	// Render a frame in ssvep
	//--------------------------------------------------------------------------------------
	void Render(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6); 
	//--------------------------------------------------------------------------------------
	// Render a frame in ERP
	//--------------------------------------------------------------------------------------
	void RenderInERP(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);

	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow, int nThreadID);

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

public:
	int  mScopeNum;
	int capcity = 1024;
	

	//void setShareStackSpace(ShareStackSpace<std::string> shareStackPic);

	void printPictureNumbAndTime()//显示 图片出现的时刻ERP
	{
		printf("pictureNum is: %d   , the Show Picture time is:%f s\n", mThreadID, mbeginShowPictureTime);//
	}
	void printNum_And_BlackAndTime()//显示变黑的时刻ERP
	{
		printf("pictureNum is: %d   ,  the In  Black  time is: %f s\n", mThreadID, mbeginShowBlackTime);
	}
	double  getBeginPictureTime()
	{
		return mbeginShowPictureTime;
	}
	double  getBeginBlackTime()
	{
		return mbeginShowBlackTime;
	}
	double  mbeginShowPictureTime;
	double  mbeginShowBlackTime;
	bool mIsPictureShow;
	bool mIsBlackShow;

	bool mIsERP;
	bool  mIsBeginPlay;
	bool  mIsEndPlay;

	inline  void  setBeginPlay(bool bBeginPlay)
	{
		mIsBeginPlay = bBeginPlay;
		mCount_SetRV = 0;//等于 0后 才能  重新开启 paly
	}
	inline  bool  getBeginPlay()
	{
		return mIsBeginPlay ;
	}
	inline  void  setEndPlay(bool bEndPlay)
	{
		mIsEndPlay = bEndPlay;
	}
	inline  bool  getEndPlay()
	{
		return mIsEndPlay;
	}
	inline int getStrRoundEndMsgLen()
	{
		return strRoundEndMsgLen;
	}
	inline std::string getStrRoundEndMsg()
	{
		if (strRoundEndMsg.empty())
			strRoundEndMsg = "";
		return strRoundEndMsg;
	}
private:
	std::string strTCPSendTime;
 
	int mThreadID;
	HINSTANCE  mhInstance;
	double  mTimeLast ;
	double mDeltaTime;
	bool isFirst ;
	std::string strRoundEndMsg;
	int strRoundEndMsgLen = 0;
	int mArgPositionX=0, mArgPositionY =0, mArgWidth = 160, mArgHeight = 320;
	FILE *fpOutputTime;
	ID3D11SamplerState*                 g_pSamplerLinear = NULL;
	ID3D11RasterizerState * g_pRasterState=NULL;
	int mCount_SetRV = 0;
	Image g_image;
	IDirect3DSurface9 *g_pRenderTargetSurface;
	IDirect3DSurface9 *g_pDepthStencilSurface;
	D3DXMATRIX g_worldMtx;
	int mNFrames ;
	int nCount ;
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double time;

	int g_NumberQuat ;

	int g_ArgN1 ;
	int g_ArgM1 ;
	int g_ArgNM1;

	int g_ArgN2 ;
	int g_ArgM2 ;
	int g_ArgNM2 ;

	int g_ArgN3 ;
	int g_ArgM3 ;
	int g_ArgNM3 ;

	int g_ArgN4 ;
	int g_ArgM4 ;
	int g_ArgNM4 ;

	int g_ArgN5 ;
	int g_ArgM5 ;
	int g_ArgNM5 ;

	int g_ArgN6 ;
	int g_ArgM6 ;
	int g_ArgNM6 ;
	//--------------------------------------------------------------------------------------
	// Global Variables
	//--------------------------------------------------------------------------------------
	int mAlpha;
	HINSTANCE                           g_hInst ;
	HWND                                mhWnd ;
	D3D_DRIVER_TYPE                     g_driverType ;
	D3D_FEATURE_LEVEL                   g_featureLevel ;
	ID3D11Device*                       g_pd3dDevice ;
	ID3D11DeviceContext*                g_pImmediateContext ;
	ID3D11DeviceContext*                g_pImmediateContext2 ;
	IDXGISwapChain*                     g_pSwapChain ;
	ID3D11RenderTargetView*             g_pRenderTargetView ;
	ID3D11Texture2D*                    g_pDepthStencil ;

	ID3D11Texture2D*                    g_Texture2D_RTT ;

	ID3D11DepthStencilView*             g_pDepthStencilView ;
	ID3D11VertexShader*                 g_pVertexShader ;
	ID3D11PixelShader*                  g_pPixelShader ;
	ID3D11InputLayout*                  g_pVertexLayout ;
	ID3D11Buffer*                       g_pVertexBuffer ;
	ID3D11Buffer*                       g_pIndexBuffer ;
	ID3D11Buffer*                       g_pCBNeverChanges ;
	ID3D11Buffer*                       g_pCBChangeOnResize ;
	ID3D11Buffer*                       g_pCBChangesEveryFrame ;


	typedef std::vector<	ID3D11ShaderResourceView* >  VecShaderResourceView;			//ID3D11ShaderResourceView*           g_pTextureRV = NULL;
	VecShaderResourceView mVecTextureResourceView;//所有的需求都是这里的
	struct String_Int 
	{
	public:
		String_Int(std::string str, int num):strPictureName(str)
			,NumberFrame(num)
		{
		}
		std::string strPictureName;
		int NumberFrame;
	};
	typedef std::vector<String_Int> VecStringInt;
	VecStringInt mVec_string_int;//所有的需求都在这里  //让后再render里更新这里的东西作为真是的使用起来
	int g_TotalFrameNumber;

	ID3D11ShaderResourceView*           g_pTextureRV17 ;

	XMMATRIX                            g_World;
	//XMMATRIX                            g_World2;
	//XMMATRIX                            mMatrixView;//
	XMFLOAT4X4		mFloat44View;
	XMMATRIX                            g_Projection;
	//XMMATRIX                            g_Ortho;//
	XMFLOAT4X4		mFloat4X4Ortho;
	XMFLOAT4                            g_vMeshColor;

	struct SimpleVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
	};

	struct CBNeverChanges
	{
		XMMATRIX mView;
	};

	struct CBChangeOnResize
	{
		XMMATRIX mProjection;
	};

	struct CBChangesEveryFrame
	{
		XMMATRIX mWorld;
		XMFLOAT4 vMeshColor;
	};

	volatile bool gbExitThread ;

};

#endif