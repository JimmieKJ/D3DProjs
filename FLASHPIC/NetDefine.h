
#pragma once


#define NET_PACKET_DATA_SIZE 1024 
#define NET_PACKET_SIZE (sizeof(NetPacketHeader) + NET_PACKET_DATA_SIZE) * 10

#define SERVER_PORT		20003
#define SERVER_IP		"127.0.0.1"
#define BUFFER_SIZE		255
sockaddr_in			mServerAddr;	///< 服务器地址
/// 网络数据包包头
struct NetPacketHeader
{
	unsigned short		wDataSize;	///< 数据包大小，包含封包头和封包数据大小
	unsigned short		wOpcode;	///< 操作码
};

/// 网络数据包
struct NetPacket
{
	NetPacketHeader		Header;							///< 包头
	unsigned char		Data[NET_PACKET_DATA_SIZE];		///< 数据
};



//////////////////////////////////////////////////////////////////////////


/// 网络操作码
enum eNetOpcode
{
	NET_TEST1			= 1,
};
static SOCKET mServerSocket;
/// 测试1的网络数据包定义
struct NetPacket_Test1
{
	int		nIndex;
	char name[20];
	char sex[20];
	int age;
	char	arrMessage[512];
};

