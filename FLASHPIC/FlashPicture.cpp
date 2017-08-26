#include "FlashPicture.h"


#include"WindDrawD3D.h"

#include <vector>

#include<fstream>
#include <windows.h>
#include <process.h>  
#include <assert.h>  
#include <iostream>
#include <sstream>

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
#include <map>
#include <vector>

//#include <WinSock2.h>
#pragma comment( lib, "ws2_32.lib" )
#include "NetDefine.h"



FILE *fpOutputThreadID;


HANDLE  g_HEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
bool g_bSingleInstance = true;
bool g_isERp = false;
WindDrawD3D *windDrawD3d;//数组，0开始的：  0、1、2、3、4、5
//queue<std::string> g_strQuence;
int g_windowsNumber = 0; 
int  loopCount=6;//erp循环的第几个开始
int  g_loopCountLeft = 6;//erp循环的第几个开始
std::vector<std::string> split(const std::string &s, const std::string &seperator);

/*设置发送图片闪缩时间的TCP链接与算法方面
*/
UINT WINAPI RunSendTimeMsgThread(void*)
{

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return -1;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return -1;
	}
	// 创建套接字
	mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (mServerSocket == INVALID_SOCKET)
	{
		std::cout << "创建套接字失败!" << std::endl;
		return -1;
	}

	//lookup host to get address information
	if ((gethostbyname(SERVER_IP)) == NULL) {
		fprintf(stderr, "Host lookup failed for '%s'.\n", SERVER_IP);
		return -1;
	}
	//make sure serv_addr is cleared and then set values for each member
	memset(&mServerAddr, 0, sizeof(mServerAddr));
	// 填充服务器的IP和端口号
	mServerAddr.sin_family = AF_INET;
	mServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	mServerAddr.sin_port = htons((u_short)SERVER_PORT);

	// 连接到服务器
	while (!(connect(mServerSocket, (struct sockaddr*)&mServerAddr, sizeof(mServerAddr))) <= 0)
	{
		Sleep(100);
	}
	std::string strStart = "startFlashPic";
	LARGE_INTEGER nStartTcpTime;
	QueryPerformanceCounter(&nStartTcpTime);
	LARGE_INTEGER nFreqTcp;
	QueryPerformanceFrequency(&nFreqTcp);
	double d_freTime = (1000.0 / (double)nFreqTcp.QuadPart);
	double d_startTimems = (double)nStartTcpTime.QuadPart * d_freTime;
	int prec = std::numeric_limits<double>::digits10;
	std::ostringstream start_Buffer;
	start_Buffer.precision(prec);
	start_Buffer << d_startTimems;
	std::string strStartTimeBuf = start_Buffer.str();
	std::string strStartTimeBufAndFlag = strStart + ","+strStartTimeBuf ;
	if (strStartTimeBufAndFlag.length() > 28)
		strStartTimeBufAndFlag = strStartTimeBufAndFlag.substr(0, 28);
	else if (strStartTimeBufAndFlag.length() < 28)
	{
		int i_strBufOut = 28 - strStartTimeBufAndFlag.length();
		for (int ii = 0; ii < i_strBufOut; ii++)
			strStartTimeBufAndFlag = strStartTimeBufAndFlag + "0";

	}
	send(mServerSocket, strStartTimeBufAndFlag.data(), strStartTimeBufAndFlag.size(),0);
	g_bIsConnTCP = true;
	while (1) 
	{

		MSG   smsg;
		PeekMessage(&smsg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
		if (WM_QUIT == smsg.message)
		{
			closesocket(mServerSocket);
			WSACleanup();
			break;
		}

		for (int iSubThread = 0; iSubThread < g_loopCountLeft; ++iSubThread)
		{
			WaitForSingleObject(g_hFull, INFINITE);
			WaitForSingleObject(g_hMutex, INFINITE);
			std::string msg;
			if (!g_strQuence.empty()) 
			{
				msg = g_strQuence.front();
				g_strQuence.pop();
			}
			else 
			{
				ReleaseMutex(g_hMutex);
				ReleaseSemaphore(g_hEmpty, 1, NULL);//信号量+1操作
				continue;
			}
			ReleaseMutex(g_hMutex);
			ReleaseSemaphore(g_hEmpty, 1, NULL);//信号量+1操作

			std::vector<std::string> v_msg = split(msg,",");
			if (v_msg.size() == 2)
			{
				std::stringstream ssStr;
				double d_picFluTime;

				ssStr << v_msg[1];
				ssStr >> d_picFluTime;

				std::ostringstream sso_Buffer;
				sso_Buffer.precision(prec);
				sso_Buffer << d_picFluTime * d_freTime;
				std::string strSSOBuf = sso_Buffer.str();
				if (strSSOBuf.length() > 16)
					strSSOBuf = strSSOBuf.substr(0, 16);
				else if (strSSOBuf.length() < 16)
				{
					int i_strBufOut =16 - strSSOBuf.length();
					for (int ii = 0; ii < i_strBufOut; ii++)
						strSSOBuf = strSSOBuf + "0";
						
				}
				std::string strStringStream = v_msg[0]+","+ strSSOBuf;

				send(mServerSocket, strStringStream.c_str(), strStringStream.size(), 0);
			}

		}
		Sleep(0);
	}

	return 1;
}

