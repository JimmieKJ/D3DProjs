// Win32Window.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32WindowDll.h"
//#include "FlashPicture.h"
#include <process.h>  
#include <assert.h>  
#include <iostream>
#include <map>
#include <vector>
#include <list>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <d3dx9.h>
#include <xnamath.h>


#include <assert.h>  

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


//int g_NumberVertex = 500;
#define  g_NumberVertex 100

ID3D11SamplerState*                 g_pSamplerLinear = NULL;
ID3D11RasterizerState * g_pRasterState = NULL;
int mCount_SetRV = 0;
IDirect3DSurface9 *g_pRenderTargetSurface;
IDirect3DSurface9 *g_pDepthStencilSurface;
D3DXMATRIX g_worldMtx;
HINSTANCE  mhInstance;
HINSTANCE                           g_hInst;
HWND                                mhWnd;
D3D_DRIVER_TYPE                     g_driverType;
D3D_FEATURE_LEVEL                   g_featureLevel;
ID3D11Device*                       g_pd3dDevice;
ID3D11DeviceContext*                g_pImmediateContext;
ID3D11DeviceContext*                g_pImmediateContext2;
IDXGISwapChain*                     g_pSwapChain;
ID3D11RenderTargetView*             g_pRenderTargetView;
ID3D11Texture2D*                    g_pDepthStencil;

ID3D11Texture2D*                    g_Texture2D_RTT;

ID3D11DepthStencilView*             g_pDepthStencilView;
ID3D11VertexShader*                 g_pVertexShader;
ID3D11PixelShader*                  g_pPixelShader;
ID3D11InputLayout*                  g_pVertexLayout;
ID3D11Buffer*                       g_pVertexBuffer;
ID3D11Buffer*                       g_pIndexBuffer;
ID3D11Buffer*                       g_pCBNeverChanges;
ID3D11Buffer*                       g_pCBChangeOnResize;
ID3D11Buffer*                       g_pCBChangesEveryFrame;
ID3D11Buffer*                       g_pCBChangesEveryFrameUpdateVertexData;
ID3D10EffectShaderResourceVariable* g_pDepthMSAATex = NULL;

ID3D10Effect*                       g_pEffect10 = NULL;




// ID3D10Texture2D*                    g_pColorTexture = NULL;
// ID3D10RenderTargetView*             g_pColorRTView = NULL;
// ID3D10ShaderResourceView*           g_pColorSRView = NULL;
// 
// ID3D10Texture2D*                    g_pDepthStencilTexture = NULL;
// ID3D10DepthStencilView*             g_pDepthStencilDSView = NULL;
// ID3D10ShaderResourceView*           g_pDepthStencilSRView = NULL;

ID3D10Texture2D*                    g_pMSAAColorTexture = NULL;
ID3D10RenderTargetView*             g_pMSAAColorRTView = NULL;
ID3D10ShaderResourceView*           g_pMSAAColorSRView = NULL;



typedef std::vector<	ID3D11ShaderResourceView* >  VecShaderResourceView;			//ID3D11ShaderResourceView*           g_pTextureRV = NULL;
VecShaderResourceView mVecTextureResourceView;//所有的需求都是这里的


struct String_Int
{
public:
	String_Int(std::string str, int num) :strPictureName(str)
		, NumberFrame(num)
	{
	}
	std::string strPictureName;
	int NumberFrame;
};
typedef std::vector<String_Int> VecStringInt;
VecStringInt mVec_string_int;//所有的需求都在这里  //让后再render里更新这里的东西作为真是的使用起来
int g_TotalFrameNumber;

ID3D11ShaderResourceView*           g_pTextureRV17;

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
struct cbChangesEveryFrameVertexData
{
	XMFLOAT4  mVertex_YCoordinatesData[g_NumberVertex];
};

volatile bool gbExitThread;



HBITMAP bmp;
FILE *fpOutputThreadID;


HANDLE  g_HEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE  g_HEventQuit = CreateEvent(NULL, FALSE, FALSE, NULL);
bool g_bSingleInstance = true;
bool g_isERp = false;


int g_windowsNumber = 0;

int  loopCount = 6;//erp循环的第几个开始
int  g_loopCountLeft = 6;//erp循环的第几个开始

						 //------------------------------------------------------------------------------------------------------------------------------
						 // Helper for compiling shaders with D3DX11
						 //--------------------------------------------------------------------------------------
