#include "pch.h"
#include "LoadSys.h"

BOOL LoadSys::InstallSvsA(LPCSTR szSvcName, LPCSTR szPath)
{
	SC_HANDLE schSCManager = OpenSCManagerA(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);
	if (schSCManager == NULL)
	{
		AfxMessageBox("MyError OpenSCManager failed (%d)\n", GetLastError());
		return FALSE;
	}


	// Create the service
	SC_HANDLE schService = CreateService(
		schSCManager,              // SCM database 
		szSvcName,                   // name of service 
		szSvcName,                   // service name to display 
		SERVICE_ALL_ACCESS,        // desired access 
		SERVICE_KERNEL_DRIVER, // service type 
		SERVICE_DEMAND_START,      // start type 
		SERVICE_ERROR_NORMAL,      // error control type 
		szPath,                    // path to service's binary 
		NULL,                      // no load ordering group 
		NULL,                      // no tag identifier 
		NULL,                      // no dependencies 
		NULL,                      // LocalSystem account 
		NULL);                     // no password 
	if (schService == NULL)
	{
		AfxMessageBox("MyError CreateService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	else
	{
		AfxMessageBox("MyError Service installed successfully\n");
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return TRUE;
}

BOOL LoadSys::StartSvsA(LPCSTR szSvcName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManagerW(
		NULL,                    // local computer
		NULL,                    // servicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		AfxMessageBox("MyError OpenSCManager failed(% d)\n", GetLastError());
		return FALSE;
	}

	// Get a handle to the service.
	schService = OpenServiceA(
		schSCManager,         // SCM database 
		szSvcName,            // name of service 
		SERVICE_ALL_ACCESS);  // full access 

	if (schService == NULL)
	{
		AfxMessageBox("MyError OpenService failed(% d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	if (!StartService(
		schService,  // handle to service 
		0,           // number of arguments 
		NULL))      // no arguments 
	{
		AfxMessageBox("MyError StartService failed(% d)\n", GetLastError());

		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	else
	{
		AfxMessageBox("MyError Service start pending...\n");
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return TRUE;
}

BOOL LoadSys::StopSvsA(LPCSTR szSvcName)
{
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwStartTime = GetTickCount();
	DWORD dwBytesNeeded;
	DWORD dwTimeout = 30000; // 30-second time-out
	DWORD dwWaitTime;
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	BOOL bRet = FALSE;
	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		AfxMessageBox("MyError OpenSCManager failed (%d)\n", GetLastError());
		return FALSE;
	}

	// Get a handle to the service.

	schService = OpenServiceA(
		schSCManager,         // SCM database 
		szSvcName,            // name of service 
		SERVICE_STOP |
		SERVICE_QUERY_STATUS |
		SERVICE_ENUMERATE_DEPENDENTS);

	if (schService == NULL)
	{
		AfxMessageBox("MyError OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	// Make sure the service is not already stopped.

	if (!QueryServiceStatusEx(
		schService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded))
	{
		AfxMessageBox("MyError QueryServiceStatusEx failed (%d)\n", GetLastError());
		goto stop_cleanup;
	}

	if (ssp.dwCurrentState == SERVICE_STOPPED)
	{
		AfxMessageBox("MyError Service is already stopped.\n");
		goto stop_cleanup;
	}

	// If a stop is pending, wait for it.

	while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
	{
		AfxMessageBox("MyError Service stop pending...\n");
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds. 

		dwWaitTime = ssp.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			schService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			AfxMessageBox("MyError QueryServiceStatusEx failed (%d)\n", GetLastError());
			goto stop_cleanup;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
		{
			AfxMessageBox("MyError Service stopped successfully.\n");
			bRet = TRUE;
			goto stop_cleanup;
		}

		if (GetTickCount() - dwStartTime > dwTimeout)
		{
			AfxMessageBox("MyError Service stop timed out.\n");
			goto stop_cleanup;
		}
	}

	// If the service is running, dependencies must be stopped first.

	StopDependentServices(schSCManager, schService);

	// Send a stop code to the service.

	if (!ControlService(
		schService,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp))
	{
		AfxMessageBox("MyError ControlService failed (%d)\n", GetLastError());
		goto stop_cleanup;
	}

	// Wait for the service to stop.

	while (ssp.dwCurrentState != SERVICE_STOPPED)
	{
		Sleep(ssp.dwWaitHint);
		if (!QueryServiceStatusEx(
			schService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			AfxMessageBox("MyError QueryServiceStatusEx failed (%d)\n", GetLastError());
			goto stop_cleanup;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
			break;

		if (GetTickCount() - dwStartTime > dwTimeout)
		{
			AfxMessageBox("Wait timed out\n");
			goto stop_cleanup;
		}
	}
	AfxMessageBox("Service stopped successfully\n");
stop_cleanup:
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return bRet;
}

BOOL LoadSys::DeleteSvsA(LPCSTR szSvcName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManagerW(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		AfxMessageBox("MyError OpenSCManager failed(% d)\n", GetLastError());
		return FALSE;
	}

	// Get a handle to the service.

	schService = OpenServiceA(
		schSCManager,       // SCM database 
		szSvcName,          // name of service 
		DELETE);            // need delete access 

	if (schService == NULL)
	{
		AfxMessageBox("MyError OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	// Delete the service.

	if (!DeleteService(schService))
	{
		AfxMessageBox("MyError DeleteService failed (%d)\n", GetLastError());
	}
	else
	{
		AfxMessageBox("MyError Service deleted successfully\n");
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return TRUE;
}

BOOL LoadSys::StopDependentServices(SC_HANDLE schSCManager, SC_HANDLE schService)
{
	DWORD i;
	DWORD dwBytesNeeded;
	DWORD dwCount;

	LPENUM_SERVICE_STATUS   lpDependencies = NULL;
	ENUM_SERVICE_STATUS     ess;
	SC_HANDLE               hDepService;
	SERVICE_STATUS_PROCESS  ssp;

	DWORD dwStartTime = GetTickCount();
	DWORD dwTimeout = 30000; // 30-second time-out

	// Pass a zero-length buffer to get the required buffer size.
	if (EnumDependentServicesA(schService, SERVICE_ACTIVE,
		lpDependencies, 0, &dwBytesNeeded, &dwCount))
	{
		// If the Enum call succeeds, then there are no dependent
		// services, so do nothing.
		return TRUE;
	}
	else
	{
		if (GetLastError() != ERROR_MORE_DATA)
			return FALSE; // Unexpected error

		// Allocate a buffer for the dependencies.
		lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(
			GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

		if (!lpDependencies)
			return FALSE;

		__try {
			// Enumerate the dependencies.
			if (!EnumDependentServicesA(schService, SERVICE_ACTIVE,
				lpDependencies, dwBytesNeeded, &dwBytesNeeded,
				&dwCount))
				return FALSE;

			for (i = 0; i < dwCount; i++)
			{
				ess = *(lpDependencies + i);
				// Open the service.
				hDepService = OpenService(schSCManager,
					ess.lpServiceName,
					SERVICE_STOP | SERVICE_QUERY_STATUS);

				if (!hDepService)
					return FALSE;

				__try {
					// Send a stop code.
					if (!ControlService(hDepService,
						SERVICE_CONTROL_STOP,
						(LPSERVICE_STATUS)&ssp))
						return FALSE;

					// Wait for the service to stop.
					while (ssp.dwCurrentState != SERVICE_STOPPED)
					{
						Sleep(ssp.dwWaitHint);
						if (!QueryServiceStatusEx(
							hDepService,
							SC_STATUS_PROCESS_INFO,
							(LPBYTE)&ssp,
							sizeof(SERVICE_STATUS_PROCESS),
							&dwBytesNeeded))
							return FALSE;

						if (ssp.dwCurrentState == SERVICE_STOPPED)
							break;

						if (GetTickCount() - dwStartTime > dwTimeout)
							return FALSE;
					}
				}
				__finally
				{
					// Always release the service handle.
					CloseServiceHandle(hDepService);
				}
			}
		}
		__finally
		{
			// Always free the enumeration buffer.
			HeapFree(GetProcessHeap(), 0, lpDependencies);
		}
	}
	return TRUE;
}
