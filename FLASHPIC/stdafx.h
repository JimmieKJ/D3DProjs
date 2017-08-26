// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#include <stdio.h>
#include <tchar.h>
#include <iostream>


#include <WinSock2.h>
#pragma comment( lib, "ws2_32.lib" )


#include "NetDefine.h"


#define SERVER_PORT		20001
#define SERVER_IP		"192.168.1.103"
#define BUFFER_SIZE		255


class WinSocketSystem
{
public:
	WinSocketSystem()
	{
		int iResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
		if (iResult != NO_ERROR)
		{
			exit(-1);
		}
	}

	~WinSocketSystem()
	{
		WSACleanup();
	}

protected:
	WSADATA wsaData;
};

static WinSocketSystem g_winsocketsystem;