// HRESULT  CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
// {
// // 	hr = DXUTFindDXSDKMediaFileCch(
// // 		str,
// // 		MAX_PATH,
// // 		L"DepthOfField10.1.fx");
// 
// 
// 
// 	//
// 	//  Check to see if we are on SP1 and have a dx10.1 device.
// 	//
// 	LPCSTR                      pszTarget;
// 		pszTarget = "fx_4_1";
// 
// 
// 		pszTarget = "fx_4_1";
// 
// 
// 	ID3D10Blob* pErrorBlob;
// 	HRESULT                     hr;
// 	hr = D3DX11effe(
// 		str,
// 		NULL,
// 		NULL,
// 		pszTarget,
// 		dwShaderFlags,
// 		0,
// 		pd3dDevice,
// 		NULL,
// 		NULL,
// 		&g_pEffect10,
// 		&pErrorBlob,
// 		NULL);
// 	//////////////////////////////////////////////////////////////////
// 
// 	HRESULT hr = S_OK;
// 
// 	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
// #if defined( DEBUG ) || defined( _DEBUG )
// 	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
// 	// Setting this flag improves the shader debugging experience, but still allows 
// 	// the shaders to be optimized and to run exactly the way they will run in 
// 	// the release configuration of this program.
// 	dwShaderFlags |= D3DCOMPILE_DEBUG;
// #endif
// 
// 	ID3DBlob* pErrorBlob;
// 	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
// 		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
// 	if (FAILED(hr))
// 	{
// 		if (pErrorBlob != NULL)
// 			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
// 		if (pErrorBlob) pErrorBlob->Release();
// 		return hr;
// 	}
// 	if (pErrorBlob) pErrorBlob->Release();
// 
// 	return S_OK;
// }
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=NULL; } }
HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	// find the file
	WCHAR str[MAX_PATH];
	//V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, szFileName));

	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		SAFE_RELEASE(pErrorBlob);
		return hr;
	}
	SAFE_RELEASE(pErrorBlob);

	return S_OK;
}

