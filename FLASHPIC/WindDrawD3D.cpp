#include"WindDrawD3D.h"


UINT WINAPI  WindDrawD3D::SubThread(void* ptr)
{
	int nThreadID = *((int*)ptr);
	nThreadID++;
	SetEvent(ghDataEvent);
	printf("%d Thead liuchang  is  running......./n", nThreadID);
	int i = 0;
	while (!gbExitThread)
	{
		Sleep(1000);
	}

	return 0;
}
int g_iMaxQueneSize = 6;
HANDLE  ghDataEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE  g_HEventQuit = CreateEvent(NULL, FALSE, FALSE, NULL);//控制runflash函数不退出，这样就可以 命令控制我们了
HANDLE  g_hMutex = CreateMutex(NULL, FALSE, NULL);//while(0|1)
HANDLE g_hEmpty = CreateSemaphore(NULL, g_iMaxQueneSize, g_iMaxQueneSize, _T("empty"));//--操作
HANDLE g_hFull = CreateSemaphore(NULL, 0, g_iMaxQueneSize, _T("full"));//--操作
queue<std::string> g_strQuence;
bool g_bIsConnTCP = false;
//ShareStackSpace<std::string> shareStackPic(1024);

WindDrawD3D::WindDrawD3D() : g_vMeshColor(0.7f, 0.7f, 0.7f, 1.0f)
{
	mScopeNum = 0;
	mbeginShowPictureTime = 0;
	mbeginShowBlackTime = 0;
	mIsBeginPlay = false;
	mIsEndPlay = false;
	mAlpha = 255;
	//mFloat44View = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	mTimeLast = 0.016666;
	mDeltaTime = 0;
	isFirst = true;
	mNFrames = 0;
	g_NumberQuat = 1;
	nCount = 0;
	g_NumberQuat = 1;
	g_ArgN1 = 0;
	g_ArgM1 = 0;
	g_ArgNM1 = 0;

	g_ArgN2 = 0;
	g_ArgM2 = 0;
	g_ArgNM2 = 0;

	g_ArgN3 = 0;
	g_ArgM3 = 0;
	g_ArgNM3 = 0;

	g_ArgN4 = 0;
	g_ArgM4 = 0;
	g_ArgNM4 = 0;

	g_ArgN5 = 0;
	g_ArgM5 = 0;
	g_ArgNM5 = 0;

	g_ArgN6 = 0;
	g_ArgM6 = 0;
	g_ArgNM6 = 0;
	// Global Variables
	g_hInst = NULL;
	mhWnd = NULL;
	g_driverType = D3D_DRIVER_TYPE_NULL;
	g_featureLevel = D3D_FEATURE_LEVEL_11_0;
	g_pd3dDevice = NULL;
	g_pImmediateContext = NULL;
	g_pImmediateContext2 = NULL;
	g_pSwapChain = NULL;
	g_pRenderTargetView = NULL;
	g_pDepthStencil = NULL;
	g_Texture2D_RTT = NULL;
	g_pDepthStencilView = NULL;
	g_pVertexShader = NULL;
	g_pPixelShader = NULL;
	g_pVertexLayout = NULL;
	g_pVertexBuffer = NULL;
	g_pIndexBuffer = NULL;
	g_pCBNeverChanges = NULL;
	g_pCBChangeOnResize = NULL;
	g_pCBChangesEveryFrame = NULL;
	g_pTextureRV17 = NULL;
	g_TotalFrameNumber = 0;
	gbExitThread = false;
}
//通过config文件获得纹理信息数据和窗口尺寸数据
void  WindDrawD3D::getDataFromConfigFile(int nThreadID)
{
	FILE *fp;
	if (mIsERP)
	{
		fp = fopen("ConfigERP.txt", "rt+");
	}
	else
	{
		fp = fopen("Config.txt", "rt+");
	}

	//	fscanf(fp, "%d %d %d %d %d\n", &arg1, &arg2, &arg3, &arg4, &arg5);    ////将123456和789写到test.txt文件中	//fprintf(fp, "%s %s", "China", "ChongQing"); //将字符串China和ChongQing追加写到test.txt文件中
	//		if ('\n' == szbuff[0]) continue;     //空行，结束这行数据解析操作并继续
	//	char *pchar;	//		pchar = szbuff;

	char szbuff[256];
	int nCountSpace = 0;
	int NumberParase = 6 + nThreadID - 1;
	fscanf(fp, "%d\n", &mAlpha);//先读取第一行得到alpha值
	for (int k = 0; k < NumberParase; k++)//解析前6行 的文本数据
	{
		fgets(szbuff, 99, fp);//读取一行的字符串内容
		if (k != (nThreadID - 1))//根据不同的线程 读取不同的行的 数据
		{
			continue;//除了自己这行的数据，其他的都跳过不管
		}
		std::string strLineData(szbuff);

		int start = 0, end = 0, nPosi = 0, n = 0, n1 = 0;
		while (szbuff[n1] != '\n')			//对此行字符串
		{
			if (szbuff[n1] == ' ')
			{
				nCountSpace++;
			}
			n1++;
		}
		int nPairCount = (nCountSpace + 1) / 2;//为了获得空格的数目		//std::vecter< int > vecInt;
		nCountSpace = 0;
		while (true)			//对此行字符串 	///////////////while (szbuff[n] != '\n')	/////////////-------------------------/////////
		{
			if (szbuff[n] == '\n')//最后的一个字符   \n
			{
				nPosi++;
				nCountSpace++;			//	te[flag][num] = str[t];
				std::string strName1 = strLineData.substr(start, nPosi - start - 1);//截取字符串
				int nFrameShowCount, kk = 0;;
				nFrameShowCount = atoi(strName1.c_str());//90
				VecStringInt::iterator  iterMapStingInt = mVec_string_int.begin();
				for (; iterMapStingInt != mVec_string_int.end(); ++iterMapStingInt)
				{
					kk++;
					if (kk == (nCountSpace - nPairCount))
					{
						iterMapStingInt->NumberFrame = nFrameShowCount; 				//	mVec_string_int[nCountSpace- nPairCount/2] = //真正的数字；			//	或者	vecInt.push(nFrameShowCount);////真正的数字；
						g_TotalFrameNumber += nFrameShowCount;//最后的那个 数字
					}
				}
				break;
			}
			if (szbuff[n] == ' ')
			{
				nPosi++;
				nCountSpace++;			//	te[flag][num] = str[t];
				std::string strName1 = strLineData.substr(start, nPosi - start - 1);//截取字符串
				if (nCountSpace >nPairCount)
				{
					int nFrameShowCount, kk = 0;;
					nFrameShowCount = atoi(strName1.c_str());
					VecStringInt::iterator  iterMapStingInt = mVec_string_int.begin();
					for (; iterMapStingInt != mVec_string_int.end(); ++iterMapStingInt)
					{
						kk++;
						if (kk == (nCountSpace - nPairCount))
						{
							iterMapStingInt->NumberFrame = nFrameShowCount; 				//	mVec_string_int[nCountSpace- nPairCount/2] = //真正的数字；			//	或者	vecInt.push(nFrameShowCount);////真正的数字；
							g_TotalFrameNumber += nFrameShowCount;
						}
					}
				}
				else
				{
					//999是暂时 数字 int//  这里 把真正的 pictureName放进来了           //这里把picturename和 帧数都 获得了
					//mVec_string_int[strName1] = 999;
					mVec_string_int.push_back(String_Int(strName1, 999));
				}
				start = nPosi;
			}
			else
			{
				nPosi++;
			}
			n++;
		}					///////////////while (szbuff[n] != '\n')	///////////////------------------------------------------///////
	}//end  //////////////	for (int k = 0; k < 6; k++)/////
	fscanf(fp, "%d %d %d %d\n", &mArgPositionX, &mArgPositionY, &mArgWidth, &mArgHeight);//窗口起始位置和宽高（对应视锥体也是这样，等比例缩小  ,0.05吧)
	fclose(fp);
}

