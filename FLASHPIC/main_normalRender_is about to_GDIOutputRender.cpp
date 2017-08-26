//--------------------------------------------------------------------------------------
// File: Tutorial07.cpp
//
// This application demonstrates texturing
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#include <d3dx9.h>

#include <xnamath.h>
#include "resource.h"

#include<stdio.h>
#include<stdlib.h> 
#include <io.h>

#include <fcntl.h>

#include <iostream>
#include <string>
ID3D11RenderTargetView* g_pOffscreenParticleColorRTV = NULL;

static ID3D11Texture2D* g_pD3DTexture2D_Capture = NULL;

#define IMAGE_WIDTH     798
#define IMAGE_HEIGHT    598

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
Image g_image;
IDirect3DSurface9 *g_pRenderTargetSurface;
IDirect3DSurface9 *g_pDepthStencilSurface;
D3DXMATRIX g_worldMtx;
int g_frames=0;

int nCount = 0;
LARGE_INTEGER nFreq;
LARGE_INTEGER nBeginTime;
LARGE_INTEGER nEndTime;
double time;

int g_NumberQuat = 1;

int g_ArgN1 = 0;
int g_ArgM1 = 0;
int g_ArgNM1 = 0;

int g_ArgN2 = 0;
int g_ArgM2 = 0;
int g_ArgNM2 = 0;

int g_ArgN3 = 0;
int g_ArgM3 = 0;
int g_ArgNM3 = 0;


int g_ArgN4 = 0;
int g_ArgM4 = 0;
int g_ArgNM4 = 0;


int g_ArgN5 = 0;
int g_ArgM5 = 0;
int g_ArgNM5 = 0;

int g_ArgN6 = 0;
int g_ArgM6 = 0;
int g_ArgNM6 = 0;
//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE                           g_hInst = NULL;
HWND                                g_hWnd = NULL;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = NULL;
ID3D11DeviceContext*                g_pImmediateContext = NULL;
ID3D11DeviceContext*                g_pImmediateContext2 = NULL;
IDXGISwapChain*                     g_pSwapChain = NULL;
ID3D11RenderTargetView*             g_pRenderTargetView = NULL;
ID3D11Texture2D*                    g_pDepthStencil = NULL;

ID3D11Texture2D*                    g_Texture2D_RTT = NULL;

ID3D11DepthStencilView*             g_pDepthStencilView = NULL;
ID3D11VertexShader*                 g_pVertexShader = NULL;
ID3D11PixelShader*                  g_pPixelShader = NULL;
ID3D11InputLayout*                  g_pVertexLayout = NULL;
ID3D11Buffer*                       g_pVertexBuffer = NULL;
ID3D11Buffer*                       g_pIndexBuffer = NULL;
ID3D11Buffer*                       g_pCBNeverChanges = NULL;
ID3D11Buffer*                       g_pCBChangeOnResize = NULL;
ID3D11Buffer*                       g_pCBChangesEveryFrame = NULL;
ID3D11ShaderResourceView*           g_pTextureRV = NULL;
ID3D11ShaderResourceView*           g_pTextureRV1_1 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV2 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV2_2 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV3 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV3_3 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV4 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV4_4 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV5 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV5_5 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV6 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV6_6 = NULL;

ID3D11ShaderResourceView*           g_pTextureRV13 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV14 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV15 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV16 = NULL;
ID3D11ShaderResourceView*           g_pTextureRV17 = NULL;

ID3D11SamplerState*                 g_pSamplerLinear = NULL;
XMMATRIX                            g_World;
XMMATRIX                            g_World2;
XMMATRIX                            g_View;
XMMATRIX                            g_Projection;
XMMATRIX                            g_Oth;
XMFLOAT4                            g_vMeshColor(0.7f, 0.7f, 0.7f, 1.0f);