void  Render(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
{
	// Update our time
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 1000.0f;
	}

	// Rotate cube around the origin
	// g_World = XMMatrixRotationY( t );//liuchang by   不旋转了
	g_World = XMMatrixIdentity();

	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.3f, 0.33f, 0.3f, 1.0f }; // red, green, blue, alpha { 0.0f, 0.125f, 0.3f, 1.0f }; //最后的alpha值好像没用
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	//
	// Clear the depth buffer to 1.0 (max depth)
	//
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Update variables that change once per frame
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);    // Render the 第一个 
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBNeverChanges);
	g_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pCBChangeOnResize);
	g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);
	g_pImmediateContext->VSSetConstantBuffers(3, 1, &g_pCBChangesEveryFrameUpdateVertexData);

	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);



	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);//0,1  1,1   2,1  这些玩意第一个数表示register里的数字，“register( b2 )”就写2

																// Update variables that change once per frame
	XMFLOAT4 movePos(0.0f, 0.0f, 0.0f, 1.0f);//移动y方向上下移几个单位 /绘制正方形   正方形的宽高都是1个单位     （-1 1 ， 11  ，-1-1，  1 -1）
	XMMATRIX	MatrixWorld2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
	CBChangesEveryFrame cb;
	cb.mWorld = XMMatrixTranspose(MatrixWorld2);
	cb.vMeshColor = XMFLOAT4(0.15f, 1.0f, 1.0f, 1.0f);			//为了 图A  的显示
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
	cbChangesEveryFrameVertexData cbVertexData;
	XMFLOAT4 * floatdate = new XMFLOAT4[g_NumberVertex];
	for (int ff=0;ff<g_NumberVertex;ff++)
	{
		int Nff = (ff*2);
		if ((ff%3)==0)
		{
			Nff =-Nff;
		}
	//	int Nff = (-ff);
		floatdate[ff] = XMFLOAT4(Nff,0,0,0) ;
	}
	memcpy(cbVertexData.mVertex_YCoordinatesData, floatdate, sizeof(XMFLOAT4) * g_NumberVertex); 	//cbVertexData.mVertex_YCoordinatesData = floatdate;
	delete floatdate;

	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrameUpdateVertexData, 0, NULL, &cbVertexData, 0, 0);//更新 Vertex  YCoordinates Data数据
	g_pImmediateContext->DrawIndexed(g_NumberVertex, 0, 0);			//执行绘制的




	g_pSwapChain->Present(1, 0);//  1  ： 开启垂直同步 （fps大约60）

}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT  InitDevice()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(mhWnd, &rc);
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
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;//  Desc的width 和height 都应该由  窗口的大小提供
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//原先的DXGI_FORMAT_R8G8B8A8_UNORM//
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// sd.BufferUsage = DXGI_CPU_ACCESS_READ_WRITE;//DXGI_CPU_ACCESS_READ_WRITE
	sd.OutputWindow = mhWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;



	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
		//g_pd3dDevice->Create//CreateOffscreenPlainSurface

	}
	if (FAILED(hr))
		return hr;



	///test 光栅

	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerState.CullMode = D3D11_CULL_NONE;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = true;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;
	//rasterizerState.ForcedSampleCount = 0;
	g_pd3dDevice->CreateRasterizerState(&rasterizerState, &g_pRasterState);


	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;



	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	//g_pRenderTargetView->GetPrivateData();
	//	g_pRenderTargetView->GetResource(&g_Texture2D_RTT);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;



	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
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
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;


	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(L"3DBreakLine.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
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
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"3DBreakLine.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Create vertex buffer
	float  DelatPos = 1.0f;

	SimpleVertex* pVectexArray = new SimpleVertex[g_NumberVertex];
	for (int  Set_N=0;Set_N<g_NumberVertex;Set_N++)
	{
		if (Set_N%2==0)
		{
			pVectexArray[Set_N].Pos = XMFLOAT3(Set_N, Set_N, 1.0f);
			pVectexArray[Set_N].Tex = XMFLOAT2(0.0f, 1.0f);
		}
		else
		{
			pVectexArray[Set_N].Pos = XMFLOAT3(Set_N, -1.0f*Set_N, 1.0f);
			pVectexArray[Set_N].Tex = XMFLOAT2(0.0f, 1.0f);
		}
	}
// 	SimpleVertex vertices[] =
// 	{
// 		// 		{ XMFLOAT3(-1.0f - 3 * DelatPos, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
// 		// 		{ XMFLOAT3(1.0f -3 * DelatPos, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
// 		// 		{ XMFLOAT3(1.0f - 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
// 		// 		{ XMFLOAT3(-1.0f - 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },			//后面 x方向偏移 3 //这里的正的纹理     横着的纹理 
// 
// 		// 		{ XMFLOAT3(-1.0f - 3 * DelatPos, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
// 		// 		{ XMFLOAT3(1.0f - 3 * DelatPos, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
// 		// 		{ XMFLOAT3(1.0f - 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
// 		// 		{ XMFLOAT3(-1.0f - 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },			//后面 x方向偏移 3//这里的       正的纹理
// 
// 		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
// 		{ XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
// 		{ XMFLOAT3(1.0f, -2.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
// 		{ XMFLOAT3(2.0f, 3.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },		
// 		{ XMFLOAT3(3.0f, -3.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
// 		{ XMFLOAT3(4.0f, 4.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
// 		{ XMFLOAT3(5.0f, -5.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
// 		{ XMFLOAT3(6.0f, 6.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
// 
// 
// 	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex)  * g_NumberVertex;//不应该还是 4*6
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pVectexArray;//vertices是旧的
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// Create index buffer
	WORD* pIndexArray = new WORD[g_NumberVertex];
	for (int Set_Index_N =0 ; Set_Index_N < g_NumberVertex; Set_Index_N++)
	{
		pIndexArray[Set_Index_N] = Set_Index_N;
	}

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * g_NumberVertex ;//index  的 数目
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = pIndexArray;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	// Set index buffer
	g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology、、//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);//D3D11_PRIMITIVE_TOPOLOGY_LINELIST//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST

																					   // Create the constant buffers
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBNeverChanges);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pCBNeverChanges);
	if (FAILED(hr))
		return hr;

	bd.ByteWidth = sizeof(CBChangeOnResize);
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pCBChangeOnResize);
	if (FAILED(hr))
		return hr;

	bd.ByteWidth = sizeof(CBChangesEveryFrame);
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pCBChangesEveryFrame);
	if (FAILED(hr))
		return hr;

	bd.ByteWidth = sizeof(cbChangesEveryFrameVertexData);
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pCBChangesEveryFrameUpdateVertexData);
	if (FAILED(hr))
		return hr;

	VecStringInt::iterator  iterMapStingInt = mVec_string_int.begin();
	int kk = 0;
	std::string strTempPictureName;


	for (; iterMapStingInt != mVec_string_int.end(); ++iterMapStingInt)
	{

		strTempPictureName = iterMapStingInt->strPictureName;
		ID3D11ShaderResourceView* g_pTextureRV = NULL;
		hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strTempPictureName.c_str(), NULL, NULL, &g_pTextureRV, NULL);		// Load the Texture			//目前支持  png图 dds图
		mVecTextureResourceView.push_back(g_pTextureRV);
	}

	//     hr = D3DX11CreateShaderResourceViewFromFileA( g_pd3dDevice, strPictureName1, NULL, NULL, &g_pTextureRV, NULL );	
	// 	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName1_1, NULL, NULL, &g_pTextureRV1_1, NULL);


	if (FAILED(hr))
		return hr;

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
	if (FAILED(hr))
		return hr;

	// Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -40.0f, 0.0f);//正投影的eye在 后面50米和100米没区别
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);//位置下移一下
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX  	MatrixView = XMMatrixLookAtLH(Eye, At, Up);

	CBNeverChanges cbNeverChanges;
	cbNeverChanges.mView = XMMatrixTranspose(MatrixView);//////////////////////////////////////////sse    simd/////////////////////

														 // 	XMFLOAT4X4 LOAT4X4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
														 // 	XMMATRIX  LiuXMMatrix = XMLoadFloat4x4(&LOAT4X4);

	XMStoreFloat4x4(&mFloat44View, MatrixView);//存到mFloat44View成员变量里起来

	g_pImmediateContext->UpdateSubresource(g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0);

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
	float Orthowidth = 800;
	float Orthoheight = 1600;
	XMMATRIX    MatrixOrtho = XMMatrixOrthographicLH(400, 400 * (Orthoheight / Orthowidth), 0.1f, 100.0f);//正 投影了、、width, height   10 20  是正视锥体的宽高  8/160=1/20  //目前是以高度位置显示5个 正方形
	XMStoreFloat4x4(&mFloat4X4Ortho, MatrixOrtho);
	CBChangeOnResize cbChangesOnResize;
	cbChangesOnResize.mProjection = XMMatrixTranspose(MatrixOrtho);//g_Projection  by liuchang
	g_pImmediateContext->UpdateSubresource(g_pCBChangeOnResize, 0, NULL, &cbChangesOnResize, 0, 0);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void  CleanupDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();

	if (g_pSamplerLinear) g_pSamplerLinear->Release();

	VecShaderResourceView::iterator iter = mVecTextureResourceView.begin();
	for (; iter != mVecTextureResourceView.end(); ++iter)
	{
		(*iter)->Release();  //  if( g_pTextureRV ) g_pTextureRV->Release();
	}

	if (g_pCBNeverChanges) g_pCBNeverChanges->Release();
	if (g_pCBChangeOnResize) g_pCBChangeOnResize->Release();
	if (g_pCBChangesEveryFrame) g_pCBChangesEveryFrame->Release();
	if (g_pCBChangesEveryFrameUpdateVertexData)
	{
		g_pCBChangesEveryFrameUpdateVertexData->Release();
	}
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pIndexBuffer) g_pIndexBuffer->Release();
	if (g_pVertexLayout) g_pVertexLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pDepthStencil) g_pDepthStencil->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();

	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	WNDPROC  winProc = (WNDPROC)(WndProc);
	wcex.lpfnWndProc = winProc;			//WNDPROC 
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mhInstance;
	wcex.hIcon = LoadIcon(mhInstance, (LPCTSTR)IDM_ABOUT);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	TCHAR szbuffClasssName[251];
	wsprintf(szbuffClasssName, L"LiuWindowClass%d", 666);
	wcex.lpszClassName = szbuffClasssName;

	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDM_ABOUT);
	UnregisterClassW(wcex.lpszClassName, wcex.hInstance);


}