void WindDrawD3D::addPictureAndTime(std::string strPictureName, int numberFrame)
{
	ID3D11ShaderResourceView* g_pTextureRVTemp = NULL;
	HRESULT	hr = D3DX11CreateShaderResourceViewFromFileA(g_pd3dDevice, strPictureName.c_str(), NULL, NULL, &g_pTextureRVTemp, NULL);		// Load the Texture			//目前支持  png图 dds图
	mVecTextureResourceView.push_back(g_pTextureRVTemp);
	g_TotalFrameNumber += numberFrame;
	if (g_TotalFrameNumber == 0)
	{
		SetLayeredWindowAttributes(mhWnd, 0, 0, LWA_ALPHA);//不显示这个窗体了
	}
	else
	{
		SetLayeredWindowAttributes(mhWnd, 0, mAlpha, LWA_ALPHA);//255的ALPHA
	}

	mVec_string_int.push_back(String_Int(strPictureName, numberFrame));
	int n = 0;
	n++;
}
//void WindDrawD3D::setShareStackSpace(ShareStackSpace<std::string> shareStackPic)
//{
//
//	this->shareStackPic = shareStackPic;
//
//}
unsigned int WINAPI  WindDrawD3D::StartThreadMainFunc(void *ptr,bool isERP)//int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	mIsERP = isERP;
	if (mIsERP)
	{
		mCount_SetRV = 99999;//永不进入 PSSetShaderResources
	}
	 mThreadID = *((int*)ptr);

	SetEvent(ghDataEvent);
	printf("ThreadID: %d  is  Secend_Printf Going StartThreadMainFunc\n", mThreadID);
	mThreadID++;

	int i = 0;

	HINSTANCE hInstance = GetModuleHandle(NULL); //获取模块 的instance
	int nCmdShow = 10;
	// UNREFERENCED_PARAMETER( hPrevInstance );
	// UNREFERENCED_PARAMETER( lpCmdLine );
	//	g_hInst = hInstance;
	QueryPerformanceFrequency(&nFreq);//获取前后的时间，精确到微秒



	fpOutputTime = fopen("OutputTime.txt", "a+");
	//printf("ThreadID: %d     SSSS____BBBB   while_GoingPeekMessage!\n", mThreadID);
	// fprintf(fpOutputTime, "%f , %f\n", 9.999, 8.888);//将123456和789写到test.txt文件中	fprintf(fp, "%s %s", "China", "ChongQing"); //将字符串China和ChongQing追加写到test.txt文件中

	int argCount = 0;
	char* argument[6];
	int arg1 = 0, arg2 = 0, arg3 = 0, arg4 = 0, arg5 = 0, arg6 = 1;

	getDataFromConfigFile(mThreadID);//通过config文件获得纹理信息数据和窗口尺寸数据

	if (FAILED(InitWindow(hInstance, nCmdShow, mThreadID)))
		return 0;

	if (FAILED(InitDevice()))
	{
		CleanupDevice();
		return 0;
	}

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
			if (mIsERP)
			{
				RenderInERP(arg1, arg2, arg3, arg4, arg5, arg6);
			} 
			else
			{
				Render(arg1, arg2, arg3, arg4, arg5, arg6);
			}
		}
	}

	CleanupDevice();
	fclose(fpOutputTime);


	return (int)msg.wParam;
}