void ImageDestroy(Image *pImage)
{
	if (!pImage)
		return;

	pImage->width = 0;
	pImage->height = 0;
	pImage->pitch = 0;

	if (pImage->hBitmap)
	{
		DeleteObject(pImage->hBitmap);
		pImage->hBitmap = 0;
	}

	if (pImage->hdc)
	{
		DeleteDC(pImage->hdc);
		pImage->hdc = 0;
	}

	memset(&pImage->info, 0, sizeof(pImage->info));
	pImage->pPixels = 0;
}
bool ImageCreate(Image *pImage, int width, int height)
{
	if (!pImage)
		return false;

	// All Windows DIBs are aligned to 4-byte (DWORD) memory boundaries. This
	// means that each scan line is padded with extra bytes to ensure that the
	// next scan line starts on a 4-byte memory boundary. The 'pitch' member
	// of the Image structure contains width of each scan line (in bytes).

	pImage->width = width;
	pImage->height = height;
	pImage->pitch = ((width * 32 + 31) & ~31) >> 3;
	pImage->pPixels = NULL;
	pImage->hdc = CreateCompatibleDC(NULL);

	if (!pImage->hdc)
		return false;

	memset(&pImage->info, 0, sizeof(pImage->info));

	pImage->info.bmiHeader.biSize = sizeof(pImage->info.bmiHeader);
	pImage->info.bmiHeader.biBitCount = 32;
	pImage->info.bmiHeader.biWidth = width;
	pImage->info.bmiHeader.biHeight = -height;
	pImage->info.bmiHeader.biCompression = BI_RGB;
	pImage->info.bmiHeader.biPlanes = 1;

	pImage->hBitmap = CreateDIBSection(pImage->hdc, &pImage->info,
		DIB_RGB_COLORS, (void**)&pImage->pPixels, NULL, 0);

	if (!pImage->hBitmap)
	{
		ImageDestroy(pImage);
		return false;
	}

	GdiFlush();
	return true;
}
using namespace std;
void ImagePreMultAlpha(Image *pImage)
{
	// The per pixel alpha blending API for layered windows deals with
	// pre-multiplied alpha values in the RGB channels. For further details see
	// the MSDN documentation for the BLENDFUNCTION structure. It basically
	// means we have to multiply each red, green, and blue channel in our image
	// with the alpha value divided by 255.
	//
	// Notes:
	// 1. ImagePreMultAlpha() needs to be called before every call to
	//    UpdateLayeredWindow() (in the RedrawLayeredWindow() function).
	//
	// 2. Must divide by 255.0 instead of 255 to prevent alpha values in range
	//    [1, 254] from causing the pixel to become black. This will cause a
	//    conversion from 'float' to 'BYTE' possible loss of data warning which
	//    can be safely ignored.

	if (!pImage)
		return;

	BYTE *pPixel = NULL;

	if (pImage->width * 4 == pImage->pitch)//通常一个像素 rgba 就是4byte
	{
		// This is a special case. When the image width is already a multiple
		// of 4 the image does not require any padding bytes at the end of each
		// scan line. Consequently we do not need to address each scan line
		// separately. This is much faster than the below case where the image
		// width is not a multiple of 4.

		int totalBytes = pImage->width * pImage->height * 4;

		for (int i = 0; i < totalBytes; i += 4)
		{
			pPixel = &pImage->pPixels[i];
			pPixel[0] *= (BYTE)((float)pPixel[3] / 255.0f);
			pPixel[1] *= (BYTE)((float)pPixel[3] / 255.0f);
			pPixel[2] *= (BYTE)((float)pPixel[3] / 255.0f);
		}
	}
	else
	{
		// Width of the image is not a multiple of 4. So padding bytes have
		// been included in the DIB's pixel data. Need to address each scan
		// line separately. This is much slower than the above case where the
		// width of the image is already a multiple of 4.

		for (int y = 0; y < pImage->height; ++y)
		{
			for (int x = 0; x < pImage->width; ++x)
			{
				pPixel = &pImage->pPixels[(y * pImage->pitch) + (x * 4)];
				pPixel[0] *= (BYTE)((float)pPixel[3] / 255.0f);
				pPixel[1] *= (BYTE)((float)pPixel[3] / 255.0f);
				pPixel[2] *= (BYTE)((float)pPixel[3] / 255.0f);
			}
		}
	}
}

void RedrawLayeredWindow()
{
	// The call to UpdateLayeredWindow() is what makes a non-rectangular
	// window possible. To enable per pixel alpha blending we pass in the
	// argument ULW_ALPHA, and provide a BLENDFUNCTION structure filled in
	// to do per pixel alpha blending.

	HDC hdc = GetDC(g_hWnd);

	if (hdc)
	{
		HGDIOBJ hPrevObj = 0;
		POINT ptDest = { 0, 0 };
		POINT ptSrc = { 0, 0 };
		SIZE client = { g_image.width, g_image.height };
		BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

		hPrevObj = SelectObject(g_image.hdc, g_image.hBitmap);
		ClientToScreen(g_hWnd, &ptDest);

		UpdateLayeredWindow(g_hWnd, hdc, &ptDest, &client,
			g_image.hdc, &ptSrc, 0, &blendFunc, ULW_ALPHA);

		SelectObject(g_image.hdc, hPrevObj);
		ReleaseDC(g_hWnd, hdc);
	}
}

BOOL InitRenderToTexture(D3DFORMAT format, D3DFORMAT depthStencil)
{
// 	HRESULT hr = 0;
// 	int width = g_image.width;
// 	int height = g_image.height;
// 
// 	// Create the dynamic render target texture. Since we want use this texture
// 	// with the Win32 layered windows API to create a per pixel alpha blended
// 	// non-rectangular window we *must* use a D3DFORMAT that contains an ALPHA
// 	// channel.
// 	hr = D3DXCreateTexture(g_pd3dDevice, width, height, 0, D3DUSAGE_RENDERTARGET,
// 		format, D3DPOOL_DEFAULT, &g_pTexture);
// 
// 
// 
// 	if (FAILED(hr))
// 		return FALSE;
// 
// 	// Cache the top level surface of the render target texture. This will make
// 	// it easier to bind the dynamic render target texture to the device.
// 	hr = g_pTexture->GetSurfaceLevel(0, &g_pRenderTargetSurface);
// 
// 	if (FAILED(hr))
// 		return FALSE;
// 
// 	// Create a depth-stencil surface so the scene rendered to the dynamic
// 	// texture will be correctly depth sorted.
// 	hr = g_pDevice->CreateDepthStencilSurface(width, height, depthStencil,
// 		D3DMULTISAMPLE_NONE, 0, TRUE, &g_pDepthStencilSurface, 0);
// 
// 	if (FAILED(hr))
// 		return FALSE;
// 
// 	// Create an off-screen plain system memory surface. This system memory
// 	// surface will be used to fetch a copy of the pixel data rendered into the
// 	// render target texture. We can't directly read the render target texture
// 	// because textures created with D3DPOOL_DEFAULT can't be locked.
// 	hr = g_pDevice->CreateOffscreenPlainSurface(width, height,
// 		format, D3DPOOL_SYSTEMMEM, &g_pSurface, 0);

	return TRUE;
}


