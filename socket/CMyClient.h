#pragma once
#include "pch.h"
#include "TcpSocket.h"
#include "CJsonObject.hpp"
#include <Tlhelp32.h>

class CMyClient
{
public:
    CMyClient();
    ~CMyClient();

private:
    typedef struct tagNodeInof
    {
        HKEY m_hMainKey; //主键
        CString m_strSubKeyPath;

        tagNodeInof(HKEY hMainKey, CString strSubKeyPath) :
            m_hMainKey(hMainKey),
            m_strSubKeyPath(strSubKeyPath)
        {		}
        
    }NODEINFO, * PNODEINFO;

    //数据类型
    CString m_szRegType[12] = {
        "REG_NONE",
        "REG_SZ",
        "REG_EXPAND_SZ",
        "REG_BINARY",
        "REG_DWORD",
        "REG_DWORD_BIG_ENDIAN",
        "REG_LINK",
        "REG_MULTI_SZ",
        "REG_RESOURCE_LIST",
        "REG_FULL_RESOURCE_DESCRIPTOR",
        "REG_RESOURCE_REQUIREMENTS_LIST",
        "REG_QWORD"
    };
    CString RegDataToStr(BYTE* lpData, DWORD lpcbData, int nID);

public:
    HICON m_hIcon;
    HANDLE m_hParentRead;
    HANDLE m_hParentWrite;
    HANDLE m_hChildRead;
    HANDLE m_hChildWrite;
    HANDLE m_hThread = nullptr;
    HANDLE m_OuthThread = nullptr;
    static DWORD WINAPI CmdThreadProc(LPVOID lpParameter);
    
    HANDLE m_hFile;
    HANDLE m_hFileMap;


    CTcpSocket m_SockClient;
    sockaddr_in m_siServer;//服务器地址

    BOOL ConnectServer();
    DWORD RecvThread();

    void SendSeeDisk();
    void SendSeeFile(CString& filepath);
    void SendSeeProcess();
    void SendSeeThread(DWORD dwpid);
    void SendStopThread(DWORD pid);
    void SendSeeRegedit(neb::CJsonObject jsonGet);
    void QueryKey(HKEY hKey, HTREEITEM hParentItem, PNODEINFO pParentNodInfo);
    void EnumValues(HKEY hKey, int nValueCount);
    void SendAddRegedit(neb::CJsonObject jsonGet);
    void SendAddRegeditCl(neb::CJsonObject jsonGet);
    void SendDelRegedit(neb::CJsonObject jsonGet);
    void SendSeeCmd(CString cmd);
    void InitCmd();
    void WriteToCmd(CString result);
    void SendDelRegeditValue(neb::CJsonObject jsonGet);
    void SendUpFileOk(neb::CJsonObject jsonGet);
    void SendDownFileOk(neb::CJsonObject jsonGet);
    void SendUpFile(neb::CJsonObject jsonGet);
    void SendDownFile(neb::CJsonObject jsonGet);
};