//------------------------------------------------------------------------------------------------------------------------------
HRESULT  WindDrawD3D::InitWindow(HINSTANCE hInstance, int nCmdShow,int nThreadID)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	WNDPROC  winProc= (WNDPROC)(WindDrawD3D::WndProc);
	wcex.lpfnWndProc = winProc;			//WNDPROC 
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	  mhInstance= hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDM_ABOUT);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	TCHAR szbuffClasssName[251];
	wsprintf(szbuffClasssName, L"LiuWindowClass%d", nThreadID);
	wcex.lpszClassName = szbuffClasssName;

	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDM_ABOUT);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 160, 320 };//480
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);//L"Tutorial07"
	mhWnd = CreateWindow(szbuffClasssName, szbuffClasssName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!mhWnd)
		return E_FAIL;

	//       //WS_EX_TRANSPARENT| WS_BORDER		//						dx11渲染的弊端：		没border 会创建 硬件版的swapChain都会失败,还有SetWindowPos的宽高130 40是最小值了
	SetWindowLong(mhWnd, GWL_STYLE, WS_BORDER);// 会导致不能点击关闭来关闭这个窗口// WS_BORDER是无边框| WS_EX_NOACTIVATE
	SetWindowLong(mhWnd, GWL_EXSTYLE, GetWindowLongPtr(mhWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT |WS_EX_LAYERED);//这个WS_EX_LAYERED导致透明成功

	ShowWindow(mhWnd, nCmdShow);
	SetWindowPos(mhWnd, HWND_TOPMOST, mArgPositionX, mArgPositionY, mArgWidth, mArgHeight, SWP_NOACTIVATE);//置顶  不激活窗口

	if (g_TotalFrameNumber == 0)
	{
		SetLayeredWindowAttributes(mhWnd, 0, 0, LWA_ALPHA);//不显示这个窗体了
	}
	else
	{
		SetLayeredWindowAttributes(mhWnd, 0, mAlpha, LWA_ALPHA);//255的ALPHA
	}

	return S_OK;
}


