
/*	MySerialPort.h
封装了串口通讯的windows API。
支持异步操作串口。
*/
#pragma once
#include <windows.h>

namespace jeff_class
{
	class CMySerialPort
	{
	public:
		CMySerialPort();
		~CMySerialPort();

		BOOL Open(int nPort = 2, int nBaud = 9600);
		BOOL Close(void);

		int ReadData(void *, int);
		int SendData(const char *, int);
		int ReadDataWaiting(void);	// 查询缓冲区内是否有未读取的数据

		BOOL IsOpened(void) {
			return (m_bOpened);
		}

	protected:
		BOOL WriteCommByte(unsigned char);	// 内部实现，向串口写数据

		HANDLE m_hIDComDev;
		OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
		BOOL m_bOpened;
	};
}