void redirectIOToConsole(void)//没行啊
{
	AllocConsole();
	// Get the handle for STDOUT's file system.
	HANDLE stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	// Redirect STDOUT to the new console by associating STDOUT's file

	// descriptor with an existing operating-system file handle.
	int hConsoleHandle = _open_osfhandle((intptr_t)stdOutputHandle, _O_TEXT);
	FILE *pFile = _fdopen(hConsoleHandle, "w");
	*stdout = *pFile;
	setvbuf(stdout, NULL, _IONBF, 0);

	// This call ensures that iostream and C run-time library operations occur 
	// in the order that they appear in source code.
	//ios::sync_with_stdio();

}



//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
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





//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int argXPosition, argYPosition, argWight=160, argHeight=320;
FILE *fpOutputTime;

char strPictureName1[255] = { 0 };
char strPictureName1_1[255] = { 0 };
char strPictureName2[255] = { 0 };
char strPictureName2_2[255] = { 0 };
char strPictureName3[255] = { 0 };
char strPictureName3_3[255] = { 0 };
char strPictureName4[255] = { 0 };
char strPictureName4_4[255] = { 0 };
char strPictureName5[255] = { 0 };
char strPictureName5_5[255] = { 0 };
char strPictureName6[255] = { 0 };
char strPictureName6_6[255] = { 0 };

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	//std::wstring strPictureFile;
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
	//redirectIOToConsole();//使用控制台的输出
	cout << sizeof(int) << endl;
	cout << sizeof(int) << endl;

	QueryPerformanceFrequency(&nFreq);//获取前后的时间，精确到微秒


	fpOutputTime = fopen("OutputTime.txt", "a+");
// 	fprintf(fpOutputTime, "%f , %f\n", 9.999, 8.888);//将123456和789写到test.txt文件中	fprintf(fp, "%s %s", "China", "ChongQing"); //将字符串China和ChongQing追加写到test.txt文件中

	int argCount= 0;
	char* argument[6];
	FILE *fp;
	fp = fopen("Config.txt", "rt+");
	int arg1=0, arg2 = 0, arg3 = 0, arg4 = 0, arg5 = 0, arg6=1;

//	fscanf(fp, "%d %d %d %d %d\n", &arg1, &arg2, &arg3, &arg4, &arg5);    ////将123456和789写到test.txt文件中	//fprintf(fp, "%s %s", "China", "ChongQing"); //将字符串China和ChongQing追加写到test.txt文件中
	//fscanf(fp, "%d %d\n", &g_ArgN1, &g_ArgM1);

	fscanf(fp, "%s %s %d %d\n", strPictureName1, strPictureName1_1, &g_ArgN1, &g_ArgM1);
	fscanf(fp, "%s %s %d %d\n", strPictureName2, strPictureName2_2, &g_ArgN2, &g_ArgM2);
	fscanf(fp, "%s %s %d %d\n", strPictureName3, strPictureName3_3, &g_ArgN3, &g_ArgM3);
	fscanf(fp, "%s %s %d %d\n", strPictureName4, strPictureName4_4, &g_ArgN4, &g_ArgM4);
	fscanf(fp, "%s %s %d %d\n", strPictureName5, strPictureName5_5, &g_ArgN5, &g_ArgM5);
	fscanf(fp, "%s %s %d %d\n", strPictureName6, strPictureName6_6, &g_ArgN6, &g_ArgM6);

	fscanf(fp, "%d %d %d %d\n", &argXPosition, &argYPosition, &argWight, &argHeight);//窗口起始位置和宽高（对应视锥体也是这样，等比例缩小   0.05吧

	fclose(fp);

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

	if (!ImageCreate(&g_image, IMAGE_WIDTH, IMAGE_HEIGHT))
		return FALSE;//创建  图像 buff  -------------------------------------------------------------------------------------------------------------

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render(arg1, arg2, arg3, arg4, arg5, arg6);
        }
    }

    CleanupDevice();
	fclose(fpOutputTime);
    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDM_ABOUT);
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDM_ABOUT);
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 160, 320 };//480
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"Tutorial07", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

// ShowWindow( g_hWnd, nCmdShow );WM_MOUSEACTIVATE
	SetWindowLong(g_hWnd, GWL_STYLE, WS_EX_TRANSPARENT | WS_BORDER );//WS_EX_NOACTIVATE 会导致不能点击关闭来关闭这个窗口// WS_BORDER是无边框| WS_EX_NOACTIVATE
	SetWindowLong(g_hWnd, GWL_EXSTYLE, GetWindowLongPtr(g_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);//这个WS_EX_LAYERED导致透明成功WS_EX_LAYERED

	ShowWindow(g_hWnd, nCmdShow);
	SetWindowPos(g_hWnd, HWND_TOPMOST, argXPosition, argYPosition, argWight, argHeight, SWP_NOACTIVATE);//置顶  不激活窗口
	SetLayeredWindowAttributes(g_hWnd, 0, 250, LWA_ALPHA);//0.33 的ALPHA

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }

HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;//  Desc的width 和height 都应该由  窗口的大小提供 ，窗口的大小是读取 config.txt得到的
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;//  Desc的width 和height 都应该由  窗口的大小提供
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

