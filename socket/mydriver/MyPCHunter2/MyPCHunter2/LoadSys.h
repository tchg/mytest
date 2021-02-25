#pragma once
#include <windows.h>
#include <winsvc.h>
class LoadSys
{
public:
public:
    static BOOL InstallSvsA(LPCSTR szSvcName, LPCSTR szPath);
    static BOOL StartSvsA(LPCSTR szSvcName);
    static BOOL StopSvsA(LPCSTR szSvcName);
    static BOOL DeleteSvsA(LPCSTR szSvcName);
private:
    static BOOL StopDependentServices(SC_HANDLE schSCManager, SC_HANDLE schService);
};

