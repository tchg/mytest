#include "UdpSocket.h"


CUdpSocket::CUdpSocket()
{
}


CUdpSocket::~CUdpSocket()
{
}

BOOL CUdpSocket::CreateSocket(char* szIp, u_short uPort /*= 0*/)
{
	m_sockServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sockServer == INVALID_SOCKET)
	{
		return FALSE;
	}

	sockaddr_in siServer;
	siServer.sin_family = AF_INET;
	siServer.sin_port = htons(uPort);
	siServer.sin_addr.S_un.S_addr = inet_addr(szIp);
	if (bind(m_sockServer, (sockaddr*)&siServer, sizeof(siServer)) == SOCKET_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}

void CUdpSocket::CloseSocket()
{
	closesocket(m_sockServer);
	m_sockServer = INVALID_SOCKET;
}

int CUdpSocket::RecvFrom(char* pBuff, int nSize, sockaddr_in* psi)
{
	int nLenOfSi = sizeof(sockaddr_in);
	return recvfrom(m_sockServer, pBuff, nSize, 0, (sockaddr*)psi, &nLenOfSi);
}

int CUdpSocket::SendTo(char* pBuff, int nSize, sockaddr_in* psi)
{
	return sendto(m_sockServer, pBuff, nSize, 0, (sockaddr*)psi, sizeof(sockaddr_in));
}

BOOL CUdpSocket::RecvPackage(DATAPACKAGE* pPackage, sockaddr_in* psi)
{
	//先收数据长度
	int nRet = RecvFrom((char*)&pPackage->m_hdr, sizeof(pPackage->m_hdr), psi);
	//check ...
	if (nRet == 0 || nRet == SOCKET_ERROR)
	{
		return FALSE;
	}

	//再接受数据
	if (pPackage->m_hdr.m_nDataLen != 0)
	{
		pPackage->m_pBuff = new char[pPackage->m_hdr.m_nDataLen];
		nRet = RecvFrom(pPackage->m_pBuff, pPackage->m_hdr.m_nDataLen, psi);
		if (nRet <= 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CUdpSocket::SendPackage(DATAPACKAGE* pPackage, sockaddr_in* psi)
{
	int nRet = SendTo((char*)&pPackage->m_hdr, sizeof(pPackage->m_hdr), psi);
	if (nRet == SOCKET_ERROR)
	{
		return FALSE;
	}

	if (pPackage->m_hdr.m_nDataLen != 0)
	{
		nRet = SendTo(pPackage->m_pBuff, pPackage->m_hdr.m_nDataLen, psi);
		if (nRet == SOCKET_ERROR)
		{
			return FALSE;
		}
	}
	return TRUE;
}
