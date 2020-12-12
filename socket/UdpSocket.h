#pragma once
#include <windows.h>
#include "comman.h"

typedef struct tagDataPackage
{
	PACKAGEHEADER m_hdr;	//���ݰ��İ�ͷ
	char* m_pBuff;			//���ݰ��İ���
}DATAPACKAGE, *PDATAPACKAGE;

class CUdpSocket
{
public:
	CUdpSocket();
	~CUdpSocket();

	/*
	����:
	  szIp  - IP��ַ
	  uport - �˿ں�, Ĭ��������˿�
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

