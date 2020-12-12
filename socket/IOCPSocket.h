#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <Mswsock.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

#include "ByteStreamBuff.h"
#include "CLock.h"

class CIOCPSocket
{
public:
	enum IO_EVENT
	{
		IO_ACCEPT,
		IO_RECV,
		IO_SEND
	};

	//���������ϵĿͻ���
	struct SocketInfo
	{
		SOCKET m_sock;  //��Ӧ���ӿͻ��ε�socket

		CLock m_lockRecv; //ͬ��,����m_buffRecv
		CByteStreamBuff m_buffRecv;  //��ȡ���ݵĻ�����, iocp�յ������ݶ��ŵ�����

		CLock m_lockSend; //ͬ��, ����m_buffSend
		CByteStreamBuff m_buffSend; //д�����ݵĻ�����, iocp���������ݶ���������

		SocketInfo(SOCKET sock) :m_sock(sock){	}
	};


	struct OVERLAPPED
	{
		OVERLAPPED m_overlapped;
		DWORD  m_ioEvent;		   //���������
		SocketInfo* m_pSocketInfo; //����ͻ��������Ϣ

		//��ʱ������,���յ�������iocp��ŵ�����, Ȼ����������ݻ�ŵ��������м仺����
		char   m_szBuffRecvTmp[MAXWORD];
		WSABUF m_wsabuffForRecv; //�������ݵĻ�����

		WSABUF m_wsabuffForSend; //�������ݵĻ�����

		OVERLAPPED(SocketInfo* pSocketInfo, DWORD ioEvent) :
			m_pSocketInfo(pSocketInfo), m_ioEvent(ioEvent)
		{
			memset(&m_overlapped, 0, sizeof(m_overlapped));
			m_wsabuffForSend.buf = NULL;
			m_wsabuffForSend.len = 0;
		}
		~OVERLAPPED()
		{
			if (m_wsabuffForSend.buf != NULL)
			{
				delete m_wsabuffForSend.buf;
			}

		}
	};

public:
	CIOCPSocket();
	~CIOCPSocket();

	BOOL CreateIocp(char* szIp, unsigned short nPort);
	BOOL PostAccept();
	BOOL PostSend(SocketInfo* pSocketInfo);
	BOOL PostRecv(SocketInfo* pSocketInfo);
	int WaitForIoEvent(SocketInfo*& pSocketInfo);

private:
	SOCKET m_sockServer;
	HANDLE m_hIOCP;
};