unsigned int WINAPI  StartThreadMainFunc(void *ptr, bool isERP)//测试的代码 int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	int i = 0;
	HINSTANCE hInstance = GetModuleHandle(NULL); //获取模块 的instance

	int argCount = 0;
	char* argument[6];
	int arg1 = 0, arg2 = 0, arg3 = 0, arg4 = 0, arg5 = 0, arg6 = 1;



	MSG msg = { 0 };
	while (WM_QUIT != msg.message)    // Main message loop
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			//	printf("ThreadID: %d     3th   while ->PeekMessage!\n", mThreadID);
		}
		else
		{

		}
	}


	return (int)msg.wParam;
}
LRESULT CALLBACK  BitMapWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMainFunc(_In_ int       nCmdShow)
{

	int i = 0;

	HINSTANCE hInstance = GetModuleHandle(NULL); //获取模块 的instance

	int argCount = 0;
	char* argument[6];
	int arg1 = 0, arg2 = 0, arg3 = 0, arg4 = 0, arg5 = 0, arg6 = 1;
	// TODO: 在此放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32WINDOW, szWindowClass, MAX_LOADSTRING);
	//szWindowClass = L"ssssssssb";
	//szTitle = L"Title_SSSSSSSSSSSBB";
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	if (FAILED(InitDevice()))
	{
		CleanupDevice();
		return 0;
	}

	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32WINDOW));

	// 主消息循环: 

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)    // Main message loop
	{
		if (PeekMessage(&msg, NULL,0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render(arg1, arg2, arg3, arg4, arg5, arg6);
		}
	}

	CleanupDevice();

	return (int)msg.wParam;
}


