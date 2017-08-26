#pragma once

class TCPClient
{
public:
	TCPClient(const char* p_IP, u_int i_Port, SOCKET serverSocket);
	virtual ~TCPClient();

public:
	/// ��ѭ��
	void run(char* sendBuf, int bufLen);

	/// ����������Ϣ
	bool OnNetMessage(const unsigned short& nOpcode, 
		const char* pDataBuffer, unsigned short nDataSize);

	bool OnNetPacket(char* pMsg);

private:
	SOCKET				mServerSocket;	///< �������׽��־��
	sockaddr_in			mServerAddr;	///< ��������ַ

	char				m_cbRecvBuf[NET_PACKET_SIZE * 10];
	char				m_cbDataBuf[NET_PACKET_SIZE * 10];
	int					m_nRecvSize;
	struct hostent *server; // server information
};