//	int width = g_image.width;
//	int height = g_image.height;//liuchang
	if (g_image.width==width)//  Desc的width 和height 都应该由  窗口的大小提供
	{
		HRESULT hr = S_OK;
	}

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
	//g_pRenderTargetView->GetPrivateData();
//	g_pRenderTargetView->GetResource(&g_Texture2D_RTT);
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

	// Create the offscreen particle buffer
	D3D11_TEXTURE2D_DESC Desc;//-----------------------------------------------------------搞一块离屏渲染的buff而已------------CreateTexture2D-----------------//---------------------------------------------------------
	Desc.Width = g_image.width;// pBackBufferSurfaceDesc->Width;//  Desc的width 和height 都应该由  窗口的大小提供
	Desc.Height = g_image.height ;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;

//	(g_pd3dDevice->CreateTexture2D(&Desc, NULL, &g_Texture2D_RTT));
	Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	(g_pd3dDevice->CreateTexture2D(&Desc, NULL, &g_Texture2D_RTT));//-------------------------CreateTexture2D--------------------------------------------------------
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	//RTVDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	//V_RETURN(pd3dDevice->CreateRenderTargetView(g_pOffscreenParticleTex, &RTVDesc, &g_pOffscreenParticleRTV));
	RTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	(g_pd3dDevice->CreateRenderTargetView(g_Texture2D_RTT, &RTVDesc,	&g_pOffscreenParticleColorRTV));//--------------搞一块离屏渲染的buff而已----------------CreateTexture2D-----CreateRenderTargetView-----------------------------------------------

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;
	// We need a screen-sized STAGING resource for frame capturing    //liuchang-------------------------------------------------
	D3D11_TEXTURE2D_DESC TexDesc;
	DXGI_SAMPLE_DESC SingleSample = { 1, 0 };
	TexDesc.Width = width;
	TexDesc.Height = height;
	TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		//pBackBufferSurfaceDesc->Format
	TexDesc.SampleDesc = SingleSample;
	TexDesc.MipLevels = 1;
	TexDesc.Usage = D3D11_USAGE_STAGING;
	TexDesc.MiscFlags = 0;
	TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	TexDesc.BindFlags = 0;
	TexDesc.ArraySize = 1;
	(g_pd3dDevice->CreateTexture2D(&TexDesc, NULL, &g_pD3DTexture2D_Capture));

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    // Compile the vertex shader
    ID3DBlob* pVSBlob = NULL;
    hr = CompileShaderFromFile( L"Tutorial07.fx", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the vertex shader
    hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader );
    if( FAILED( hr ) )
    {    
        pVSBlob->Release();
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
    hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
    pVSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout( g_pVertexLayout );

    // Compile the pixel shader
    ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile( L"Tutorial07.fx", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader );
    pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    // Create vertex buffer
	float  DelatPos = 1.0f;
    SimpleVertex vertices[] =
    {


// 		{ XMFLOAT3(-1.0f - 3 * DelatPos, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
// 		{ XMFLOAT3(1.0f -3 * DelatPos, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
// 		{ XMFLOAT3(1.0f - 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
// 		{ XMFLOAT3(-1.0f - 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },			//后面 x方向偏移 3 //这里的正的纹理     横着的纹理 

// 		{ XMFLOAT3(-1.0f - 3 * DelatPos, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
// 		{ XMFLOAT3(1.0f - 3 * DelatPos, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
// 		{ XMFLOAT3(1.0f - 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
// 		{ XMFLOAT3(-1.0f - 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },			//后面 x方向偏移 3//这里的       正的纹理

        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },			//后面 //这里的    正的纹理

// 		{ XMFLOAT3(-1.0f+3* DelatPos, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
// 		{ XMFLOAT3(1.0f + 3 * DelatPos, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
// 		{ XMFLOAT3(1.0f + 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
// 		{ XMFLOAT3(-1.0f + 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },			//后面 x方向偏移 3 、、这里的      反的纹理
    };

    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 4* g_NumberQuat;//不应该还是 4*6
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set vertex buffer
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    // Create index buffer
    // Create vertex buffer
    WORD indices[] =
    {
        3,1,0,
        2,1,3

// 		7,5,4,
// 		6,5,7,
// 
// 		11,9,8,
// 		10,9,11
    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( WORD ) * g_NumberQuat *6;//index  的 数目
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
    if( FAILED( hr ) )
        return hr;

    // Set index buffer
    g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//D3D11_PRIMITIVE_TOPOLOGY_LINELIST//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST

    // Create the constant buffers
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CBNeverChanges);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBNeverChanges );
    if( FAILED( hr ) )
        return hr;
    
    bd.ByteWidth = sizeof(CBChangeOnResize);
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBChangeOnResize );
    if( FAILED( hr ) )
        return hr;
    
    bd.ByteWidth = sizeof(CBChangesEveryFrame);
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBChangesEveryFrame );
    if( FAILED( hr ) )
        return hr;

    // Load the Texture			//目前支持  png图 dds图
    hr = D3DX11CreateShaderResourceViewFromFileA( g_pd3dDevice, strPictureName1, NULL, NULL, &g_pTextureRV, NULL );
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName1_1, NULL, NULL, &g_pTextureRV1_1, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName2, NULL, NULL, &g_pTextureRV2, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName2_2, NULL, NULL, &g_pTextureRV2_2, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName3, NULL, NULL, &g_pTextureRV3, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName3_3, NULL, NULL, &g_pTextureRV3_3, NULL);

	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName4, NULL, NULL, &g_pTextureRV4, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName4_4, NULL, NULL, &g_pTextureRV4_4, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName5, NULL, NULL, &g_pTextureRV5, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName5_5, NULL, NULL, &g_pTextureRV5_5, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName6, NULL, NULL, &g_pTextureRV6, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName6_6, NULL, NULL, &g_pTextureRV6_6, NULL);

	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName6, NULL, NULL, &g_pTextureRV13, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName6, NULL, NULL, &g_pTextureRV14, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName6, NULL, NULL, &g_pTextureRV15, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName6, NULL, NULL, &g_pTextureRV16, NULL);
	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName6, NULL, NULL, &g_pTextureRV17, NULL);

    if( FAILED( hr ) )
        return hr;

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState( &sampDesc, &g_pSamplerLinear );
    if( FAILED( hr ) )
        return hr;

    // Initialize the world matrices
    g_World = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet( 0.0f, 0.0f, -10.0f, 0.0f );
    XMVECTOR At = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    g_View = XMMatrixLookAtLH( Eye, At, Up );

    CBNeverChanges cbNeverChanges;
    cbNeverChanges.mView = XMMatrixTranspose( g_View );
    g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0 );

    // Initialize the projection matrix
    g_Projection = XMMatrixPerspectiveFovLH( XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f );
	float Orthowidth = argWight;
	float Orthoheight = argHeight; 
	g_Oth=XMMatrixOrthographicLH(12, 12 * (Orthoheight / Orthowidth),  0.1f, 100.0f);//正 投影了、、width, height   10 20  是正视锥体的宽高  8/160=1/20  //目前是以高度位置显示5个 正方形
    
    CBChangeOnResize cbChangesOnResize;
    cbChangesOnResize.mProjection = XMMatrixTranspose(g_Oth);//g_Projection  by liuchang
    g_pImmediateContext->UpdateSubresource( g_pCBChangeOnResize, 0, NULL, &cbChangesOnResize, 0, 0 );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    if( g_pSamplerLinear ) g_pSamplerLinear->Release();
    if( g_pTextureRV ) g_pTextureRV->Release();
	if (g_pTextureRV1_1) g_pTextureRV1_1->Release();
	if (g_pTextureRV2) g_pTextureRV2->Release();
	if (g_pTextureRV2_2) g_pTextureRV2_2->Release();
	if (g_pTextureRV3) g_pTextureRV3->Release();
	if (g_pTextureRV3_3) g_pTextureRV3_3->Release();
	if (g_pTextureRV4) g_pTextureRV4->Release();
	if (g_pTextureRV4_4) g_pTextureRV4_4->Release();
	if (g_pTextureRV5) g_pTextureRV5->Release();
	if (g_pTextureRV5_5) g_pTextureRV5_5->Release();
	if (g_pTextureRV6) g_pTextureRV6->Release();
	if (g_pTextureRV6_6) g_pTextureRV6_6->Release();

    if( g_pCBNeverChanges ) g_pCBNeverChanges->Release();
    if( g_pCBChangeOnResize ) g_pCBChangeOnResize->Release();
    if( g_pCBChangesEveryFrame ) g_pCBChangesEveryFrame->Release();
    if( g_pVertexBuffer ) g_pVertexBuffer->Release();
    if( g_pIndexBuffer ) g_pIndexBuffer->Release();
    if( g_pVertexLayout ) g_pVertexLayout->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
    if( g_pDepthStencil ) g_pDepthStencil->Release();
    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
		case WM_KEYDOWN:
				switch (wParam)
			   {
					case VK_ESCAPE://按下Esc键
					 DestroyWindow(hWnd);
					 PostQuitMessage(0);
					 break;
				}
			SetCursor(LoadCursor(NULL, IDC_WAIT));//设置新的鼠标光标
			ShowCursor(TRUE);//显示鼠标光标
		break;

		case WM_LBUTTONDOWN://MA_NOACTIVATE 
			SetCursor(LoadCursor(NULL, IDC_UPARROW));//设置新的鼠标光标
			ShowCursor(TRUE);//显示鼠标光标
			return DefWindowProc(hWnd, message, MA_NOACTIVATE, lParam);//已经在钩子里拦截了，这里不会来的
			break;
		case WM_RBUTTONDOWN://MA_NOACTIVATE 
			SetCursor(LoadCursor(NULL, IDC_UPARROW));//设置新的鼠标光标
			ShowCursor(TRUE);//显示鼠标光标
			break;

        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

		case WM_MOUSEACTIVATE:
			return MA_NOACTIVATE;
			break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}