UINT WINAPI  RunMainFuncThread(void* ptr)
{
	int nThreadID = *((int*)ptr);

	SetEvent(g_HEvent);
	//WindDrawD3D   windDrawD3d;
	//windDrawD3d[nThreadID].StartThreadMainFunc(&nThreadID, g_isERp);
	WinMainFunc(10);


	//WaitForSingleObject(g_HEventQuit, INFINITE);//不需要这个行代码

	return 0;
}

LRESULT CALLBACK  BitMapWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE://按下Esc键
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		}
		SetCursor(LoadCursor(NULL, IDC_HELP));//设置新的鼠标光标
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
		hdc = BeginPaint(hWnd, &ps);
		bmp = (HBITMAP)::LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP1),	IMAGE_BITMAP, 0, 0, 0);
		if (bmp != NULL)
		{
			HDC mem = ::CreateCompatibleDC(hdc);
			::SelectObject(mem, bmp);
			::BitBlt(hdc, 0, 0, 48, 48, mem, 0, 0, SRCCOPY);
			::DeleteDC(mem);
			::DeleteObject(bmp);
		}
		else
		{
			wchar_t err[20];
			wsprintfW(err, L"%d\r\n", GetLastError());
			OutputDebugString(err);
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


class Int_Int
{
public:
	Int_Int(int x, int y) : mX(x), mY(y)
	{

	}
public:
	int mX;
	int mY;
};

typedef std::list<Int_Int> VecIntInt;
VecIntInt  mVetIntInt;
HANDLE  g_HEventSetData = CreateEvent(NULL, FALSE, FALSE, NULL);

int  ExportMainStartRun()
{
	for (int n = 1; n < 501; n++)
	{
		mVetIntInt.push_back(Int_Int(0, 0));
	}
	int threadnmumber=99;
	unsigned OutputThreadID;
	HANDLE Thread= (HANDLE)_beginthreadex(NULL, 0, RunMainFuncThread, &threadnmumber, 0, &OutputThreadID);
	//WaitForSingleObject(g_HEventQuit, INFINITE);//会死锁调用者的按键消息
	return 0;
 }


int  ExportSetBreaklineData(int x,int y)
{
//	WaitForSingleObject(g_HEventSetData, INFINITE);
	VecIntInt::iterator iterOne = mVetIntInt.begin();
	iterOne = mVetIntInt.erase(iterOne);//用vector其实不太适合，list的比较适合	//mVetIntInt.pop_front();
	mVetIntInt.push_back(Int_Int(x, y));

//	VecIntInt::iterator iterOne = mVetIntInt.begin();
// 	for (; iterOne != mVetIntInt.end(); )
// 	{
// 		if (mVetIntInt.size() > 500)
// 		{
// 			iterOne = mVetIntInt.erase(iterOne);//用vector其实不太适合，list的比较适合
// 												//	mVetIntInt.pop_front();
// 		}
// 		else
// 		{
// 			iterOne++;
// 		}
// 	}
	return 0;
}

int  rangsound()
{
	MessageBeep(MB_ICONWARNING);//播放声音 响一下
	return 0;
}


//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	float liu;
	float liuarry[3];

	float liuarray_array[4][4];
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32WINDOW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
   // wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName   = NULL;
	wcex.lpszClassName = L"szWindowClass09";// szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中
   g_hInst = hInstance;
   RECT rc = { 0, 0, 1600, 800 };//480
   AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);//L"Tutorial07"