//------------------------------------------------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT  WindDrawD3D::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT  WindDrawD3D::InitDevice()
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
// 	// We need a screen-sized STAGING resource for frame capturing    //liuchang-------------------------------------------------
// 	D3D11_TEXTURE2D_DESC TexDesc;
// 	DXGI_SAMPLE_DESC SingleSample = { 1, 0 };
// 	TexDesc.Width = width;
// 	TexDesc.Height = height;
// 	TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		//pBackBufferSurfaceDesc->Format
// 	TexDesc.SampleDesc = SingleSample;
// 	TexDesc.MipLevels = 1;
// 	TexDesc.Usage = D3D11_USAGE_STAGING;
// 	TexDesc.MiscFlags = 0;
// 	TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
// 	TexDesc.BindFlags = 0;
// 	TexDesc.ArraySize = 1;
// 	(g_pd3dDevice->CreateTexture2D(&TexDesc, NULL, &g_pD3DTexture2D_Capture));

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
	hr = CompileShaderFromFile(L"Tutorial07.fx", "VS", "vs_4_0", &pVSBlob);
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
	hr = CompileShaderFromFile(L"Tutorial07.fx", "PS", "ps_4_0", &pPSBlob);
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

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },			//后面 //这里的    正的纹理

																		// 		{ XMFLOAT3(-1.0f+3* DelatPos, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
																		// 		{ XMFLOAT3(1.0f + 3 * DelatPos, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
																		// 		{ XMFLOAT3(1.0f + 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
																		// 		{ XMFLOAT3(-1.0f + 3 * DelatPos, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },			//后面 x方向偏移 3 、、这里的      反的纹理
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4 * g_NumberQuat;//不应该还是 4*6
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// Create index buffer
	// Create vertex buffer
	WORD indices[] =
	{
		3,1,0,
		2,1,3
		// 		7,5,4,
		// 		6,5,7,
	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * g_NumberQuat * 6;//index  的 数目
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	// Set index buffer
	g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//D3D11_PRIMITIVE_TOPOLOGY_LINELIST//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST

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
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -20.0f, 0.0f);
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
	float Orthowidth = mArgWidth;
	float Orthoheight = mArgHeight;
	XMMATRIX    MatrixOrtho = XMMatrixOrthographicLH(2, 2 * (Orthoheight / Orthowidth), 0.1f, 100.0f);//正 投影了、、width, height   10 20  是正视锥体的宽高  8/160=1/20  //目前是以高度位置显示5个 正方形
	XMStoreFloat4x4(&mFloat4X4Ortho, MatrixOrtho);
	CBChangeOnResize cbChangesOnResize;
	cbChangesOnResize.mProjection = XMMatrixTranspose(MatrixOrtho);//g_Projection  by liuchang
	g_pImmediateContext->UpdateSubresource(g_pCBChangeOnResize, 0, NULL, &cbChangesOnResize, 0, 0);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void  WindDrawD3D::CleanupDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();

	if (g_pSamplerLinear) g_pSamplerLinear->Release();

	VecShaderResourceView::iterator iter = mVecTextureResourceView.begin();
	for (; iter != mVecTextureResourceView.end(); ++iter)
	{
		if(*iter)
			(*iter)->Release();  //  if( g_pTextureRV ) g_pTextureRV->Release();
	}

	if (g_pCBNeverChanges) g_pCBNeverChanges->Release();
	if (g_pCBChangeOnResize) g_pCBChangeOnResize->Release();
	if (g_pCBChangesEveryFrame) g_pCBChangesEveryFrame->Release();
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
	WNDPROC  winProc = (WNDPROC)(WindDrawD3D::WndProc);
	wcex.lpfnWndProc = winProc;			//WNDPROC 
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mhInstance;
	wcex.hIcon = LoadIcon(mhInstance, (LPCTSTR)IDM_ABOUT);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	TCHAR szbuffClasssName[251];
	wsprintf(szbuffClasssName, L"LiuWindowClass%d", mThreadID);
	wcex.lpszClassName = szbuffClasssName;

	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDM_ABOUT);
	UnregisterClassW(wcex.lpszClassName, wcex.hInstance);


}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK  WindDrawD3D::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		hdc = BeginPaint(hWnd, &ps);
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



