
#include "stdafx.h"
#include "TCPClient.h"


TCPClient::TCPClient(const char* p_IP,u_int i_Port,SOCKET serverSocket)
{
	memset( m_cbRecvBuf, 0, sizeof(m_cbRecvBuf) );
	m_nRecvSize = 0;

	// 创建套接字
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cout << "创建套接字失败!" << std::endl;
		return;
	}

	

	//lookup host to get address information
	if ((server = gethostbyname(SERVER_IP)) == NULL) {
		fprintf(stderr, "Host lookup failed for '%s'.\n", SERVER_IP);
		exit(0);
	}
	//make sure serv_addr is cleared and then set values for each member
	memset(&mServerAddr, 0, sizeof(mServerAddr));
	// 填充服务器的IP和端口号
	mServerAddr.sin_family = AF_INET;
	mServerAddr.sin_addr.s_addr = inet_addr(p_IP);
	mServerAddr.sin_port = htons((u_short)i_Port);

	// 连接到服务器
	if ( ::connect(serverSocket, (struct sockaddr*)&mServerAddr, sizeof(mServerAddr)))
	{
		::closesocket(serverSocket);
		std::cout << "连接服务器失败!" << std::endl;
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
		// 接收数据
		int nRecvSize = ::recv(mServerSocket,
			m_cbRecvBuf+m_nRecvSize, 
			sizeof(m_cbRecvBuf)-m_nRecvSize, 0);

		if (nRecvSize <= 0)
		{
			std::cout << "服务器主动断开连接!" << std::endl;
			return;
		}

		// 保存已经接收数据的大小
		m_nRecvSize += nRecvSize;

		// 接收到的数据够不够一个包头的长度
		/*while (m_nRecvSize >= sizeof(NetPacketHeader))
		{*/
			// 收够5个包，主动与服务器断开
			/*if (nCount >= 5)
			{
				::closesocket(mServerSocket);
				return;
			}*/

			// 读取包头
			NetPacketHeader* pHead = (NetPacketHeader*) (m_cbRecvBuf);
			const unsigned short nPacketSize = pHead->wDataSize;

			//// 判断是否已接收到足够一个完整包的数据
			//if (m_nRecvSize < nPacketSize)
			//{
			//	// 还不够拼凑出一个完整包
			//	break;
			//}

			// 拷贝到数据缓存
			CopyMemory(m_cbDataBuf, m_cbRecvBuf, nPacketSize);

			// 从接收缓存移除
			MoveMemory(m_cbRecvBuf, m_cbRecvBuf+nPacketSize, m_nRecvSize);
			m_nRecvSize -= nPacketSize;

			// 解密数据，以下省略一万字
			// ...

			// 分派数据包，让应用层进行逻辑处理
			pHead = (NetPacketHeader*) (m_cbDataBuf);
			const unsigned short nDataSize = nPacketSize - (unsigned short)sizeof(NetPacketHeader);
			OnNetMessage(pHead->wOpcode, m_cbDataBuf+sizeof(NetPacketHeader), nDataSize);
			////发送消息
			send(mServerSocket, sendBuf, bufLen + 1, 0);

			++nCount;
		}
	//}

	std::cout << "已经和服务器断开连接!" << std::endl;
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
			std::cout << "收取到未知网络数据包:" << nOpcode << std::endl;
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