//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
int g_test_SetRV = 0;

HRESULT  getBUff()
{
	HRESULT  hr;
	//g_pd3dDevice->SetDepthStencilSurface(g_pDepthStencilSurface);
	//g_pd3dDevice->SetRenderTarget(0, g_pRenderTargetSurface);
	ID3D11Resource *pRTResource;//----	// Retrieve RT resource---------------------------------------------------------------------------------------
	g_pRenderTargetView->GetResource(&pRTResource);
	// Retrieve a Texture2D interface from resource
	ID3D11Texture2D* RTTexture;
	pRTResource->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&RTTexture);

	// Check if RT is multi sampled or not
	D3D11_TEXTURE2D_DESC    TexDesc;
	RTTexture->GetDesc(&TexDesc);
	if (TexDesc.SampleDesc.Count > 1)
	{
		// RT is multi sampled, need resolving before dumping to disk
		// Create single-sample RT of the same type and dimensions
		DXGI_SAMPLE_DESC SingleSample = { 1, 0 };
		TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		TexDesc.MipLevels = 1;
		TexDesc.Usage = D3D11_USAGE_DEFAULT;
		TexDesc.CPUAccessFlags = 0;
		TexDesc.BindFlags = 0;
		TexDesc.SampleDesc = SingleSample;

		ID3D11Texture2D *pSingleSampleTexture;
		g_pd3dDevice->CreateTexture2D(&TexDesc, NULL, &pSingleSampleTexture);
		//DXUT_SetDebugName(pSingleSampleTexture, "Single Sample");

		g_pImmediateContext->ResolveSubresource(pSingleSampleTexture, 0, RTTexture, 0, TexDesc.Format);

		// Copy RT into STAGING texture
		g_pImmediateContext->CopyResource(g_pD3DTexture2D_Capture, pSingleSampleTexture);//保存用的 
	 //D3DX11SaveTextureToFile(g_pImmediateContext, g_pD3DTexture2D_Capture, D3DX11_IFF_BMP, _T("saveFile.png"));//保存用的 

		SAFE_RELEASE(pSingleSampleTexture);

	}
	else		// Single sample case
	{
	/* hr=*/	g_pImmediateContext->CopyResource(g_pD3DTexture2D_Capture, pRTResource);//保存用的 //第二次蹦了
// 	 if (FAILED(hr))
// 		 return hr;
	//	D3DX11SaveTextureToFileA(g_pImmediateContext, g_pD3DTexture2D_Capture, D3DX11_IFF_PNG, "save_D3DTexture2D_Capture.png");//保存用的 //D3DX11_IFF_PNG //D3DX11_IFF_BMP
		char* pData;
		BYTE *pDest = g_image.pPixels;
		UINT  Size = 0;
	//	d3d11DevCon->CopyResource(textureBuf, texture);
		D3D11_MAPPED_SUBRESOURCE  mapResource;
		hr = g_pImmediateContext->Map(g_pD3DTexture2D_Capture, 0, D3D11_MAP_READ, NULL, &mapResource);

		struct Color { float r, g, b, a; };
		//Color* obj;
		//obj = new Color[(mapResource.RowPitch / sizeof(Color)) *TexDesc.Height ];
	//	memcpy(pDest, mapResource.pData, mapResource.RowPitch *TexDesc.Height );//   rgb 3  和  rgba  4  不能直接copy
	//const BYTE *pSrc = (const BYTE *)rcLock.pBits;		//int srcPitch = rcLock.Pitch;
		const BYTE *pSrc = (const BYTE *)mapResource.pData;
		int  srcPitch = mapResource.RowPitch;
		int destPitch = g_image.pitch;

		if (srcPitch == destPitch)
		{
			memcpy(pDest, pSrc, destPitch * g_image.height);
		}
		else
		{
			for (int i = 0; i < g_image.height; ++i)
				memcpy(&pDest[destPitch * i], &pSrc[srcPitch * i], destPitch);
		}


		g_pImmediateContext->Unmap(g_pD3DTexture2D_Capture, 0);
		//g_pD3DTexture2D_Capture->Release();
	//	delete[] obj;F

		ImagePreMultAlpha(&g_image);
		RedrawLayeredWindow();//像素终于输出到 gdi了



	}
	SAFE_RELEASE(RTTexture);
	SAFE_RELEASE(pRTResource);//------------------------------------------------------------------------------------------------------------------------------
	return hr;
}