//    mhWnd = CreateWindow(szbuffClasssName, szbuffClasssName, WS_OVERLAPPEDWINDOW,
// 	   CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
// 	   NULL);

   HWND hWnd = CreateWindowW(L"szWindowClass09", L"windowNameLiu", WS_OVERLAPPEDWINDOW,
	   200, 200, rc.right - rc.left, rc.bottom - rc.top, NULL,NULL, hInstance,NULL);

   mhWnd = hWnd;
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   //SetWindowPos()hw
   UpdateWindow(hWnd);

   return TRUE;
}

//绘制 折现的方法
void  drawLine(HDC hdc, POINT apt[],int benginx,int beginy,int endx,int endy)
{
	MoveToEx(hdc, benginx, beginy, NULL);
	LineTo(hdc, endx, endy);
}


void DrawBezier(HDC hdc, POINT apt[])
{
	SetEvent(g_HEventSetData);
	int n = 0;
	int deltaX = 0;
	VecIntInt::iterator iter = mVetIntInt.begin();
	for (; iter!=mVetIntInt.end() ;++iter)
	{
		if (n%2==0)
		{
			if (n!=0)
			{
				LineTo(hdc, deltaX, iter->mY+200);
			}
			MoveToEx(hdc, deltaX, iter->mY + 200, NULL);
		}
		else
		{
			LineTo(hdc, deltaX, iter->mY + 200);
		}
		n++;
		deltaX=deltaX+3;
	}
	ResetEvent(g_HEventSetData);
}


LPCTSTR g_lpchText = TEXT("Hello, windowdll");
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static POINT    apt[4];
	int             cxClient, cyClient;
	HDC             hdc;
	PAINTSTRUCT     ps;

    switch (message)
    {
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		apt[0].x = cxClient / 4;
		apt[0].y = cyClient / 2;

		apt[1].x = cxClient / 2;
		apt[1].y = cyClient / 4;

		apt[2].x = cxClient / 2;
		apt[2].y = 3 * cyClient / 4;

		apt[3].x = 3 * cxClient / 4;
		apt[3].y = cyClient / 2;

		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
			case VK_ESCAPE:
			{
				HWND    hDesk;
				RECT    rc;
				hDesk = GetDesktopWindow();
				GetWindowRect(hDesk, &rc);
				SetWindowLong(hWnd, GWL_STYLE, WS_BORDER);
				SetWindowPos(hWnd, HWND_TOP, 0, 0, rc.right, rc.bottom,	SWP_SHOWWINDOW);//HWND_TOP//HWND_TOPMOST
			}
			break;
			case VK_SPACE://空格键按下时
			{
				HWND    hDesk;
				RECT    rc;
				hDesk = GetDesktopWindow();
// 				GetWindowRect(hDesk, &rc);
// 		HWND hCreateWND=		CreateWindowA("IDD_DIALOG1", "CHIld 子窗口ChildProc ", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_OVERLAPPEDWINDOW, 222, 333, 100, 114, hWnd, (HMENU)(124), hInst, NULL);
// 		ShowWindow(hCreateWND, 123);
// 		HWND hClild = CreateWindowA("BUTTON", "BUTTON创建子窗口", WS_CHILD | WS_VISIBLE, 730, 450, 150, 28, hWnd, (HMENU)123, hInst, NULL);
//				runFlashPictureERP( (size_t)hWnd );//调用 闪烁图片dll的
			}
			break;
			case VK_UP:
			{
			//	addPictureAndFrameNum(5,"1234.png", 60);//测试第二个接口
			}
			break;

		}
	case WM_RBUTTONDOWN:
		RECT  Rect;
		GetWindowRect(hWnd, &Rect);
		//SetWindowPos(hWnd, HWND_TOP, Rect.left, Rect.top, Rect.right- Rect.left+100, Rect.bottom - Rect.top + 100, SWP_NOACTIVATE);//置顶  不激活窗口
		g_lpchText = TEXT("WM_LBUTTONDOWN");

		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
 //           PAINTSTRUCT ps;         // TODO: 在此处添加使用 hdc 的任何绘图代码...
            HDC hdc = BeginPaint(hWnd, &ps);
// 			RECT  Rect;
// 			GetClientRect(hWnd, &Rect);
// 			HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
// 			SelectObject(hdc, hRedBrush);
// 			DrawText(hdc, TEXT("Hello---Windows 98"), -1, &Rect,	DT_SINGLELINE | DT_CENTER | DT_VCENTER);
// 			InvalidateRect(hWnd, NULL, TRUE);
// 			static int  number = 0;			number++;
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}



// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

