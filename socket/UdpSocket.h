#pragma once
#include <windows.h>
#include "comman.h"

typedef struct tagDataPackage
{
	PACKAGEHEADER m_hdr;	//数据包的包头
	char* m_pBuff;			//数据包的包体
}DATAPACKAGE, *PDATAPACKAGE;

class CUdpSocket
{
public:
	CUdpSocket();
	~CUdpSocket();

	/*
	参数:
	  szIp  - IP地址
	  uport - 端口号, 默认是随机端口
	*/
	BOOL CreateSocket(char* szIp, u_short uPort = 0);
	void CloseSocket();

	int RecvFrom(char* pBuff, int nSize, sockaddr_in* psi);
	int SendTo(char* pBuff, int nSize, sockaddr_in* psi);

	BOOL RecvPackage(DATAPACKAGE* pPackage, sockaddr_in* psi);
	BOOL SendPackage(DATAPACKAGE* pPackage, sockaddr_in* psi);

private:
	SOCKET m_sockServer = INVALID_SOCKET;
};

