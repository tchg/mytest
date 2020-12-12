#include "IOCPSocket.h"



CIOCPSocket::CIOCPSocket()
{
}


CIOCPSocket::~CIOCPSocket()
{
}

BOOL CIOCPSocket::CreateIocp(char* szIp, unsigned short nPort)
{
	// 	1) ����socket
	m_sockServer = socket(
		AF_INET,
		SOCK_STREAM, //��ʽ 
		IPPROTO_TCP);//tcpЭ��

	// 	2) �󶨶˿�
	sockaddr_in siServer;
	siServer.sin_family = AF_INET;
	siServer.sin_port = htons(nPort);
	siServer.sin_addr.S_un.S_addr = inet_addr(szIp);
	int nRet = bind(m_sockServer, (sockaddr*)&siServer, sizeof(siServer));
	if (nRet == SOCKET_ERROR)
	{
		return FALSE;
	}

	// 	3) ����
	nRet = listen(m_sockServer, SOMAXCONN);
	if (nRet == SOCKET_ERROR)
	{
		return FALSE;
	}

	//����IOCP����
	m_hIOCP = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		NULL,
		NULL, //�Զ������
		0);//�Զ������̵߳ĸ���, �����˫��,����4�߳�, ����ǵ���,����˫�߳�
	if (m_hIOCP == NULL)
	{
		return FALSE;
	}

	//��socketע���iocp, ����socket��iocp
	HANDLE hRet = CreateIoCompletionPort(
		(HANDLE)m_sockServer,
		m_hIOCP,
		NULL,
		0);

	return TRUE;
}


//��iocp������Ͷ��һ���������ӵ�����
BOOL CIOCPSocket::PostAccept()
{
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SocketInfo* pClientInfo = new SocketInfo(sockClient);

	char szBuff[MAXWORD] = { 0 };
	DWORD dwBytesRet = 0;
	OVERLAPPED* pOverlapped = new OVERLAPPED(pClientInfo, IO_ACCEPT);
	BOOL bRet = AcceptEx(
		m_sockServer, //����������socket
		sockClient, //δ���Ϳͻ��˽��������õ�, 
		szBuff,
		0, //��iocp�����ӽ�����֮������֪ͨ����, ���ý��տͻ��˷��ĵ�һ����
		sizeof(sockaddr_in) + 16,//MSDNҪ�����16
		sizeof(sockaddr_in) + 16,//MSDNҪ�����16
		&dwBytesRet,
		(OVERLAPPED*)pOverlapped); //ÿ��������һ������ṹ��

	HANDLE hRet = CreateIoCompletionPort(
		(HANDLE)sockClient,
		m_hIOCP,
		NULL,
		0);


	return bRet;
}

//�������Ͷ��һ���������ݵ�����
BOOL CIOCPSocket::PostRecv(SocketInfo* pSocketInfo)
{
	//���ڽ������ݵĻ�����
	VERLAPPED* pOverlapped = new OVERLAPPED(pSocketInfo, IO_RECV);
	pOverlapped->m_wsabuffForRecv.buf = pOverlapped->m_szBuffRecvTmp;
	pOverlapped->m_wsabuffForRecv.len = MAXWORD;

	DWORD dwFlag = 0;
	int nRet = WSARecv(
		pSocketInfo->m_sock,
		&pOverlapped->m_wsabuffForRecv,
		1, //��������WSABUF�ĸ���, һ��ֻ��һ��
		0,
		&dwFlag,
		(OVERLAPPED*)pOverlapped,
		NULL);
	if (nRet == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	{
		return TRUE;
	}
	if (nRet == 0)
	{
		return TRUE;
	}
	return FALSE;
}

int CIOCPSocket::WaitForIoEvent(SocketInfo*& pSocketInfo)
{
	DWORD dwBytesRecv = 0; //����Ƕ�����,�򷵻ض�ȡ���ݵĸ���
	CR36OVERLAPPED* pOverlapped = NULL;
	ULONG_PTR pCompleteKey = NULL;
	BOOL bRet = GetQueuedCompletionStatus(
		m_hIOCP,
		&dwBytesRecv,
		&pCompleteKey,
		(LPOVERLAPPED*)&pOverlapped,
		INFINITE
	);
	pSocketInfo = pOverlapped->m_pSocketInfo;
	int nRetIoEvent = pOverlapped->m_ioEvent;

	switch (pOverlapped->m_ioEvent)
	{
	case IO_ACCEPT:
		PostRecv(pOverlapped->m_pSocketInfo);

		//�����������Ͷ�ݽ������ӵ�����,�������Ӻ���Ŀͻ���
		PostAccept();
		break;
	case IO_RECV:
	{
		//�յ������ݷ�����ջ�������
		pOverlapped->m_pSocketInfo->m_buffRecv.Write(
			pOverlapped->m_szBuffRecvTmp,
			dwBytesRecv);

		//�ٶ�һ�������ݵ�����
		PostRecv(pOverlapped->m_pSocketInfo);
		break;
	}
	case IO_SEND:
	{
		//����������֮��, �������Ͷ��һ���������ݵ�����,�������պ��������
		PostSend(pOverlapped->m_pSocketInfo);
		break;
	}
	default:
		break;
	}

	delete pOverlapped;

	return nRetIoEvent;
}

BOOL CIOCPSocket::PostSend(SocketInfo* pSocketInfo)
{
	//�����������û������, �����������Ͷ�ݷ������ݵ�����
	pSocketInfo->m_lockSend.Lock();
	if (pSocketInfo->m_buffSend.GetSize() <= 0)
	{
		pSocketInfo->m_lockSend.UnLock();
		return FALSE;
	}
	pSocketInfo->m_lockSend.UnLock();


	//���ڷ������ݵĻ�����
	CR36OVERLAPPED* pOverlapped = new CR36OVERLAPPED(pSocketInfo, IO_SEND);

	//�ӷ��ͻ�������ȡ������,�����ݷ��ͳ�ȥ
	pSocketInfo->m_lockSend.Lock();
	pOverlapped->m_wsabuffForSend.buf = new char[pSocketInfo->m_buffSend.GetSize()];
	pOverlapped->m_wsabuffForSend.len = pSocketInfo->m_buffSend.GetSize();
	pSocketInfo->m_buffSend.Read(pOverlapped->m_wsabuffForSend.buf, pOverlapped->m_wsabuffForSend.len);
	pSocketInfo->m_lockSend.UnLock();

	int nRet = WSASend(
		pSocketInfo->m_sock,
		&pOverlapped->m_wsabuffForSend,
		1, //��������WSABUF�ĸ���, һ��ֻ��һ��
		NULL,
		0,
		(OVERLAPPED*)pOverlapped,
		NULL);
	if (nRet == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	{
		return TRUE;
	}
	if (nRet == 0)
	{
		return TRUE;
	}
	return FALSE;
}
