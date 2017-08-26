#pragma once

class TCPClient
{
public:
	TCPClient(const char* p_IP, u_int i_Port, SOCKET serverSocket);
	virtual ~TCPClient();

public:
	/// 主循环
	void run(char* sendBuf, int bufLen);

	/// 处理网络消息
	bool OnNetMessage(const unsigned short& nOpcode, 
		const char* pDataBuffer, unsigned short nDataSize);

	bool OnNetPacket(char* pMsg);

private:
	SOCKET				mServerSocket;	///< 服务器套接字句柄
	sockaddr_in			mServerAddr;	///< 服务器地址

	char				m_cbRecvBuf[NET_PACKET_SIZE * 10];
	char				m_cbDataBuf[NET_PACKET_SIZE * 10];
	int					m_nRecvSize;
	struct hostent *server; // server information
};