std::vector<std::string> split(const std::string &s, const std::string &seperator) 
{
	std::vector<std::string> result;
	typedef std::string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}
/*分配每个线程的windDrawD3d对象和每个线程的*mian*函数入口
*/
UINT WINAPI  RunSubThread(void* ptr)
{
	int nThreadID = *((int*)ptr);
	printf("ThreadID: %d  is   run____RunSubThread\n", nThreadID);
	SetEvent(g_HEvent);
	//WindDrawD3D   windDrawD3d;
	windDrawD3d[nThreadID].StartThreadMainFunc(&nThreadID,g_isERp);
	int i = 0;
	printf("ThreadID: %d  is   End911_911911911911____RunSubThread\n", nThreadID);
	g_windowsNumber--;
	if (g_windowsNumber==0)
	{
		SetEvent(g_HEventQuit);
		delete[]windDrawD3d;
	}
	return 0;
}

int g_lastSixPollPicture = 0;////记录上次轮询结束时的那个图片，避免下次sixpoll的时候又出现它

//控制ERP 用的线程
UINT WINAPI  RunControlERP(void* ptr)
{
	bool  No_0 = false;
	bool  No_1 = false;
	bool  No_2 = false;
	bool  No_3 = false;
	bool  No_4 = false;
	bool  No_5 = false;
BeginPlaySixPicture:  
		//printf("begin play six picture ");           //播放这 6张 图片-------------------------------------
	if (g_loopCountLeft == 6)
	{
		No_0 = false;
		No_1 = false;
		No_2 = false;
		No_3 = false;
		No_4 = false;
		No_5 = false;
	}

beginRand:									//如果 重复了以前的选择，则重新选择一个 0~5的随机数----------------------------
	int randNumber = rand() % 6;
	if (g_loopCountLeft == 6)
	{
		if (randNumber==g_lastSixPollPicture)
		{
			goto beginRand;
		}
	}
	if (randNumber == 0)
	{
		if (No_0)
		{
			goto beginRand;
		}
		No_0 = true;
		windDrawD3d[0].setBeginPlay(true);
		g_loopCountLeft--;
	}

	if (randNumber == 1)
	{
		if (No_1)
		{
			goto beginRand;
		}
		No_1 = true;
		windDrawD3d[1].setBeginPlay(true);
		g_loopCountLeft--;
	}

	if (randNumber == 2)
	{
		if (No_2)
		{
			goto beginRand;
		}
		No_2 = true;
		windDrawD3d[2].setBeginPlay(true);
		g_loopCountLeft--;
	}

	if (randNumber == 3)
	{
		if (No_3)
		{
			goto beginRand;
		}
		No_3 = true;
		windDrawD3d[3].setBeginPlay(true);
		g_loopCountLeft--;
	}

	if (randNumber == 4)
	{
		if (No_4)
		{
			goto beginRand;
		}
		No_4 = true;
		windDrawD3d[4].setBeginPlay(true);
		g_loopCountLeft--;
	}

	if (randNumber == 5)
	{
		if (No_5)
		{
			goto beginRand;
		}
		No_5 = true;
		windDrawD3d[5].setBeginPlay(true);
		g_loopCountLeft--;
	}

	if (g_loopCountLeft == 0)
	{
		g_lastSixPollPicture = randNumber;//记录上次轮询结束时的那个图片，避免下次sixpoll的时候又出现它
		g_loopCountLeft = 6;//一个小六轮结束了
	}
	WaitForSingleObject(g_HEventQuit, INFINITE);
	goto BeginPlaySixPicture;
	return 0;
}

int addPictureAndFrameNum(int NumberPicture, char * strImagePictureName,int numFrame)
{
	windDrawD3d[NumberPicture].addPictureAndTime(strImagePictureName, numFrame);//测试 对外接口
	return 0;
}

int  setPicturePosition(int NumberPicture, int x, int y, int width, int height)
{
	windDrawD3d[NumberPicture].setWindowPosition(x, y, width, height);//测试 对外接口
	return 0;
}

int 	setWindowAlpha(int alpha)
{
	for (int n=0;n<6;n++)
	{
		windDrawD3d[n].setWindowAlpha(alpha);
	}
	return 1;
}