// void Pick(ID3D11Texture2D* texture)
// {
// 	ID3D11Texture2D* textureBuf;
// 	D3D11_TEXTURE2D_DESC textureDesc;
// 	ZeroMemory(&textureDesc, sizeof(textureDesc));
// 	textureDesc.Width = ;
// 	textureDesc.Height = Height;
// 	textureDesc.MipLevels = 1;
// 	textureDesc.ArraySize = 1;
// 	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
// 	textureDesc.SampleDesc.Count = 1;
// 	textureDesc.SampleDesc.Quality = 0;
// 	textureDesc.Usage = D3D11_USAGE_STAGING;
// 	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
// 	textureDesc.MiscFlags = 0;
// 	d3d11Device->CreateTexture2D(&textureDesc, NULL, &textureBuf);
// 
// 	d3d11DevCon->CopyResource(textureBuf, texture);
// 	D3D11_MAPPED_SUBRESOURCE  mapResource;
// 	hr = d3d11DevCon->Map(textureBuf, 0, D3D11_MAP_READ, NULL, &mapResource);
// 
// 	struct Color { float r, g, b, a; };
// 	Color* obj;
// 	obj = new Color[(mapResource.RowPitch / sizeof(Color))*Height];
// 	memcpy(obj, mapResource.pData, mapResource.RowPitch*Height);
// 
// 
// 
// 	d3d11DevCon->Unmap(textureBuf, 0);
// 	textureBuf->Release();
// 	delete[] obj;
// }