//--------------------------------------------------------------------------------------
// Render a frame in ssvep
//--------------------------------------------------------------------------------------
void  WindDrawD3D::Render(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
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
	float ClearColor[4] = { 0.0f, 1.0f, 0.0f, 0.0f }; // red, green, blue, alpha { 0.0f, 0.125f, 0.3f, 1.0f }; //最后的alpha值好像没用
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

	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);

	mCount_SetRV++;
	int nCountPictureNum = mVec_string_int.size();//-----------------------------------------对每帧进行 筛选，进行设置对应的 纹理view
	VecStringInt::iterator iterMapString = mVec_string_int.begin();
	int nnCountForLoop = -1;
	int StartFrameCount = 1;
	int EndFrameCount = 0;

	strRoundEndMsg = "";

	
	//到那一帧显示那张图片的算法ssvep
	for (; iterMapString != mVec_string_int.end(); ++iterMapString)
	{
		nnCountForLoop++;//等同于 到了 哪张图了
		EndFrameCount += iterMapString->NumberFrame;//累计帧数 //更新 区间
		if (mCount_SetRV >= StartFrameCount && mCount_SetRV <= EndFrameCount)
		{				//目前只有一张samlper，让后设置不同的 纹理view
			g_pImmediateContext->PSSetShaderResources(0, 1, &mVecTextureResourceView[nnCountForLoop]);//PSSetShaderResources这个函数是位于 render函数，也就是可以每时每刻都执行一次set sample的纹理值 ，这样我第一张图 每帧设置 100张图片里的某一张都没事
			if (mCount_SetRV >= g_TotalFrameNumber)
			{		
				mCount_SetRV = 0;//重新开始下一个轮回
				strRoundEndMsg += iterMapString->strPictureName;
				
			}
			break;
		}
		StartFrameCount += iterMapString->NumberFrame;//更新 区间
	}

	//---------------------------------------------------------------------------------------------对每帧进行 筛选，进行设置对应的 纹理view

	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);//0,1  1,1   2,1  这些玩意第一个数表示register里的数字，“register( b2 )”就写2

	// Update variables that change once per frame
	XMFLOAT4 movePos(0.0f, 0.0f, 0.0f, 1.0f);//移动y方向上下移几个单位 /绘制正方形   正方形的宽高都是1个单位     （-1 1 ， 11  ，-1-1，  1 -1）
	XMMATRIX	MatrixWorld2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
	CBChangesEveryFrame cb;
	cb.mWorld = XMMatrixTranspose(MatrixWorld2);
	cb.vMeshColor = XMFLOAT4(0.15f, 1.0f, 1.0f, 1.0f);			//为了 图A  的显示
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
	g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);			//可以执行绘制的

	g_pSwapChain->Present(1, 0);//  1  ： 开启垂直同步 （fps大约60）

	QueryPerformanceCounter(&nBeginTime);
	double  TimeCurrent = (double)(nBeginTime.QuadPart);
	double deltaTime = (TimeCurrent - mTimeLast) / (double)nFreq.QuadPart;
	mTimeLast = TimeCurrent;
	//TCP发送的字符串拼接
	if (strRoundEndMsg.compare("") != 0)
	{
		int prec = std::numeric_limits<double>::digits10;
		std::ostringstream sso_Buffer;
		sso_Buffer.precision(prec);
		sso_Buffer << mTimeLast;
		std::string strSSOBuf = sso_Buffer.str();

		strRoundEndMsg +="," ;
		strRoundEndMsg += strSSOBuf;
		if (strRoundEndMsgLen == 0)
			strRoundEndMsgLen = strRoundEndMsg.length();
		if (g_bIsConnTCP)
		{
			WaitForSingleObject(g_hEmpty, INFINITE);
			WaitForSingleObject(g_hMutex, INFINITE);
			
			g_strQuence.push(strRoundEndMsg);

			ReleaseMutex(g_hMutex);
			ReleaseSemaphore(g_hFull, 1, NULL);
		}
		//shareStackPic.Push(strRoundEndMsg);
		//std::cout << strRoundEndMsg << std::endl;
	}

	mDeltaTime += deltaTime;//deltaTime约等于 0.0166

	//统计 fps-----------------------------------------------
	mNFrames++;
	if (mDeltaTime>1.0f)
	{
		mDeltaTime -= 1.0f;
		if (mDeltaTime>10.0f)
		{
			mDeltaTime = 0;
		}
		char szBuff[255] = { 0 };
		sprintf(szBuff, "%d FPS", mNFrames);
		SetWindowTextA(mhWnd, szBuff);
		mNFrames = 0;
	}//统计 fps-----------------------------------------------

	if (isFirst)
	{
		isFirst = false;
		return;
	}
	fprintf(fpOutputTime, "%f\n", deltaTime);//输出 【两帧】之间的时间间隔 到 文件 file
}
//--------------------------------------------------------------------------------------
// Render a frame  in ERP  style
//--------------------------------------------------------------------------------------
void  WindDrawD3D::RenderInERP(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
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
	float ClearColor[4] = { 0.0f, 1.0f, 0.0f, 0.0f }; // red, green, blue, alpha { 0.0f, 0.125f, 0.3f, 1.0f }; //最后的alpha值好像没用
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

	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pCBChangesEveryFrame);

