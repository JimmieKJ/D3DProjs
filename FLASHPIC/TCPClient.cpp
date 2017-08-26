
#include "stdafx.h"
#include "TCPClient.h"


TCPClient::TCPClient(const char* p_IP,u_int i_Port,SOCKET serverSocket)
{
	memset( m_cbRecvBuf, 0, sizeof(m_cbRecvBuf) );
	m_nRecvSize = 0;

	// �����׽���
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cout << "�����׽���ʧ��!" << std::endl;
		return;
	}

	

	//lookup host to get address information
	if ((server = gethostbyname(SERVER_IP)) == NULL) {
		fprintf(stderr, "Host lookup failed for '%s'.\n", SERVER_IP);
		exit(0);
	}
	//make sure serv_addr is cleared and then set values for each member
	memset(&mServerAddr, 0, sizeof(mServerAddr));
	// ����������IP�Ͷ˿ں�
	mServerAddr.sin_family = AF_INET;
	mServerAddr.sin_addr.s_addr = inet_addr(p_IP);
	mServerAddr.sin_port = htons((u_short)i_Port);

	// ���ӵ�������
	if ( ::connect(serverSocket, (struct sockaddr*)&mServerAddr, sizeof(mServerAddr)))
	{
		::closesocket(serverSocket);
		std::cout << "���ӷ�����ʧ��!" << std::endl;
		return;	
	}
}

TCPClient::~TCPClient()
{
	//::closesocket(mServerSocket);
}

void TCPClient::run(char* sendBuf,int bufLen)
{
	int nCount = 0;
	for (;;)
	{
		// ��������
		int nRecvSize = ::recv(mServerSocket,
			m_cbRecvBuf+m_nRecvSize, 
			sizeof(m_cbRecvBuf)-m_nRecvSize, 0);

		if (nRecvSize <= 0)
		{
			std::cout << "�����������Ͽ�����!" << std::endl;
			return;
		}

		// �����Ѿ��������ݵĴ�С
		m_nRecvSize += nRecvSize;

		// ���յ������ݹ�����һ����ͷ�ĳ���
		/*while (m_nRecvSize >= sizeof(NetPacketHeader))
		{*/
			// �չ�5������������������Ͽ�
			/*if (nCount >= 5)
			{
				::closesocket(mServerSocket);
				return;
			}*/

			// ��ȡ��ͷ
			NetPacketHeader* pHead = (NetPacketHeader*) (m_cbRecvBuf);
			const unsigned short nPacketSize = pHead->wDataSize;

			//// �ж��Ƿ��ѽ��յ��㹻һ��������������
			//if (m_nRecvSize < nPacketSize)
			//{
			//	// ������ƴ�ճ�һ��������
			//	break;
			//}

			// ���������ݻ���
			CopyMemory(m_cbDataBuf, m_cbRecvBuf, nPacketSize);

			// �ӽ��ջ����Ƴ�
			MoveMemory(m_cbRecvBuf, m_cbRecvBuf+nPacketSize, m_nRecvSize);
			m_nRecvSize -= nPacketSize;

			// �������ݣ�����ʡ��һ����
			// ...

			// �������ݰ�����Ӧ�ò�����߼�����
			pHead = (NetPacketHeader*) (m_cbDataBuf);
			const unsigned short nDataSize = nPacketSize - (unsigned short)sizeof(NetPacketHeader);
			OnNetMessage(pHead->wOpcode, m_cbDataBuf+sizeof(NetPacketHeader), nDataSize);
			////������Ϣ
			send(mServerSocket, sendBuf, bufLen + 1, 0);

			++nCount;
		}
	//}

	std::cout << "�Ѿ��ͷ������Ͽ�����!" << std::endl;
}

bool TCPClient::OnNetMessage( const unsigned short& nOpcode, 
							 const char* pDataBuffer, unsigned short nDataSize )
{
	/*switch (nOpcode)
	{
	case NET_TEST1:
		{*/
			char* pMsg = (char*) pDataBuffer;
			return OnNetPacket(pMsg);
		/*}
		break;

	default:
		{
			std::cout << "��ȡ��δ֪�������ݰ�:" << nOpcode << std::endl;
			return false;
		}
		break;
	}*/
}

bool TCPClient::OnNetPacket( char* pMsg )
{
	std::cout << pMsg<< std::endl;
	return true;
}