void Render(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
{
    // Update our time
    static float t = 0.0f;
    if( g_driverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        t += ( float )XM_PI * 0.0125f;
		
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();
        if( dwTimeStart == 0 )
            dwTimeStart = dwTimeCur;
        t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    }

    // Rotate cube around the origin
   // g_World = XMMatrixRotationY( t );//liuchang by   不旋转了
	g_World = XMMatrixIdentity();


    //
    // Clear the back buffer
    //
    float ClearColor[4] = { 1.0f, 0.0f, 0.0f, 0.0f }; // red, green, blue, alpha { 0.0f, 0.125f, 0.3f, 1.0f }; //最后的alpha值好像没用
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    //
    // Update variables that change once per frame
  


    g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );    // Render the 第一个 
    g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
    g_pImmediateContext->VSSetConstantBuffers( 1, 1, &g_pCBChangeOnResize );
    g_pImmediateContext->VSSetConstantBuffers( 2, 1, &g_pCBChangesEveryFrame );

    g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );
    g_pImmediateContext->PSSetConstantBuffers( 2, 1, &g_pCBChangesEveryFrame );

	g_test_SetRV++;
	if (g_test_SetRV>100)
	{
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV4_4);//这个函数是位于 render函数，也就是可以每时每刻都执行一次set sample的纹理值 ，这样我第一张图 每帧设置 100张图片里的某一张都没事
		if (g_test_SetRV>200)
		{
			g_test_SetRV = 0;
		}
	} 
	else
	{
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	}
//    g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pTextureRV );
	g_pImmediateContext->PSSetShaderResources(1, 1, &g_pTextureRV1_1);//liu//这个函数是位于 render函数，也就是可以每时每刻都执行
	g_pImmediateContext->PSSetShaderResources(2, 1, &g_pTextureRV2);
	g_pImmediateContext->PSSetShaderResources(3, 1, &g_pTextureRV2_2);//liu
	g_pImmediateContext->PSSetShaderResources(4, 1, &g_pTextureRV3);//liu
	g_pImmediateContext->PSSetShaderResources(5, 1, &g_pTextureRV3_3);
	g_pImmediateContext->PSSetShaderResources(6, 1, &g_pTextureRV4);//liu
	g_pImmediateContext->PSSetShaderResources(7, 1, &g_pTextureRV4_4);//liu
	g_pImmediateContext->PSSetShaderResources(8, 1, &g_pTextureRV5);
	g_pImmediateContext->PSSetShaderResources(9, 1, &g_pTextureRV5_5);//liu
	g_pImmediateContext->PSSetShaderResources(10, 1, &g_pTextureRV6);//liu
	g_pImmediateContext->PSSetShaderResources(11, 1, &g_pTextureRV6_6);

	g_pImmediateContext->PSSetShaderResources(12, 1, &g_pTextureRV13);
	g_pImmediateContext->PSSetShaderResources(13, 1, &g_pTextureRV14);// test
	g_pImmediateContext->PSSetShaderResources(14, 1, &g_pTextureRV15);//test
	g_pImmediateContext->PSSetShaderResources(15, 1, &g_pTextureRV16);
	g_pImmediateContext->PSSetShaderResources(16, 1, &g_pTextureRV17);

    g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerLinear );//0,1  1,1   2,1  这些玩意第一个数表示register里的数字，“register( b2 )”就写2

