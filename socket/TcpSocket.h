#pragma once
#include <windows.h>
#include "comman.h"

class CTcpSocket
{
public:
	CTcpSocket();
	~CTcpSocket();

	BOOL CreateSocket();
	BOOL BindListen(char* szIp, u_short nPort);
	BOOL Accept(CTcpSocket* pTcpSocket );
	BOOL Connect(const char* szIp, u_short nPort);
	BOOL Recv(char* pBuff, int* pLen/*传入传出, 传出收到的字节数*/);
	BOOL Send(char* pBuff, int* pLen/*传入传出, 传出发送的字节数*/);


	BOOL RecvPackage(DATAPACKAGE* pPackage);
	BOOL SendPackage(DATAPACKAGE* pPackage);


	void CloseSocket();

	const sockaddr_in& GetSockaddrIn()const;
private:
	SOCKET m_socket;
	sockaddr_in m_si;
};

