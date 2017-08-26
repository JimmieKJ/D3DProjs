#include "MySerialPort.h"
#include <tchar.h>

// 不使用读超时
#define TIMEOUT_READ_INTERVAL			0xFFFFFFFF
#define TIMEOUT_READ_TOTAL_MULTIPLIER	0
#define TIMEOUT_READ_TOTAL_CONSTANT		0
// 写超时为秒
#define TIMEOUT_WRITE_TOTAL_MULTIPLIER	0
#define TIMEOUT_WRITE_TOTAL_CONSTANT	5000

// 推荐的输入/输出缓冲区（注意：实际值由系统设置）
#define BUFFER_INPUT_RECOMMEND	10000
#define BUFFER_OUTPUT_RECOMMEND	10000

// 异步读取/写入操作时等待事件的超时时间
#define TIMEOUT_READCOMM_EVENT	4000
#define TIMEOUT_WRITECOMM_EVENT	2000

// 一些通讯协议使用的宏
#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

jeff_class::CMySerialPort::CMySerialPort()
{
	memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));
	m_hIDComDev = NULL;
	m_bOpened = FALSE;
}

jeff_class::CMySerialPort::~CMySerialPort()
{
	Close();
}

// 打开串口
BOOL jeff_class::CMySerialPort::Open(int nPort, int nBaud)
{
	if (m_bOpened)
		return TRUE;

	TCHAR szPort[50];
	TCHAR szComParams[50];
	DCB dcb;

	wsprintf(szPort, _T("COM%d"), nPort);
	// API：建立文件，Windows中将串口设备当做文件对待
	m_hIDComDev = CreateFile(
		szPort,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,	// 异步读写
		NULL
		);

	if (m_hIDComDev == NULL) return (FALSE);

	memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));

	// 设置超时
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = TIMEOUT_READ_INTERVAL;
	CommTimeOuts.ReadTotalTimeoutMultiplier = TIMEOUT_READ_TOTAL_MULTIPLIER;
	CommTimeOuts.ReadTotalTimeoutConstant = TIMEOUT_READ_TOTAL_CONSTANT;
	CommTimeOuts.WriteTotalTimeoutMultiplier = TIMEOUT_WRITE_TOTAL_MULTIPLIER;
	CommTimeOuts.WriteTotalTimeoutConstant = TIMEOUT_WRITE_TOTAL_CONSTANT;
	SetCommTimeouts(m_hIDComDev, &CommTimeOuts);

	wsprintf(szComParams, _T("COM%d:%d,n,8,1"), nPort, nBaud);

	// 设置异步读取/写入监视事件
	m_OverlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// 读取/设置串口设备参数
	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hIDComDev, &dcb);
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	unsigned char ucSet;
	ucSet = (unsigned char)((FC_RTSCTS & FC_DTRDSR) != 0);
	ucSet = (unsigned char)((FC_RTSCTS & FC_RTSCTS) != 0);
	ucSet = (unsigned char)((FC_RTSCTS & FC_XONXOFF) != 0);

	if (!SetCommState(m_hIDComDev, &dcb) ||
		!SetupComm(m_hIDComDev, BUFFER_INPUT_RECOMMEND, BUFFER_OUTPUT_RECOMMEND) ||
		m_OverlappedRead.hEvent == NULL ||
		m_OverlappedWrite.hEvent == NULL) {
		DWORD dwError = GetLastError();

		if (m_OverlappedRead.hEvent != NULL) CloseHandle(m_OverlappedRead.hEvent);

		if (m_OverlappedWrite.hEvent != NULL) CloseHandle(m_OverlappedWrite.hEvent);

		CloseHandle(m_hIDComDev);
		return (FALSE);
	}

	m_bOpened = TRUE;

	return (m_bOpened);
}

// 关闭串口
BOOL jeff_class::CMySerialPort::Close(void)
{
	if (!m_bOpened || m_hIDComDev == NULL) return (TRUE);

	if (m_OverlappedRead.hEvent != NULL) CloseHandle(m_OverlappedRead.hEvent);

	if (m_OverlappedWrite.hEvent != NULL) CloseHandle(m_OverlappedWrite.hEvent);

	CloseHandle(m_hIDComDev);
	m_bOpened = FALSE;
	m_hIDComDev = NULL;

	return (TRUE);
}

// 向串口写入数据，类内部使用
BOOL jeff_class::CMySerialPort::WriteCommByte(unsigned char ucByte)
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile(m_hIDComDev, (LPSTR)&ucByte, 1, &dwBytesWritten, &m_OverlappedWrite);

	// 查询异步写入是否完成，未完成则挂起等待
	if (!bWriteStat && (GetLastError() == ERROR_IO_PENDING)) {
		if (WaitForSingleObject(m_OverlappedWrite.hEvent, TIMEOUT_WRITECOMM_EVENT))
			dwBytesWritten = 0;
		else {
			GetOverlappedResult(m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE);
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}

	return (TRUE);
}

// 向串口发送数据
int jeff_class::CMySerialPort::SendData(const char *buffer, int size)
{
	if (!m_bOpened || m_hIDComDev == NULL) return (0);

	DWORD dwBytesWritten = 0;
	int i;

	for (i = 0; i<size; i++) {
		WriteCommByte(buffer[i]);
		dwBytesWritten++;
	}

	return ((int)dwBytesWritten);
}

// 查询接受缓冲区内是否有数据（只查询，不读取）
int jeff_class::CMySerialPort::ReadDataWaiting(void)
{
	if (!m_bOpened || m_hIDComDev == NULL) return (0);

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);

	return ((int)ComStat.cbInQue);
}

// 读取来自串口的数据
int jeff_class::CMySerialPort::ReadData(void *buffer, int limit)
{

	if (!m_bOpened || m_hIDComDev == NULL) return (0);

	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	// 读取之前须清楚错误信息
	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);

	if (!ComStat.cbInQue) return (0);

	dwBytesRead = (DWORD)ComStat.cbInQue;

	if (limit < (int)dwBytesRead) dwBytesRead = (DWORD)limit;

	bReadStatus = ReadFile(m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, &m_OverlappedRead);

	// 查询异步读取是否完成，未完成则挂起等待
	if (!bReadStatus) {
		if (GetLastError() == ERROR_IO_PENDING) {
			WaitForSingleObject(m_OverlappedRead.hEvent, TIMEOUT_READCOMM_EVENT);
			return ((int)dwBytesRead);
		}

		return (0);
	}

	return ((int)dwBytesRead);
}