// 	if (mIsBeginPlay)
// 	{				//受到命令激活后
// 		mCount_SetRV = 0;//重新开启 paly
// 	}
	mCount_SetRV++;

	int nCountPictureNum = mVec_string_int.size();//-----------------------------------------对每帧进行 筛选，进行设置对应的 纹理view
	VecStringInt::iterator iterMapString = mVec_string_int.begin();
	int nnCountForLoop = -1;
	int StartFrameCount = 1;
	int EndFrameCount = 0;
	//到那一帧显示那张图片的算法erp
	for (; iterMapString != mVec_string_int.end(); ++iterMapString)
	{
		//mScopeNum++;
		nnCountForLoop++;//等同于 到了 哪张图了
		EndFrameCount += iterMapString->NumberFrame;//累计帧数 //更新 区间
		if (mCount_SetRV >= StartFrameCount && mCount_SetRV <= EndFrameCount)
		{				//目前只有一张samlper，让后设置不同的 纹理view
			if (mScopeNum == 0)//0
			{
				if (mIsPictureShow)
				{
					mScopeNum=1;//1
					QueryPerformanceCounter(&nBeginTime);
					mbeginShowPictureTime = (double)(nBeginTime.QuadPart);//记录开始显示 A.jpg或者 b.jpg   图片的时刻
					//printPictureNumbAndTime();
					mIsPictureShow = false;
				}
			}
			else if(mScopeNum==2)//2
			{
				if (!mIsPictureShow)
				{
					QueryPerformanceCounter(&nBeginTime);
					mbeginShowBlackTime = (double)(nBeginTime.QuadPart);//记录开始显示 黑色 图片的时刻
					//printNum_And_BlackAndTime();
					mIsPictureShow = true;
				}
			}
			g_pImmediateContext->PSSetShaderResources(0, 1, &mVecTextureResourceView[nnCountForLoop]);//PSSetShaderResources这个函数是位于 render函数，也就是可以每时每刻都执行一次set sample的纹理值 ，这样我第一张图 每帧设置 100张图片里的某一张都没事
			if (mCount_SetRV >= g_TotalFrameNumber)
			{
				//mCount_SetRV = 0;//重新开始下一个轮回       //这个重新开始下一个轮回恐怕得等上面下命令了
				SetEvent(g_HEventQuit);   //这里还得通知 命令者  我们已经结束 play了，开始下一次抽签   决定开启  下一家    的play
				mScopeNum = 0;
			}
			break;
		}
		StartFrameCount += iterMapString->NumberFrame;//更新 区间
		mScopeNum=2;//2
	}
	if (iterMapString== mVec_string_int.end())
	{
		mScopeNum = 0;
	}


	//---------------------------------------------------------------------------------------------对每帧进行 筛选，进行设置对应的 纹理view

	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);//0,1  1,1   2,1  这些玩意第一个数表示register里的数字，“register( b2 )”就写2
	//g_pImmediateContext->RSSetState(g_pRasterState);
																// Update variables that change once per frame
	XMFLOAT4 movePos(0.0f, 0.0f, 0.0f, 1.0f);//移动y方向上下移几个单位 /绘制正方形   正方形的宽高都是1个单位     （-1 1 ， 11  ，-1-1，  1 -1）
	XMMATRIX	MatrixWorld2 = XMMatrixTranslationFromVector(XMLoadFloat4(&movePos));//移动y方向上下移几个单位  // *XMMatrixRotationZ(t);//   旋转了           liuchang by	
	CBChangesEveryFrame cb;
	cb.mWorld = XMMatrixTranspose(MatrixWorld2);
	cb.vMeshColor = XMFLOAT4(0.15f, 1.0f, 1.0f, 1.0f);			//为了 图A  的显示
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
	g_pImmediateContext->DrawIndexed(6 * g_NumberQuat, 0, 0);			//可以执行绘制的




	g_pSwapChain->Present(1, 0);//  1  ： 开启垂直同步 （fps大约60）

	QueryPerformanceCounter(&nBeginTime);
	double  TimeCurrent = (double)(nBeginTime.QuadPart);
	double deltaTime = (TimeCurrent - mTimeLast) / (double)nFreq.QuadPart;
	mTimeLast = TimeCurrent;



	mDeltaTime += deltaTime;//deltaTime约等于 0.0166

	mNFrames++;//统计 fps-----------------------------------------------
	if (mDeltaTime > 1.0f)
	{
		mDeltaTime -= 1.0f;
		if (mDeltaTime > 10.0f)
		{
			mDeltaTime = 0;
		}
		char szBuff[255] = { 0 };
		sprintf(szBuff, "%d FPS", mNFrames);
		SetWindowTextA(mhWnd, szBuff);
		mNFrames = 0;
	}//统计 fps-----------------------------------------------

	if (isFirst)
	{
		isFirst = false;
		return;
	}
//	fprintf(fpOutputTime, "%f\n", deltaTime);//输出 【两帧】之间的时间间隔 到 文件 file
}