// 	CBChangesEveryFrame cb;//											绘制固定的quat
// 	cb.mWorld = XMMatrixTranspose(g_World);
// 	cb.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
// 	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
//     g_pImmediateContext->DrawIndexed( 6* g_NumberQuat, 0, 0 );//开始是36

				
		// Update variables that change once per frame

		g_ArgNM1++; //一秒会  ++ 60次	//绘制第一个正方形
		if (g_ArgNM1>g_ArgN1)
		{
				XMFLOAT4 movePos(-4.0f, 2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位
				g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
				CBChangesEveryFrame cb;
				cb.mWorld = XMMatrixTranspose(g_World2);
				cb.vMeshColor = XMFLOAT4(0.15f, 1.0f, 1.0f, 1.0f);			//为了 图A  的显示
				g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
				g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);			//可以执行绘制的
				if (g_ArgNM1 == (g_ArgN1 + g_ArgM1))
				{
					g_ArgNM1 = 0;
				}
		}
		else
		{
			XMFLOAT4 movePos(-4.0f, 2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位
			g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
			CBChangesEveryFrame cb;
			cb.mWorld = XMMatrixTranspose(g_World2);
			cb.vMeshColor = XMFLOAT4(0.1f, 1.0f, 1.0f, 1.0f);		//为了 图a  的显示
			g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
			g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);
		}

		g_ArgNM2++;
		if (g_ArgNM2 > g_ArgN2) 
		{
				XMFLOAT4 movePos(0.0f, 2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位  //绘制第2 个正方形
				g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
				CBChangesEveryFrame cb;
				cb.mWorld = XMMatrixTranspose(g_World2);
				cb.vMeshColor = XMFLOAT4(0.25f, 0.0f, 0.5f, 1.0f);//B
				g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
				g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);
				if (g_ArgNM2 == (g_ArgN2 + g_ArgM2))
				{
					g_ArgNM2 = 0;
				}
		}
		else
		{
			XMFLOAT4 movePos(0.0f, 2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位  //绘制第2 个正方形
			g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
			CBChangesEveryFrame cb;
			cb.mWorld = XMMatrixTranspose(g_World2);
			cb.vMeshColor = XMFLOAT4(0.2f, 0.0f, 0.0f, 1.0f);//b
			g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
			g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);
		}

		g_ArgNM3++;
		if (g_ArgNM3 > g_ArgN3) //一秒钟进来1次， 也就是60帧里进来1次			//60
		{
				XMFLOAT4 movePos(4.0f, 2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位
				g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
				CBChangesEveryFrame cb;
				cb.mWorld = XMMatrixTranspose(g_World2);
				cb.vMeshColor = XMFLOAT4(0.35f, 1.0f, 0.0f, 1.0f);//C
				g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
				g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);
				if (g_ArgNM3 == (g_ArgN3 + g_ArgM3))
				{
					g_ArgNM3 = 0;
				}
		}
		else
		{
			XMFLOAT4 movePos(4.0f, 2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位
			g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
			CBChangesEveryFrame cb;
			cb.mWorld = XMMatrixTranspose(g_World2);
			cb.vMeshColor = XMFLOAT4(0.3f, 1.0f, 0.0f, 1.0f);//c
			g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
			g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);
		}

		g_ArgNM4++;
		if (g_ArgNM4 > g_ArgN4) //一秒钟进来1次， 也就是60帧里进来1次			//60
		{
				XMFLOAT4 movePos(-4.0f, -2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位
				g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
				CBChangesEveryFrame cb;
				cb.mWorld = XMMatrixTranspose(g_World2);
				cb.vMeshColor = XMFLOAT4(0.45f, 1.0f, 0.0f, 1.0f);//D
				g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
				g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);
				if (g_ArgNM4 == (g_ArgN4 + g_ArgM4))
				{
					g_ArgNM4 = 0;
				}
		}
		else
		{
			XMFLOAT4 movePos(-4.0f, -2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位
			g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
			CBChangesEveryFrame cb;
			cb.mWorld = XMMatrixTranspose(g_World2);
			cb.vMeshColor = XMFLOAT4(0.4f, 1.0f, 0.0f, 1.0f);//d
			g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
			g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);
		}

		g_ArgNM5++;
		if (g_ArgNM5 > g_ArgN5) 
		{
				XMFLOAT4 movePos(0.0f, -2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位
				g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
				CBChangesEveryFrame cb;
				cb.mWorld = XMMatrixTranspose(g_World2);
				cb.vMeshColor = XMFLOAT4(0.55f, 1.0f, 0.0f, 1.0f);//E
				g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
				g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);
				if (g_ArgNM5 == (g_ArgN5 + g_ArgM5))
				{
					g_ArgNM5 = 0;
				}
		}
		else
		{
			XMFLOAT4 movePos(0.0f, -2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位
			g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
			CBChangesEveryFrame cb;
			cb.mWorld = XMMatrixTranspose(g_World2);
			cb.vMeshColor = XMFLOAT4(0.5f, 1.0f, 0.0f, 1.0f);//e
			g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
			g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);
		}

		g_ArgNM6++;
		if (g_ArgNM6 > g_ArgN6)
		{
			XMFLOAT4 movePos(4.0f, -2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位
			g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
			CBChangesEveryFrame cb;
			cb.mWorld = XMMatrixTranspose(g_World2);
			cb.vMeshColor = XMFLOAT4(0.65f, 1.0f, 0.0f, 1.0f);//E
			g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
			g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);
			if (g_ArgNM6 == (g_ArgN6 + g_ArgM6))
			{
				g_ArgNM6 = 0;
			}
		}
		else
		{
			XMFLOAT4 movePos(4.0f, -2.0f, 0.0f, 1.0f);//移动y方向上下移几个单位
			g_World2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
			CBChangesEveryFrame cb;
			cb.mWorld = XMMatrixTranspose(g_World2);
			cb.vMeshColor = XMFLOAT4(0.6f, 1.0f, 0.0f, 1.0f);//e
			g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
			g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);
		}




   // g_pSwapChain->Present( 1, 0 );//  1  ： 开启垂直同步 （fps大约60）
	getBUff();//获取buff准备输出到 hdc		
	//QueryPerformanceCounter(&nEndTime);
	//time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;//这是微秒精度的 时间段的时间

	QueryPerformanceCounter(&nBeginTime);
	static		double  TimeLast = 0.11;// (double)(nBeginTime.QuadPart);
	double  TimeCurrent = (double)(nBeginTime.QuadPart);
	double deltaTime =( TimeCurrent - TimeLast) / (double)nFreq.QuadPart ;
	TimeLast = TimeCurrent;

	static double static_DeltaTime = 0;
	static_DeltaTime += deltaTime;//deltaTime约等于 0.0166
	//统计 fps-----------------------------------------------
	g_frames++;
	if (static_DeltaTime>1.0f)
	{
		static_DeltaTime -= 1.0f;
		if (static_DeltaTime>100.0f)
		{
			static_DeltaTime = 0;
		}
		char szBuff[255] = { 0 };
		sprintf(szBuff, "%d FPS", g_frames);
		SetWindowTextA(g_hWnd,szBuff);
		g_frames = 0;
	}//统计 fps-----------------------------------------------

	static bool isFirst = true;
	if (isFirst)
	{
		isFirst = false;
		return;
	}
	fprintf(fpOutputTime, "%f\n", deltaTime);//输出 【两帧】之间的时间间隔 到 文件 file
//	g_pd3dDevice->GetRenderTargetData(g_pRenderTargetSurface, g_pSurface);





}