int   runFlashPicture(unsigned   int  hWnd)//_TCHAR    //这里是真正的main函数，程序的入口  int argc, char* argv[]
{
	if (g_bSingleInstance==true)
	{
		g_bSingleInstance = false;
	}
	else
	{
		return 0;
	}

	//AllocConsole();//生产一个控制台用于显示当前是哪个图片开始显示和熄灭 ，屏蔽这里可以去掉console显示
	//freopen("CONOUT$", "w", stdout);
	//std::cout << "ssssb333333333333bb" << std::endl;

	int nThreadCount = 6;
	g_windowsNumber = 6;
	HANDLE* phaThread = new HANDLE[nThreadCount];
	windDrawD3d=new WindDrawD3D[nThreadCount];

	fpOutputThreadID = fopen("OutputThreadID.txt", "w+");
	unsigned OutputThreadID[6];
	int nErr = 0;

	unsigned sendThreadID;
	HANDLE p_hSendThread = (HANDLE)_beginthreadex(NULL, 0, RunSendTimeMsgThread,nullptr, 0, &sendThreadID);
	fprintf(fpOutputThreadID, "time%d\n", sendThreadID);
	WaitForSingleObject(p_hSendThread, 1000);
	for (int i = 0; i < nThreadCount; i++)
	{
// 		typedef unsigned(__stdcall* _begin thread ex_proc_type)(void*);// 		_beginthreadex_proc_type  func;
		phaThread[i] = (HANDLE)_beginthreadex(NULL, 0, RunSubThread, &i, 0, &OutputThreadID[i]);
		if (phaThread[i] == 0)
		{
			nErr = GetLastError();
			if (nErr == 8)
			{
				printf("Open thread failure, insufficient storage space !\n");
			}
			else
			{
				printf("Open thread failure, wrong number %d\n", nErr);
			}
			break;
		}
		fprintf(fpOutputThreadID, "%d\n", OutputThreadID[i]);//输出 OutputThreadID 到 文件 file
	
		WaitForSingleObject(g_HEvent, INFINITE);
	}
	//::closesocket(mServerSocket);
	fclose(fpOutputThreadID);
	Sleep(300);
	//windDrawD3d[5].addPictureAndTime("1234.png", 60);//测试 对外接口
	//windDrawD3d[4].setWindowPosition(1188, 888, 44, 44);//测试 对外接口
	

	HWND  hwnd = (HWND)hWnd;
	if (hwnd==NULL)
	{
		return 0;
	}
	SetForegroundWindow(hwnd);
	SetActiveWindow(hwnd);
	SetFocus(hwnd);

	return 0;//退出了
}




int   runFlashPictureERP(unsigned   int  hWnd)//_TCHAR    //这里是真正的main函数，程序的入口  int argc, char* argv[]
{
	if (g_bSingleInstance == true)
	{
		g_bSingleInstance = false;
	}
	else
	{
		return 0;
	}
	AllocConsole();//生产一个控制台用于显示当前是哪个图片开始显示和熄灭 ，屏蔽这里可以去掉console显示
	freopen("CONOUT$", "w", stdout);
	std::cout << "ssssb333333333333bb" << std::endl;

	int nThreadCount = 6;
	g_windowsNumber = 6;
	HANDLE* phaThread = new HANDLE[nThreadCount];
	windDrawD3d = new WindDrawD3D[nThreadCount];
	g_isERp = true;
	unsigned OutputThreadID[6];
	int nErr = 0;
	for (int i = 0; i < nThreadCount; i++)
	{
		phaThread[i] = (HANDLE)_beginthreadex(NULL, 0, RunSubThread, &i, 0, &OutputThreadID[i]);

		if (phaThread[i] == 0)
		{
			nErr = GetLastError();
			if (nErr == 8)
			{
				printf("Open thread failure, insufficient storage space !\n");
			}
			else
			{
				printf("Open thread failure, wrong number %d\n", nErr);
			}
			break;
		}

		WaitForSingleObject(g_HEvent, INFINITE);
	}
	Sleep(30);

	HWND  hwnd = (HWND)hWnd;
	if (hwnd == NULL)
	{
		//return 0;
	}
	else
	{
		SetForegroundWindow(hwnd);
		SetActiveWindow(hwnd);
		SetFocus(hwnd);
	}

	unsigned threadIDss;
	int arglist;
	(HANDLE)_beginthreadex(NULL, 0, RunControlERP, &arglist, 0, &threadIDss);
	//WaitForSingleObject(g_HEventQuit, INFINITE);	//	//这个会死锁，等待其他地方解锁才能，死锁后游戏窗口不能动了坑爹啊，屏蔽这条代码，，，也控制整个程序进程不退出的   //  为ERP的作为命令者  的  抽签事物
	return 0;//退出 对6个窗口的命令控制
}



