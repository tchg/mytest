// testNtProcess.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include "MyNtHeader.h"


NTQUERYSYSTEMINFORMATION g_NtQuerySystemInformation;

BOOL GetModuleInformation(void ** ModuleInfo)
{

    DWORD dwSize = 0;
    void* buf = NULL;
    
    if (g_NtQuerySystemInformation)
    {
        g_NtQuerySystemInformation(SystemModuleInformation, &buf, 0, &dwSize);
        if (dwSize)
        {
            dwSize += 4096;
            buf = new char[dwSize] {0};
            if (buf)
            {
                g_NtQuerySystemInformation(SystemModuleInformation, buf, dwSize, 0);
                if (*(DWORD*)buf)
                {
                    *ModuleInfo = buf;
                    return TRUE;
                }
                delete[] buf;
            }
        }
    }
    return FALSE;
}

typedef struct MyIdt
{
    unsigned low : 16;
    //unsigned selector : 16;
    unsigned rpl : 2;
    unsigned tl : 1;
    unsigned index : 9;
    unsigned res : 8;
    unsigned type : 4;
    unsigned s : 1;
    unsigned dpl : 2;
    unsigned p : 1;
    unsigned hei : 16;
}MYIDT, * PMYIDT;

typedef struct _IDTENTRY
{
    unsigned short LowOffset;
    unsigned short selector;
    unsigned char retention : 5;
    unsigned char zero1 : 3;
    unsigned char gate_type : 1;
    unsigned char zero2 : 1;
    unsigned char interrupt_gate_size : 1;
    unsigned char zero3 : 1;
    unsigned char zero4 : 1;
    unsigned char DPL : 2;
    unsigned char P : 1;
    unsigned short HiOffset;
} IDTENTRY, * PIDTENTRY;

int main()
{
    std::cout << "Hello World!\n";

    HMODULE hmodule = GetModuleHandle(L"Ntdll.dll");
    g_NtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress(hmodule, "NtQuerySystemInformation");


    //获取进程模块
    /*void* buf = NULL;
    if (GetModuleInformation(&buf))
    {
        PRTL_PROCESS_MODULES Modules = (PRTL_PROCESS_MODULES)buf;
        PRTL_PROCESS_MODULE_INFORMATION ModuleInfo;
        int i;
        for (i = 0, ModuleInfo = &(Modules->Modules[0]);
            i < Modules->NumberOfModules;
            i++, ModuleInfo++) {
            printf("[HookSSDT] ImageBase=%p ImageSize=%p FullPathName=%s\n",
                Modules->Modules[i].ImageBase,
                Modules->Modules[i].ImageSize,
                Modules->Modules[i].FullPathName);
        }
    }*/

    //提升程序权限
    //BOOL v0 = false;
    //HANDLE hand = GetCurrentProcess();
    //HANDLE TokenHandle = NULL;
    //TOKEN_PRIVILEGES NewState{ 0 };
    //if (OpenProcessToken(hand, 0x28, &TokenHandle))
    //{
    //    if (LookupPrivilegeValueW(0, L"SeLoadDriverPrivilege", &NewState.Privileges[0].Luid))
    //    {
    //        NewState.PrivilegeCount = 1;
    //        NewState.Privileges[0].Attributes = 2;
    //        if (AdjustTokenPrivileges(TokenHandle, 0, &NewState, sizeof(NewState), 0, 0))
    //        {
    //            v0 = true;
    //            TOKEN_ELEVATION tokenEle;
    //            DWORD dwRetLen = 0;

    //            // Retrieve token elevation information
    //            if (GetTokenInformation(TokenHandle, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen))
    //            {
    //                if (dwRetLen == sizeof(tokenEle))
    //                {
    //                    tokenEle.TokenIsElevated;
    //                }
    //            }
    //        }
    //    }
    //}
    int buf[] = { 0x000831a0, 0x80548e00 };
    int buf2[] = { 0x0058113e, 0x00008500 };
    PMYIDT z = (PMYIDT)buf;
    PMYIDT z2 = (PMYIDT)buf2;
}

