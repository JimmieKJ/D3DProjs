
#pragma once


#define NET_PACKET_DATA_SIZE 1024 
#define NET_PACKET_SIZE (sizeof(NetPacketHeader) + NET_PACKET_DATA_SIZE) * 10

#define SERVER_PORT		20003
#define SERVER_IP		"127.0.0.1"
#define BUFFER_SIZE		255
sockaddr_in			mServerAddr;	///< ��������ַ
/// �������ݰ���ͷ
struct NetPacketHeader
{
	unsigned short		wDataSize;	///< ���ݰ���С���������ͷ�ͷ�����ݴ�С
	unsigned short		wOpcode;	///< ������
};

/// �������ݰ�
struct NetPacket
{
	NetPacketHeader		Header;							///< ��ͷ
	unsigned char		Data[NET_PACKET_DATA_SIZE];		///< ����
};



//////////////////////////////////////////////////////////////////////////


/// ���������
enum eNetOpcode
{
	NET_TEST1			= 1,
};
static SOCKET mServerSocket;
/// ����1���������ݰ�����
struct NetPacket_Test1
{
	int		nIndex;
	char name[20];
	char sex[20];
	int age;
	char	arrMessage[512];
};

